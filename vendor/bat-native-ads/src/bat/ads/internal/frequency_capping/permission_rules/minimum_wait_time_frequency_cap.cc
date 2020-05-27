/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/permission_rules/minimum_wait_time_frequency_cap.h"
#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/ad_history.h"
#include "base/time/time.h"
#include "base/strings/stringprintf.h"

namespace ads {

MinimumWaitTimeFrequencyCap::MinimumWaitTimeFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

MinimumWaitTimeFrequencyCap::~MinimumWaitTimeFrequencyCap() = default;

bool MinimumWaitTimeFrequencyCap::IsAllowed() {
  if (ads_->IsMobile()) {
    return true;
  }

  const std::deque<uint64_t> history = GetHistory();
  if (!DoesHistoryRespectCap(history)) {
    const uint64_t minimum_wait_time = GetMinimumWaitTime();

    last_message_ = base::StringPrintf("The minimum wait time of %s seconds "
        "has not elapsed since the last ad was delivered",
            std::to_string(minimum_wait_time).c_str());

    return false;
  }

  return true;
}

const std::string& MinimumWaitTimeFrequencyCap::get_last_message() const {
  return last_message_;
}

std::deque<uint64_t> MinimumWaitTimeFrequencyCap::GetHistory() const {
  std::deque<uint64_t> filtered_history;

  const std::deque<AdHistory> history =
      ads_->get_client()->GetAdsShownHistory();

  for (const auto& ad : history) {
    if (ad.ad_content.ad_action != ConfirmationType::kViewed) {
      continue;
    }

    filtered_history.push_back(ad.timestamp_in_seconds);
  }

  return filtered_history;
}

bool MinimumWaitTimeFrequencyCap::DoesHistoryRespectCap(
    const std::deque<uint64_t>& history) const {
  const uint64_t minimum_wait_time = GetMinimumWaitTime();

  return DoesHistoryRespectCapForRollingTimeConstraint(
      history, minimum_wait_time, 1);
}

uint64_t MinimumWaitTimeFrequencyCap::GetMinimumWaitTime() const {
  return base::Time::kSecondsPerHour / ads_->get_ads_client()->GetAdsPerHour();
}

}  // namespace ads
