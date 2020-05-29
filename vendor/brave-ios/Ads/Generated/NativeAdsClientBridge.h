/* WARNING: THIS FILE IS GENERATED. ANY CHANGES TO THIS FILE WILL BE OVERWRITTEN
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#import <Foundation/Foundation.h>
#import "bat/ads/ads_client.h"

@protocol NativeAdsClientBridge
@required

- (void)confirmAd:(const ads::AdInfo &)info confirmationType:(const ads::ConfirmationType)confirmation_type;
- (void)confirmAction:(const std::string &)uuid creativeSetId:(const std::string &)creative_set_id confirmationType:(const ads::ConfirmationType &)confirmation_type;
- (void)getCreativeAdNotifications:(const std::vector<std::string> &)categories callback:(ads::GetCreativeAdNotificationsCallback)callback;
- (void)getAdConversions:(ads::GetAdConversionsCallback)callback;
- (uint64_t)getAdsPerDay;
- (uint64_t)getAdsPerHour;
- (void)getClientInfo:(ads::ClientInfo *)info;
- (std::vector<std::string>)getUserModelLanguages;
- (bool)isAdsEnabled;
- (bool)shouldAllowAdConversionTracking;
- (bool)isForeground;
- (bool)canShowBackgroundNotifications;
- (bool)isNetworkConnectionAvailable;
- (bool)shouldShowNotifications;
- (void)load:(const std::string &)name callback:(ads::LoadCallback)callback;
- (std::string)loadJsonSchema:(const std::string &)name;
- (void)loadUserModelForLanguage:(const std::string &)language callback:(ads::LoadCallback)callback;
- (void)log:(const char *)file line:(const int)line verboseLevel:(const int)verbose_level message:(const std::string &) message;
- (void)reset:(const std::string &)name callback:(ads::ResultCallback)callback;
- (void)save:(const std::string &)name value:(const std::string &)value callback:(ads::ResultCallback)callback;
- (void)saveBundleState:(std::unique_ptr<ads::BundleState>)state callback:(ads::ResultCallback)callback;
- (void)setCatalogIssuers:(std::unique_ptr<ads::IssuersInfo>)info;
- (void)setIdleThreshold:(const int)threshold;
- (void)showNotification:(std::unique_ptr<ads::AdNotificationInfo>)info;
- (void)closeNotification:(const std::string&)id;
- (void)URLRequest:(const std::string &)url headers:(const std::vector<std::string> &)headers content:(const std::string &)content contentType:(const std::string &)content_type method:(const ads::URLRequestMethod)method callback:(ads::URLRequestCallback)callback;
- (bool)shouldAllowSubdivisionAdTargeting;
- (std::string)getCountrySubdivision;
- (void)setCountrySubdivision:(const std::string &)country_subdivision;
- (bool)didOverrideAdsSubdivision;
- (void)setSubdivisionAdTargetingRegion:(bool)is_region;

@end
