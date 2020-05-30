/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser;

public interface BraveRewardsObserver {
  default public void OnWalletInitialized(int error_code) {};
  default public void OnWalletProperties(int error_code) {};
  default public void OnPublisherInfo(int tabId) {};
  default public void OnGetCurrentBalanceReport(double[] report) {};
  default public void OnNotificationAdded(String id, int type, long timestamp,
        String[] args) {};
  default public void OnNotificationsCount(int count) {};
  default public void OnGetLatestNotification(String id, int type, long timestamp,
            String[] args) {};
  default public void OnNotificationDeleted(String id) {};
  default public void OnIsWalletCreated(boolean created) {};
  default public void OnGetPendingContributionsTotal(double amount) {};
  default public void OnGetRewardsMainEnabled(boolean enabled) {};
  default public void OnGetAutoContributeProperties() {};
  default public void OnGetReconcileStamp(long timestamp) {};
  default public void OnRecurringDonationUpdated() {};
  default public void OnResetTheWholeState(boolean success) {};
  default public void OnRewardsMainEnabled(boolean enabled) {};
  default public void OnFetchPromotions() {};
  default public void OnGrantFinish(int result) {};
  default public void OnGetExternalWallet(int error_code,
          String external_wallet) {};
  default public void OnDisconnectWallet(int error_code,
          String external_wallet) {};
  default public void OnProcessRewardsPageUrl(int error_code,
          String wallet_type, String action, String json_args ) {};
  default public void OnClaimPromotion(int error_code) {};
  default public void OnRecoverWallet(int errorCode) {};
}
