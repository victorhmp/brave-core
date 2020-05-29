/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/content_settings/renderer/brave_content_settings_agent_impl_helper.h"

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_RETURN                           \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return;

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_NULLPTR                          \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return nullptr;

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_NULLOPT                          \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return base::nullopt;

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_ZERO                             \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return 0;

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_MINUS_ONE                        \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return -1;

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_SCRIPT_VALUE                     \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return ScriptValue::CreateNull(script_state->GetIsolate());

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_GET_EXTENSION                      \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) { \
    v8::Local<v8::Value> wrapped_extension =                                  \
        ToV8(extension, script_state->GetContext()->Global(),                 \
             script_state->GetIsolate());                                     \
    return ScriptValue(script_state->GetIsolate(), wrapped_extension);        \
  }

#define BRAVE_WEBGL_RENDERING_CONTEXT_BASE_STRING                           \
  if (canvas() && !AllowFingerprinting(canvas()->GetDocument().GetFrame())) \
    return String();

#include "../../../../../../third_party/blink/renderer/modules/webgl/webgl_rendering_context_base.cc.cc"
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_STRING
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_GET_EXTENSION
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_SCRIPT_VALUE
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_MINUS_ONE
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_ZERO
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_NULLOPT
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_NULLPTR
#undef BRAVE_WEBGL_RENDERING_CONTEXT_BASE_RETURN
