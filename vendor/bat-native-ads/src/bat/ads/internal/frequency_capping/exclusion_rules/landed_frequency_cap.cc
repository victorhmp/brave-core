/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/strings/stringprintf.h"
#include "base/time/time.h"
#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/frequency_capping/exclusion_rules/landed_frequency_cap.h"
#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"

namespace ads {

LandedFrequencyCap::LandedFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

LandedFrequencyCap::~LandedFrequencyCap() = default;

bool LandedFrequencyCap::ShouldExclude(
    const CreativeAdInfo& ad) {
  if (DoesAdRespectCap(ad)) {
    return false;
  }

  last_message_ = base::StringPrintf("campaignId %s has exceeded the frequency "
      "capping for landed", ad.campaign_id.c_str());

  return true;
}

const std::string& LandedFrequencyCap::get_last_message() const {
  return last_message_;
}

std::deque<uint64_t> LandedFrequencyCap::GetHistory(
    const std::string& campaign_id) const {
  std::deque<uint64_t> filtered_history;

  const std::deque<AdHistory> history =
      ads_->get_client()->GetAdsShownHistory();

  for (const auto& ad : history) {
    if (ad.ad_content.campaign_id != campaign_id ||
        ad.ad_content.ad_action != ConfirmationType::kLanded) {
      continue;
    }

    filtered_history.push_back(ad.timestamp_in_seconds);
  }

  return filtered_history;
}

bool LandedFrequencyCap::DoesAdRespectCap(
    const CreativeAdInfo& ad) const {
  const std::deque<uint64_t> history = GetHistory(ad.campaign_id);

  const uint64_t hour_window = 48 * base::Time::kSecondsPerHour;

  return DoesHistoryRespectCapForRollingTimeConstraint(history, hour_window, 1);
}

}  // namespace ads
