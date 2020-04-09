/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_user_model_installer/browser/regions.h"

namespace brave_user_model_installer {

base::Optional<RegionInfo> Regions::GetRegionInfo(
    const std::string& country_code) {
  for (const auto& info : _regions) {
    if (info.region == country_code)
      return info;
  }

  return base::nullopt;
}

}  // namespace brave_user_model_installer
