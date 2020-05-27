/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

#include "base/guid.h"
#include "base/time/time.h"
#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/frequency_capping/exclusion_rule.h"
#include "bat/ads/internal/frequency_capping/exclusion_rules/per_hour_frequency_cap.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

using std::placeholders::_1;

namespace ads {

namespace {
const char kCreativeInstanceId[] = "9aea9a47-c6a0-4718-a0fa-706338bb2156";
}  // namespace

class BraveAdsPerHourFrequencyCapTest : public ::testing::Test {
 protected:
  BraveAdsPerHourFrequencyCapTest()
      : ads_client_mock_(std::make_unique<AdsClientMock>()),
        ads_(std::make_unique<AdsImpl>(ads_client_mock_.get())) {
    // You can do set-up work for each test here
  }

  ~BraveAdsPerHourFrequencyCapTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(
        &BraveAdsPerHourFrequencyCapTest::OnAdsImplInitialize, this, _1);
    ads_->Initialize(callback);

    frequency_cap_ = std::make_unique<PerHourFrequencyCap>(ads_.get());
  }

  void OnAdsImplInitialize(const Result result) {
    EXPECT_EQ(Result::SUCCESS, result);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  void GeneratePastHistory(
      const std::string& creative_instance_id,
      const uint64_t time_offset_in_seconds,
      const uint8_t count) {
    uint64_t now_in_seconds = base::Time::Now().ToDoubleT();

    for (uint8_t i = 0; i < count; i++) {
      now_in_seconds -= time_offset_in_seconds;

      AdHistory history;
      history.uuid = base::GenerateGUID();
      history.ad_content.creative_instance_id = creative_instance_id;
      history.timestamp_in_seconds = now_in_seconds;
      history.ad_content.ad_action = ConfirmationType::kViewed;

      ads_->get_client()->AppendToAdsShownHistory(history);
    }
  }

  std::unique_ptr<AdsClientMock> ads_client_mock_;
  std::unique_ptr<AdsImpl> ads_;

  std::unique_ptr<PerHourFrequencyCap> frequency_cap_;
};

TEST_F(BraveAdsPerHourFrequencyCapTest,
    AdAllowedWhenNoAds) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.creative_instance_id = kCreativeInstanceId;

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsPerHourFrequencyCapTest,
    AdAllowedOverTheHour) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.creative_instance_id = kCreativeInstanceId;

  GeneratePastHistory(kCreativeInstanceId, base::Time::kSecondsPerHour, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsPerHourFrequencyCapTest,
    AdExcludedWithPastAdsJustWithinTheHour) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.creative_instance_id = kCreativeInstanceId;

  GeneratePastHistory(kCreativeInstanceId, base::Time::kSecondsPerHour - 1, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_TRUE(should_exclude);
  EXPECT_EQ(frequency_cap_->get_last_message(), "creativeInstanceId 9aea9a47-c6a0-4718-a0fa-706338bb2156 has exceeded the frequency capping for perHour");  // NOLINT
}

TEST_F(BraveAdsPerHourFrequencyCapTest,
    AdExcludedWithPastAdWithinTheHour) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.creative_instance_id = kCreativeInstanceId;

  GeneratePastHistory(kCreativeInstanceId, 0, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_TRUE(should_exclude);
  EXPECT_EQ(frequency_cap_->get_last_message(), "creativeInstanceId 9aea9a47-c6a0-4718-a0fa-706338bb2156 has exceeded the frequency capping for perHour");  // NOLINT
}

}  // namespace ads
