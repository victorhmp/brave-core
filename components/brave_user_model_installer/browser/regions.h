/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_REGIONS_H_
#define BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_REGIONS_H_

#include <string>
#include <vector>

#include "base/optional.h"

namespace brave_user_model_installer {

struct RegionInfo {
  std::string region;
  std::string component_base64_public_key;
  std::string component_id;
};

static const std::vector<RegionInfo> _regions = {
    {"US",
      "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA22Pjefa2d1B1Ms3n3554kpGQK9hgnoGgkKnGOODNB9+pwnXIbUBQ0UPNzfxUnqU16++y3JAbmDpLKswlioRrCY8ZX0uhnotU1ZfqtNd48MEPg/DqJGU37XDxa2lxSoUQq3ppGUm6j384Ma90WEAW05ZIwfe9fu1AUpO5RRoad79LG5C+Ol2HbIQQga5YJjpFuAM5KHqbXkrYZfoDOOEAoDiV4YkmZpmsrntB45LoX0eFaQAMkd7wSujzJ261jSRmc5fBpWni3DCWjeVMqYhv40tNAjtPqwwqXEG2p3QO3wlT5LLW6mIw/SXSgecW/fzcA7gKwMsoEIumN13j21WH8wIDAQAB,"  // NOLINT
      "cchgndhfgmkkfmhjhmdenpgdbcdjfmgh" },
    {"GB",
      "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoo+aDr4xxpKJQjgiuN/YDRJA9U5r/oTCgCRjxUBzu+LShtxyfue77RQY+xu0PVZpTznvbR4NLs3jBHiIGDHTzXOFMjO5Pn+8DNKtvNAGjBHoJUvIx6h+fK7++m8IW1RlEd8U1rrjzdfPSh2akzqCY3mM7yk4SXFeN5F+1uFdX9ZJTMWme1gvH4YDziJSjr42AKphTihlFQzddGqgfTdJaTbY7ka6rkdub5w46lle5xw2VNChVhhybnOKSZ+vXAw2yeYUzfik2PZaqAwlrxx6U3AHgv612rcZbHd1SnnTeBm2CWeaAwqa/JCiyXswWi+wHnEppiHSb4UPLHq9elTQRwIDAQAB",  // NOLINT
      "oldkbaailkiinmopalbhaidpjdndifpa" },
};

class Regions {
 public:
  Regions();
  ~Regions();

  static base::Optional<RegionInfo> GetRegionInfo(
      const std::string& country_code);
};

}  // namespace brave_user_model_installer

#endif  // BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_REGIONS_H_
