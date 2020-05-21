/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_FREQUENCY_CAPPING_EXCLUSION_RULES_COUNTRY_SUBDIVISION_FREQUENCY_CAP_H_  // NOLINT
#define BAT_ADS_INTERNAL_FREQUENCY_CAPPING_EXCLUSION_RULES_COUNTRY_SUBDIVISION_FREQUENCY_CAP_H_  // NOLINT

#include <string>

#include "bat/ads/internal/frequency_capping/exclusion_rule.h"

namespace ads {

class FrequencyCapping;
class CountrySubdivision;
struct CreativeAdnfo;

class CountrySubdivisionFrequencyCap : public ExclusionRule {
 public:
  CountrySubdivisionFrequencyCap(
      const FrequencyCapping* const frequency_capping,
      const CountrySubdivision* const country_subdivision);

  ~CountrySubdivisionFrequencyCap() override;

  CountrySubdivisionFrequencyCap(
      const CountrySubdivisionFrequencyCap&) = delete;
  CountrySubdivisionFrequencyCap& operator=(
      const CountrySubdivisionFrequencyCap&) = delete;

  // For any ad that is being checked for eligibility with this rule,
  // retrieve its |geo_targets| and return |true| (exclude) if no geo target
  // matches and |false| (include) otherwise.
  bool ShouldExclude(
      const CreativeAdInfo& ad) override;

  std::string GetLastMessage() const override;

 private:
  const FrequencyCapping* const frequency_capping_;  // NOT OWNED
  const CountrySubdivision* const country_subdivision_;  // NOT OWNED

  std::string last_message_;

  bool DoesRespectCap(
      const CreativeAdInfo& ad) const;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_FREQUENCY_CAPPING_EXCLUSION_RULES_COUNTRY_SUBDIVISION_FREQUENCY_CAP_H_  // NOLINT
