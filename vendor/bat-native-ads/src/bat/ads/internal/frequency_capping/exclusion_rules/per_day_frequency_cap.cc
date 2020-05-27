/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <map>

#include "base/strings/stringprintf.h"
#include "base/time/time.h"
#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/frequency_capping/exclusion_rules/per_day_frequency_cap.h"
#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"

namespace ads {

PerDayFrequencyCap::PerDayFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

PerDayFrequencyCap::~PerDayFrequencyCap() = default;

bool PerDayFrequencyCap::ShouldExclude(
    const CreativeAdInfo& ad) {
  if (DoesAdRespectCap(ad)) {
    return false;
  }

  last_message_ = base::StringPrintf("creativeSetId %s has exceeded the "
      "frequency capping for perDay", ad.creative_set_id.c_str());

  return true;
}

const std::string& PerDayFrequencyCap::get_last_message() const {
  return last_message_;
}

std::deque<uint64_t> PerDayFrequencyCap::GetHistory(
    const std::string& creative_set_id) const {
  std::deque<uint64_t> filtered_history;

  const std::map<std::string, std::deque<uint64_t>> history =
      ads_->get_client()->GetCreativeSetHistory();
  if (history.find(creative_set_id) != history.end()) {
    filtered_history = history.at(creative_set_id);
  }

  return filtered_history;
}

bool PerDayFrequencyCap::DoesAdRespectCap(
    const CreativeAdInfo& ad) const {
  const std::deque<uint64_t> history = GetHistory(ad.creative_set_id);

  const uint64_t day_window =
      base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

  return DoesHistoryRespectCapForRollingTimeConstraint(history, day_window,
      ad.per_day);
}

}  // namespace ads
