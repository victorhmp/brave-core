/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/sync/driver/sync_session_durations_metrics_recorder.h"

#define BRAVE_NO_SYNC_METRICS return;

#include "../../../../../components/sync/driver/sync_session_durations_metrics_recorder.cc"
#undef BRAVE_NO_SYNC_METRICS
