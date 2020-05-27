/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/permission_rules/ads_per_day_frequency_cap.h"
#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"
#include "bat/ads/ad_history.h"
#include "bat/ads/internal/ads_impl.h"
#include "base/time/time.h"

namespace ads {

AdsPerDayFrequencyCap::AdsPerDayFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

AdsPerDayFrequencyCap::~AdsPerDayFrequencyCap() = default;

bool AdsPerDayFrequencyCap::IsAllowed() {
  if (!DoesHistoryRespectCap()) {
    last_message_ = "You have exceeded the allowed ads per day";

    return false;
  }

  return true;
}

const std::string& AdsPerDayFrequencyCap::get_last_message() const {
  return last_message_;
}

std::deque<uint64_t> AdsPerDayFrequencyCap::GetHistory() const {
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

bool AdsPerDayFrequencyCap::DoesHistoryRespectCap() const {
  const std::deque<uint64_t> history = GetHistory();

  const uint64_t day_window =
      base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

  const uint64_t allowed_ads_per_day = ads_->get_ads_client()->GetAdsPerDay();

  return DoesHistoryRespectCapForRollingTimeConstraint(
      history, day_window, allowed_ads_per_day);
}

}  // namespace ads
