/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/exclusion_rules/conversion_frequency_cap.h"

#include <stdint.h>

#include <deque>
#include <map>

#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/frequency_capping/frequency_capping.h"
#include "bat/ads/internal/logging.h"

#include "base/strings/stringprintf.h"

namespace ads {

ConversionFrequencyCap::ConversionFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

ConversionFrequencyCap::~ConversionFrequencyCap() = default;

bool ConversionFrequencyCap::ShouldExclude(
    const CreativeAdInfo& ad) {
  if (DoesRespectCap(ad)) {
    return false;
  }

  last_message_ = base::StringPrintf("creativeSetId %s has exceeded the "
      "frequency capping for conversions", ad.creative_set_id.c_str());

  return true;
}

const std::string& ConversionFrequencyCap::get_last_message() const {
  return last_message_;
}

bool ConversionFrequencyCap::DoesRespectCap(
      const CreativeAdInfo& ad) const {
  const std::map<std::string, std::deque<uint64_t>> history =
      ads_->get_client()->GetAdConversionHistory();

  std::deque<uint64_t> filtered_history;

  if (history.find(ad.creative_set_id) != history.end()) {
    filtered_history = history.at(ad.creative_set_id);
  }

  if (filtered_history.size() >= 1) {
    return false;
  }

  return true;
}

}  // namespace ads
