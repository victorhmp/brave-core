/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_ADS_LOCALE_HELPER_H_
#define BAT_ADS_INTERNAL_ADS_LOCALE_HELPER_H_

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/timer.h"
#include "bat/ads/internal/retry_timer.h"

namespace ads {

class AdsImpl;

class AdsLocaleHelper {
 public:
  AdsLocaleHelper(
      AdsClient* ads_client);

  ~AdsLocaleHelper();

  void GetLocale();

 private:
  void BuildUrl();

  void OnLocaleReceived(
      const std::string& url,
      const int response_status_code,
      const std::string& response,
      const std::map<std::string, std::string>& headers);

  bool ProcessLocale(
    const std::string& json);

  void RetryGettingLocale();

  void GetLocaleAfterDelay();

  AdsClient* ads_client_;  // NOT OWNED
  Timer timer_;
  RetryTimer retry_timer_;
  std::string url_;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_ADS_LOCALE_HELPER_H_
