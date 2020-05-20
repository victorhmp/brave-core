/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "bat/ads/internal/ads_locale_helper.h"

#include "base/time/time.h"
#include "bat/ads/internal/static_values.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/time_util.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace ads {

AdsLocaleHelper::AdsLocaleHelper(
    AdsClient* ads_client)
    : ads_client_(ads_client) {
  BuildUrl();
}

AdsLocaleHelper::~AdsLocaleHelper() = default;

void AdsLocaleHelper::GetLocale() {
  if (retry_timer_.IsRunning()) {
    return;
  }

  BLOG(1, "Get ads locale");
  BLOG(2, "GET " << GETSTATE_PATH);

  auto callback = std::bind(&AdsLocaleHelper::OnLocaleReceived,
      this, url_, _1, _2, _3);
  ads_client_->URLRequest(url_, {}, "", "", URLRequestMethod::GET, callback);

  return;
}

void AdsLocaleHelper::OnLocaleReceived(
    const std::string& url,
    const int response_status_code,
    const std::string& response,
    const std::map<std::string, std::string>& headers) {
  BLOG(7, UrlResponseToString(url, response_status_code, response, headers));

  auto should_retry = false;

  if (response_status_code / 100 == 2) {
    if (!response.empty()) {
      BLOG(1, "Successfully received locale");
    }

    if (!ProcessLocale(response)) {
      should_retry = true;
    }
  } else if (response_status_code == 304) {
    BLOG(1, "Locale is up to date");
  } else {
    BLOG(1, "Failed to get locale");

    should_retry = true;
  }

  // TODO(Moritz Haller): how to manually test? -> tested with charles and
  // map remote
  if (should_retry) {
    RetryGettingLocale();
    return;
  }

  retry_timer_.Stop();

  GetLocaleAfterDelay();
}

bool AdsLocaleHelper::ProcessLocale(
    const std::string& json) {
  BLOG(1, "Parsing locale " << json);
  return true;
}

void AdsLocaleHelper::RetryGettingLocale() {
  const base::Time time = retry_timer_.StartWithBackoff(
      kRetryGettingStateAfterSeconds,
          base::BindOnce(&AdsLocaleHelper::GetLocale, base::Unretained(this)));

  BLOG(1, "Retry getting locale " << FriendlyDateAndTime(time));
}

void AdsLocaleHelper::GetLocaleAfterDelay() {
  const uint64_t delay = _is_debug ? kDebugGetStatePing :
      kDefaultGetStatePing;

  const base::Time time = timer_.StartWithPrivacy(delay,
      base::BindOnce(&AdsLocaleHelper::GetLocale, base::Unretained(this)));

  BLOG(1, "Download catalog " << FriendlyDateAndTime(time));
}

void AdsLocaleHelper::BuildUrl() {
  switch (_environment) {
    case Environment::PRODUCTION: {
      url_ = PRODUCTION_SERVER;
      break;
    }

    case Environment::STAGING: {
      url_ = STAGING_SERVER;
      break;
    }

    case Environment::DEVELOPMENT: {
      url_ = DEVELOPMENT_SERVER;
      break;
    }
  }

  url_ += GETSTATE_PATH;
}

}  // namespace ads
