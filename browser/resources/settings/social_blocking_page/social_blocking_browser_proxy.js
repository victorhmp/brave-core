/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

cr.define('settings', function() {
  /** @interface */
  class SocialBlockingBrowserProxy {
    /**
     * @param {boolean} enabled (true/false).
     */
    setGoogleLoginEnabled(value) {}
    /**
     * @return {boolean}
     */
    wasGoogleLoginEnabledAtStartup() {}
  }

  /**
   * @implements {settings.SocialBlockingBrowserProxy}
   */
  class SocialBlockingBrowserProxyImpl {
    setGoogleLoginEnabled(value) {
      chrome.send('setGoogleLoginEnabled', [value]);
    }

    wasGoogleLoginEnabledAtStartup() {
      return loadTimeData.getBoolean('googleLoginEnabledAtStartup');
    }
  }

  cr.addSingletonGetter(SocialBlockingBrowserProxyImpl);

  // #cr_define_end
  return {
    SocialBlockingBrowserProxy,
    SocialBlockingBrowserProxyImpl
  };
});
