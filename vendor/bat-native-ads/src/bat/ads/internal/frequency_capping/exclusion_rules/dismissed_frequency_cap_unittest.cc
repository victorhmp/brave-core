/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"

#include "base/guid.h"
#include "base/time/time.h"
#include "bat/ads/creative_ad_info.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/frequency_capping/exclusion_rule.h"
#include "bat/ads/internal/frequency_capping/exclusion_rules/dismissed_frequency_cap.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

using std::placeholders::_1;

namespace ads {

namespace {

const std::vector<std::string> kCampaignIds = {
  "60267cee-d5bb-4a0d-baaf-91cd7f18e07e",
  "90762cee-d5bb-4a0d-baaf-61cd7f18e07e"
};

const uint64_t kSecondsPerDay =
    base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

}  // namespace

class BraveAdsDismissedFrequencyCapTest : public ::testing::Test {
 protected:
  BraveAdsDismissedFrequencyCapTest()
      : ads_client_mock_(std::make_unique<AdsClientMock>()),
        ads_(std::make_unique<AdsImpl>(ads_client_mock_.get())) {
    // You can do set-up work for each test here
  }

  ~BraveAdsDismissedFrequencyCapTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(
        &BraveAdsDismissedFrequencyCapTest::OnAdsImplInitialize, this, _1);
    ads_->Initialize(callback);

    frequency_cap_ = std::make_unique<DismissedFrequencyCap>(ads_.get());
  }

  void OnAdsImplInitialize(const Result result) {
    EXPECT_EQ(Result::SUCCESS, result);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  void GeneratePastHistory(
      const std::string& campaign_id,
      const ConfirmationType confirmation_type,
      const uint64_t time_offset_in_seconds,
      const uint8_t count) {
    uint64_t now_in_seconds = base::Time::Now().ToDoubleT();

    for (uint8_t i = 0; i < count; i++) {
      now_in_seconds -= time_offset_in_seconds;

      AdHistory history;
      history.uuid = base::GenerateGUID();
      history.ad_content.campaign_id = campaign_id;
      history.timestamp_in_seconds = now_in_seconds;
      history.ad_content.ad_action = confirmation_type;

      ads_->get_client()->AppendToAdsShownHistory(history);
    }
  }

  std::unique_ptr<AdsClientMock> ads_client_mock_;
  std::unique_ptr<AdsImpl> ads_;

  std::unique_ptr<DismissedFrequencyCap> frequency_cap_;
};

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdAllowedWhenNoAds) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdAllowedForAdsWithDifferentCampaignId) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  GeneratePastHistory(kCampaignIds.at(1), ConfirmationType::kDismissed, 0, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdAllowedForAdWithDifferentCampaignIdWithin48Hours) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  const uint64_t time_offset = (2 * kSecondsPerDay) - 1;
  GeneratePastHistory(kCampaignIds.at(1), ConfirmationType::kDismissed,
      time_offset, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdNotAllowedForAdWithSameCampaignIdWithin48Hours) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  const uint64_t time_offset = (2 * kSecondsPerDay) - 1;
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed,
      time_offset, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_TRUE(should_exclude);
  EXPECT_EQ(frequency_cap_->get_last_message(), "campaignId 60267cee-d5bb-4a0d-baaf-91cd7f18e07e has exceeded the frequency capping for dismissed");  // NOLINT
}

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdAllowedForAdWithSameCampaignIdAfter48Hours) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  const uint64_t time_offset = 2 * kSecondsPerDay;
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed,
      time_offset, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

TEST_F(BraveAdsDismissedFrequencyCapTest,
    AdAllowedForAdWithDifferentCampaignIdAfter48Hours) {
  // Arrange
  CreativeAdInfo ad_info;
  ad_info.campaign_id = kCampaignIds.at(0);
  GeneratePastHistory(ad_info.campaign_id, ConfirmationType::kDismissed, 0, 1);

  const uint64_t time_offset = 2 * kSecondsPerDay;
  GeneratePastHistory(kCampaignIds.at(1), ConfirmationType::kDismissed,
      time_offset, 1);

  // Act
  const bool should_exclude = frequency_cap_->ShouldExclude(ad_info);

  // Assert
  EXPECT_FALSE(should_exclude);
}

}  // namespace ads
