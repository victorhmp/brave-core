/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/exclusion_rules/country_subdivision_frequency_cap.h"

#include "base/strings/stringprintf.h"

#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/country_subdivision.h"
#include "bat/ads/internal/frequency_capping/frequency_capping.h"
#include "bat/ads/internal/logging.h"

namespace ads {

CountrySubdivisionFrequencyCap::CountrySubdivisionFrequencyCap(
    const FrequencyCapping* const frequency_capping,
    const CountrySubdivision* const country_subdivision)
    : frequency_capping_(frequency_capping),
      country_subdivision_(country_subdivision) {
  DCHECK(frequency_capping_);
  DCHECK(country_subdivision_);
}

CountrySubdivisionFrequencyCap::~CountrySubdivisionFrequencyCap() = default;

bool CountrySubdivisionFrequencyCap::ShouldExclude(
    const CreativeAdInfo& ad) {
  if (!DoesRespectCap(ad)) {
    last_message_ = base::StringPrintf("creativeSetId %s did not pass the "
        "exclusion rule for country subdivision targeting",
            ad.creative_set_id.c_str());

    return true;
  }

  return false;
}

std::string CountrySubdivisionFrequencyCap::GetLastMessage() const {
  return last_message_;
}

bool CountrySubdivisionFrequencyCap::DoesRespectCap(
      const CreativeAdInfo& ad) const {
  const std::string subdivision = country_subdivision_->GetCountrySubdivision();
  for (const auto& geo_target : ad.geo_targets) {
    // Only if it is a valid subdivision code, does it have to match with
    // the code stored in preferences. It it is not a valid code, it is
    // assumed to be a country code and this cap is considered to be respected
    if (!country_subdivision_->IsValidCode(geo_target)) {
      return true;
    }

    if (!subdivision.compare(geo_target)) {
      return true;
    }
  }

  return false;
}

}  // namespace ads
