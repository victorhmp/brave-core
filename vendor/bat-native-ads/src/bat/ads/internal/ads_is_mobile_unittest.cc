/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"

using ::testing::_;
using ::testing::SetArgPointee;

namespace ads {

class AdsIsMobileTest : public ::testing::Test {
 protected:
  std::unique_ptr<AdsClientMock> ads_client_mock_;
  std::unique_ptr<AdsImpl> ads_;

  AdsIsMobileTest() :
      ads_client_mock_(std::make_unique<AdsClientMock>()),
      ads_(std::make_unique<AdsImpl>(ads_client_mock_.get())) {
    // You can do set-up work for each test here
  }

  ~AdsIsMobileTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  // Objects declared here can be used by all tests in the test case
};

TEST_F(AdsIsMobileTest, IsMobile_iOS) {
  // Arrange
  ClientInfo client_info;
  client_info.platform = IOS;

  // Act
  EXPECT_CALL(*ads_client_mock_, GetClientInfo(_))
      .WillOnce(SetArgPointee<0>(client_info));

  // Assert
  EXPECT_TRUE(ads_->IsMobile());
}

TEST_F(AdsIsMobileTest, IsMobile_Android) {
  // Arrange
  ClientInfo client_info;
  client_info.platform = ANDROID_OS;

  // Act
  EXPECT_CALL(*ads_client_mock_, GetClientInfo(_))
      .WillOnce(SetArgPointee<0>(client_info));

  // Assert
  EXPECT_TRUE(ads_->IsMobile());
}

TEST_F(AdsIsMobileTest, IsDesktop_Windows) {
  // Arrange
  ClientInfo client_info;
  client_info.platform = WINDOWS;

  // Act
  EXPECT_CALL(*ads_client_mock_, GetClientInfo(_))
      .WillOnce(SetArgPointee<0>(client_info));

  // Assert
  EXPECT_FALSE(ads_->IsMobile());
}

TEST_F(AdsIsMobileTest, IsDesktop_macOS) {
  // Arrange
  ClientInfo client_info;
  client_info.platform = MACOS;

  // Act
  EXPECT_CALL(*ads_client_mock_, GetClientInfo(_))
      .WillOnce(SetArgPointee<0>(client_info));

  // Assert
  EXPECT_FALSE(ads_->IsMobile());
}

TEST_F(AdsIsMobileTest, IsDesktop_Linux) {
  // Arrange
  ClientInfo client_info;
  client_info.platform = LINUX;

  // Act
  EXPECT_CALL(*ads_client_mock_, GetClientInfo(_))
      .WillOnce(SetArgPointee<0>(client_info));

  // Assert
  EXPECT_FALSE(ads_->IsMobile());
}

}  // namespace ads
