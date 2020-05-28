/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/signin/internal/identity_manager/primary_account_manager.h"

#include "extensions/common/url_pattern.h"
#include "google_apis/gaia/gaia_urls.h"
#include "url/gurl.h"


#define BRAVE_NO_GAIA_REQUEST_ON_STARTUP                  \
  if (account_tracker_service_->GetAccounts().empty()) {  \
    return;                                               \
  }                                                       \

#include "../../../../../../components/signin/internal/identity_manager/primary_account_manager.cc"
#undef BRAVE_NO_GAIA_REQUEST_ON_STARTUP
