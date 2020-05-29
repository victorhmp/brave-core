//
//  ads_wrapper_tests.m
//  Sources
//
//  Created by Kyle Hickinson on 2019-06-28.
//

#import <XCTest/XCTest.h>
#import "BATBraveRewards.h"

@interface _MockNotificationHandler : NSObject <BATBraveAdsNotificationHandler>
@property (nonatomic, copy, nullable) void (^showNotification)(BATAdNotification *);
@property (nonatomic, copy, nullable) void (^clearNotification)(NSString *);
@end

@implementation _MockNotificationHandler
- (BOOL)shouldShowNotifications
{
  return YES;
}
- (void)showNotification:(BATAdNotification *)notification
{
  if (self.showNotification) {
    self.showNotification(notification);
  }
}
- (void)clearNotificationWithIdentifier:(NSString *)identifier
{
  if (self.clearNotification) {
    self.clearNotification(identifier);
  }
}
@end

@interface AdsWrapperTest : XCTestCase
@property (nonatomic) BATBraveAds *ads;
@end

@implementation AdsWrapperTest

- (NSString *)stateStoragePath
{
  return [NSTemporaryDirectory() stringByAppendingPathComponent:@"ads"];
}

- (void)setUp
{
  [BATBraveAds setDebug:YES];
  const auto path = [self stateStoragePath];
  [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
  self.ads = [[BATBraveAds alloc] initWithStateStoragePath:path];
}

- (void)tearDown
{
  self.ads = nil;
}

- (void)testPreferencePersistance
{
  const auto expect = [self expectationWithDescription:@"File IO"];
  self.ads.enabled = NO;
  self.ads.allowSubdivisionTargeting = YES;
  self.ads.numberOfAllowableAdsPerDay = 10;
  self.ads.numberOfAllowableAdsPerHour = 6;
  
  dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
    BATBraveAds *secondAds = [[BATBraveAds alloc] initWithStateStoragePath:[self stateStoragePath]];
    XCTAssertEqual(self.ads.enabled, secondAds.enabled);
    XCTAssertEqual(self.ads.shouldAllowSubdivisionAdTargeting, secondAds.shouldAllowSubdivisionAdTargeting);
    XCTAssertEqual(self.ads.numberOfAllowableAdsPerDay, secondAds.numberOfAllowableAdsPerDay);
    XCTAssertEqual(self.ads.numberOfAllowableAdsPerHour, secondAds.numberOfAllowableAdsPerHour);
    
    [expect fulfill];
  });
  
  [self waitForExpectations:@[expect] timeout: 4.0];
}

@end
