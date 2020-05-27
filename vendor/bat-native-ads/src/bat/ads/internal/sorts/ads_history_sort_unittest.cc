/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/sorts/ads_history_sort_factory.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=BatAds*

using std::placeholders::_1;

namespace ads {

class BatAdsHistorySortTest : public ::testing::Test {
 protected:
  BatAdsHistorySortTest()
      : ads_client_mock_(std::make_unique<AdsClientMock>()),
        ads_(std::make_unique<AdsImpl>(ads_client_mock_.get())) {
    // You can do set-up work for each test here
  }

  ~BatAdsHistorySortTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(
        &BatAdsHistorySortTest::OnAdsImplInitialize, this, _1);
    ads_->Initialize(callback);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  void OnAdsImplInitialize(const Result result) {
    EXPECT_EQ(Result::SUCCESS, result);
  }

  std::deque<AdHistory> GetUnsortedHistory() {
    AdContent ad_content;
    CategoryContent category_content;

    std::deque<AdHistory> history;
    AdHistory ad_history;
    ad_history.timestamp_in_seconds = 22222222222;
    history.push_back(ad_history);
    ad_history.timestamp_in_seconds = 33333333333;
    history.push_back(ad_history);
    ad_history.timestamp_in_seconds = 11111111111;
    history.push_back(ad_history);
    ad_history.timestamp_in_seconds = 55555555555;
    history.push_back(ad_history);
    ad_history.timestamp_in_seconds = 44444444444;
    history.push_back(ad_history);

    return history;
  }

  std::unique_ptr<AdsClientMock> ads_client_mock_;
  std::unique_ptr<AdsImpl> ads_;
};

TEST_F(BatAdsHistorySortTest,
    NoSortOrder) {
  // Arrange

  // Act
  const auto sort = AdsHistorySortFactory::Build(AdsHistory::SortType::kNone);

  // Assert
  ASSERT_EQ(sort, nullptr);
}

TEST_F(BatAdsHistorySortTest,
    DescendingSortOrder) {
  // Arrange
  const auto sort =
      AdsHistorySortFactory::Build(AdsHistory::SortType::kDescendingOrder);

  std::deque<AdHistory> expected_history;
  AdHistory ad_history;
  ad_history.timestamp_in_seconds = 55555555555;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 44444444444;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 33333333333;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 22222222222;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 11111111111;
  expected_history.push_back(ad_history);

  auto history = GetUnsortedHistory();

  // Act
  history = sort->Apply(history);

  // Assert
  ASSERT_EQ(expected_history, history);
}

TEST_F(BatAdsHistorySortTest,
    DescendingSortOrderForEmptyHistory) {
  // Arrange
  const auto sort =
      AdsHistorySortFactory::Build(AdsHistory::SortType::kDescendingOrder);

  std::deque<AdHistory> expected_history;
  std::deque<AdHistory> history;

  // Act
  history = sort->Apply(history);

  // Assert
  ASSERT_EQ(expected_history, history);
}

TEST_F(BatAdsHistorySortTest,
    AscendingSortOrder) {
  // Arrange
  const auto sort =
      AdsHistorySortFactory::Build(AdsHistory::SortType::kAscendingOrder);

  std::deque<AdHistory> expected_history;
  AdHistory ad_history;
  ad_history.timestamp_in_seconds = 11111111111;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 22222222222;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 33333333333;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 44444444444;
  expected_history.push_back(ad_history);
  ad_history.timestamp_in_seconds = 55555555555;
  expected_history.push_back(ad_history);

  auto history = GetUnsortedHistory();

  // Act
  history = sort->Apply(history);

  // Assert
  ASSERT_EQ(expected_history, history);
}

TEST_F(BatAdsHistorySortTest,
    AscendingSortOrderForEmptyHistory) {
  // Arrange
  const auto sort =
      AdsHistorySortFactory::Build(AdsHistory::SortType::kAscendingOrder);

  std::deque<AdHistory> expected_history;
  std::deque<AdHistory> history;

  // Act
  history = sort->Apply(history);

  // Assert
  ASSERT_EQ(expected_history, history);
}

}  // namespace ads
