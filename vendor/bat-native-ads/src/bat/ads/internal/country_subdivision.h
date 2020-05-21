/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_COUNTRY_SUBDIVISION_H_
#define BAT_ADS_INTERNAL_COUNTRY_SUBDIVISION_H_

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/retry_timer.h"
#include "bat/ads/internal/timer.h"

namespace ads {

class AdsImpl;

class CountrySubdivision {
 public:
  CountrySubdivision(
      AdsClient* ads_client);

  ~CountrySubdivision();

  void FetchCountrySubdivisionIfAllowedForRegion();

  std::string GetCountrySubdivision() const;

  bool IsValidCode(
    const std::string& country_subdivision_code) const;

 private:
  void BuildUrl();

  // Returns the client's inferred country subdivision (as definied in
  // ISO 3166-2 https://en.wikipedia.org/wiki/ISO_3166-2)
  void Fetch();

  void OnReceived(
      const std::string& url,
      const int response_status_code,
      const std::string& response,
      const std::map<std::string, std::string>& headers);

  bool Process(
    const std::string& json);

  void Retry();

  void FetchAfterDelay();

  AdsClient* ads_client_;  // NOT OWNED
  Timer timer_;
  RetryTimer retry_timer_;
  std::string url_;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_COUNTRY_SUBDIVISION_H_
