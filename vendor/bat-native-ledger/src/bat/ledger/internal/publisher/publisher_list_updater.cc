/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/publisher/publisher_list_updater.h"

#include <memory>
#include <utility>

#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/publisher/publisher_list_reader.h"
#include "bat/ledger/internal/request/request_publisher.h"
#include "bat/ledger/internal/state/state_keys.h"
#include "bat/ledger/option_keys.h"
#include "brave_base/random.h"
#include "net/http/http_status_code.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace {

constexpr double kRetryAfterFailureDelay = 150.0;
constexpr uint64_t kMaxRetryAfterFailureDelay = 60 * 60 * 4;

}  // namespace

namespace braveledger_publisher {

PublisherListUpdater::PublisherListUpdater(bat_ledger::LedgerImpl* ledger)
    : ledger_(ledger) {}

PublisherListUpdater::~PublisherListUpdater() = default;

void PublisherListUpdater::StartAutoUpdate() {
  auto_update_ = true;
  if (!timer_.IsRunning()) {
    StartFetchTimer(FROM_HERE, GetAutoUpdateDelay());
  }
}

void PublisherListUpdater::StopAutoUpdate() {
  BLOG(1, "Cancelling publisher prefix list update");
  auto_update_ = false;
  timer_.Stop();
}

void PublisherListUpdater::StartFetchTimer(
    const base::Location& posted_from,
    base::TimeDelta delay) {
  BLOG(1, "Scheduling publisher prefix list update in "
      << delay.InSeconds() << " seconds");
  timer_.Start(posted_from, delay, base::BindOnce(
      &PublisherListUpdater::OnFetchTimerElapsed,
      base::Unretained(this)));
}

void PublisherListUpdater::OnFetchTimerElapsed() {
  BLOG(1, "Fetching publisher prefix list");
  std::string url = braveledger_request_util::GetPublisherListUrl();
  ledger_->LoadURL(
      url, {}, "", "",
      ledger::UrlMethod::GET,
      std::bind(&PublisherListUpdater::OnFetchCompleted, this, _1));
}

void PublisherListUpdater::OnFetchCompleted(
    const ledger::UrlResponse& response) {
  if (response.status_code != net::HTTP_OK || response.body.empty()) {
    BLOG(0, "Invalid server response for publisher prefix list");
    StartFetchTimer(FROM_HERE, GetRetryAfterFailureDelay());
    return;
  }

  auto reader = std::make_unique<PublisherListReader>();
  auto parse_error = reader->Parse(response.body);
  if (parse_error != PublisherListReader::ParseError::None) {
    // This could be a problem on the client or the server, but
    // optimistically assume that it is a server issue and retry
    // with back-off.
    BLOG(0, "Failed to parse publisher prefix list: "
        << static_cast<int>(parse_error));
    StartFetchTimer(FROM_HERE, GetRetryAfterFailureDelay());
    return;
  }

  if (reader->size() == 0) {
    BLOG(0, "Publisher prefix list did not contain any values");
    StartFetchTimer(FROM_HERE, GetRetryAfterFailureDelay());
    return;
  }

  retry_count_ = 0;

  // At this point we have received a valid response from the server.
  // Store last successful fetch time for calculation of next refresh
  // interval.
  ledger_->SetUint64State(
      ledger::kStateServerPublisherListStamp,
      static_cast<uint64_t>(base::Time::Now().ToDoubleT()));

  BLOG(1, "Resetting publisher prefix list table");

  ledger_->ResetPublisherList(std::move(reader),
      [this](ledger::Result result) {
        if (result != ledger::Result::LEDGER_OK) {
          BLOG(0, "Error updating publisher prefix list table: " << result);
          return;
        }

        // NOTE: Ideally this component would not know about contributions
        // and we would provide a callback mechanism for users to specify
        // an "OnUpdate" action.

        // Attempt to reprocess any contributions for previously
        // unverified publishers that are now verified.
        ledger_->ContributeUnverifiedPublishers();
      });

  if (auto_update_) {
    StartFetchTimer(FROM_HERE, GetAutoUpdateDelay());
  }
}

base::TimeDelta PublisherListUpdater::GetAutoUpdateDelay() {
  uint64_t last_fetch_sec =
      ledger_->GetUint64State(ledger::kStateServerPublisherListStamp);
  uint64_t interval_sec =
      ledger_->GetUint64Option(ledger::kOptionPublisherListRefreshInterval);

  auto now = base::Time::Now();
  auto fetch_time = base::Time::FromDoubleT(
      static_cast<double>(last_fetch_sec));

  if (fetch_time > now) {
    fetch_time = now;
  }

  fetch_time += base::TimeDelta::FromSeconds(interval_sec);
  return fetch_time < now
      ? base::TimeDelta::FromSeconds(0)
      : fetch_time - now;
}

base::TimeDelta PublisherListUpdater::GetRetryAfterFailureDelay() {
  uint64_t seconds = brave_base::random::Geometric(kRetryAfterFailureDelay);
  for (int i = 0; i < retry_count_; ++i) {
    uint64_t next = seconds << 1;
    if (next > kMaxRetryAfterFailureDelay) {
      break;
    }
    seconds = next;
  }
  retry_count_ += 1;
  return base::TimeDelta::FromSeconds(seconds);
}

}  // namespace braveledger_publisher
