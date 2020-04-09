/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdint.h>
#include <memory>
#include <tuple>

#include "base/time/time.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/time_util.h"
#include "bat/ads/internal/static_values.h"
#include "bat/ads/internal/purchase_intent/purchase_intent_classifier.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;

// npm run test -- brave_unit_tests --filter=AdsPurchaseIntentClassifier*

namespace ads {

class AdsPurchaseIntentClassifierTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockAdsClient> mock_ads_client_;
  std::unique_ptr<AdsImpl> ads_;

  AdsPurchaseIntentClassifierTest() :
      mock_ads_client_(std::make_unique<MockAdsClient>()),
      ads_(std::make_unique<AdsImpl>(mock_ads_client_.get())) {
    // You can do set-up work for each test here
  }

  ~AdsPurchaseIntentClassifierTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)
    constexpr char model_json[] = R"(
    {
      "locale": "gb",
      "version": 1,
      "timestamp": "2020-05-15 00:00:00",
      "parameters": {
        "signal_level": 1,
        "classification_threshold": 10,
        "signal_decay_time_window_in_seconds": 100
      },
      "segments": [
        "segment 1", "segment 2", "segment 3"
      ],
      "segment_keywords": {
        "segment keyword 1": [0],
        "segment keyword 2": [0, 1]
      },
      "funnel_keywords": {
        "funnel keyword 1": 2,
        "funnel keyword 2": 3
      },
      "funnel_sites": [
        {
          "sites": [
            "http://brave.com", "http://crave.com"
          ],
          "segments": [1, 2]
        },
        {
          "sites": [
            "http://frexample.org", "http://example.org"
          ],
          "segments": [0]
        }
      ]
    })";

    purchase_intent_classifier_ = std::make_unique<PurchaseIntentClassifier>();
    purchase_intent_classifier_->Initialize(model_json);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  // Objects declared here can be used by all tests in the test case
  std::unique_ptr<PurchaseIntentClassifier> purchase_intent_classifier_;
};

TEST_F(AdsPurchaseIntentClassifierTest, InitializeClassifier) {
  EXPECT_TRUE(purchase_intent_classifier_->IsInitialized());
}

TEST_F(AdsPurchaseIntentClassifierTest, ExtractSignal) {
  // Case 1: Match Funnel Site
  // Arrange
  std::string url_1 = "https://www.brave.com/test?foo=bar";

  // Act
  const PurchaseIntentSignalInfo info_1 =
      purchase_intent_classifier_->ExtractIntentSignal(url_1);

  // Assert
  const PurchaseIntentSegmentList gold_segments_1({"segment 2", "segment 3"});
  EXPECT_EQ(info_1.segments, gold_segments_1);
  EXPECT_EQ(info_1.weight, 1);

  // Case 2: Match Segment Keyword. Note: Url has to match one of the
  // search providers in |search_providers.h|
  // Arrange
  std::string url_2 = "https://duckduckgo.com/?q=segment+keyword+1&foo=bar";

  // Act
  const PurchaseIntentSignalInfo info_2 =
      purchase_intent_classifier_->ExtractIntentSignal(url_2);

  // Assert
  const PurchaseIntentSegmentList gold_segments_2({"segment 1"});
  EXPECT_EQ(info_2.segments, gold_segments_2);
  EXPECT_EQ(info_2.weight, 1);

  // Case 3: Match with Funnel Keyword. Note: Url has to match one of the
  // search providers in |search_providers.h|. Matches segment due to
  // "segment keyword 2" and matches funnel stage due to "funnel keyword 2".
  // The signal weight should now be the one associated with the funnel keyword.
  // Arrange
  std::string url_3 = "https://duckduckgo.com/?q=segment+funnel+keyword+2";

  // Act
  const PurchaseIntentSignalInfo info_3 =
      purchase_intent_classifier_->ExtractIntentSignal(url_3);

  // Assert
  const PurchaseIntentSegmentList gold_segments_3({"segment 1", "segment 2"});
  EXPECT_EQ(info_3.segments, gold_segments_3);
  EXPECT_EQ(info_3.weight, 3);
}

}  // namespace ads
