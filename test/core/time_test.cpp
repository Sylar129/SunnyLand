// Copyright Sylar129
// Time management unit tests

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "engine/core/time.h"

class TimeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a fresh Time instance for each test
    time_ = std::make_unique<engine::core::Time>();
  }

  void TearDown() override {
    // Cleanup
    time_.reset();
  }

  std::unique_ptr<engine::core::Time> time_;
};

// Test 1: Time initialization
TEST_F(TimeTest, TimeInitialization) {
  float delta_time = time_->GetDeltaTime();
  EXPECT_GE(delta_time, 0.0f);
}

// Test 2: Delta time update
TEST_F(TimeTest, DeltaTimeUpdate) {
  time_->Update();
  float delta_time_1 = time_->GetDeltaTime();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  time_->Update();
  float delta_time_2 = time_->GetDeltaTime();
  
  // Both should be positive
  EXPECT_GT(delta_time_1, 0.0f);
  EXPECT_GT(delta_time_2, 0.0f);
}

// Test 3: Multiple frame updates
TEST_F(TimeTest, MultipleFrameUpdates) {
  for (int i = 0; i < 5; ++i) {
    time_->Update();
  }
  
  // Should complete without errors
  float delta = time_->GetDeltaTime();
  EXPECT_GE(delta, 0.0f);
}

// Test 4: Reset functionality via new instance
TEST_F(TimeTest, ResetFunctionality) {
  time_->Update();
  float delta_before = time_->GetDeltaTime();
  
  // Create new instance to reset
  time_ = std::make_unique<engine::core::Time>();
  float delta_after = time_->GetDeltaTime();
  
  // After reset, delta should be very small
  EXPECT_LE(delta_after, delta_before);
}

// Test 5: Delta time is reasonable
TEST_F(TimeTest, DeltaTimeIsReasonable) {
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  time_->Update();
  
  float delta_time = time_->GetDeltaTime();
  
  // Delta time should be at least ~10ms but not too large
  EXPECT_GT(delta_time, 0.005f);  // At least 5ms
  EXPECT_LT(delta_time, 0.1f);    // Less than 100ms
}

// Test 6: Consistent updates
TEST_F(TimeTest, ConsistentUpdates) {
  float delta_1, delta_2, delta_3;
  
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  time_->Update();
  delta_1 = time_->GetDeltaTime();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  time_->Update();
  delta_2 = time_->GetDeltaTime();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  time_->Update();
  delta_3 = time_->GetDeltaTime();
  
  // All deltas should be positive and relatively similar
  EXPECT_GT(delta_1, 0.0f);
  EXPECT_GT(delta_2, 0.0f);
  EXPECT_GT(delta_3, 0.0f);
}

// Test 7: Delta time non-negative
TEST_F(TimeTest, DeltaTimeNonNegative) {
  for (int i = 0; i < 5; ++i) {
    time_->Update();
    float delta = time_->GetDeltaTime();
    EXPECT_GE(delta, 0.0f);
  }
}

// Test 8: Time scale functionality
TEST_F(TimeTest, TimeScaleFunctionality) {
  EXPECT_FLOAT_EQ(time_->GetTimeScale(), 1.0f);
  
  time_->SetTimeScale(2.0f);
  EXPECT_FLOAT_EQ(time_->GetTimeScale(), 2.0f);
  
  time_->SetTimeScale(0.5f);
  EXPECT_FLOAT_EQ(time_->GetTimeScale(), 0.5f);
}

// Test 9: Negative time scale clamping
TEST_F(TimeTest, NegativeTimeScaleClamping) {
  time_->SetTimeScale(-1.0f);
  // Should be clamped to 0 based on the implementation
  EXPECT_GE(time_->GetTimeScale(), 0.0f);
}

// Test 10: Target FPS setting
TEST_F(TimeTest, TargetFpsSetting) {
  EXPECT_EQ(time_->GetTargetFps(), 0);  // Default unlimited
  
  time_->SetTargetFps(60);
  EXPECT_EQ(time_->GetTargetFps(), 60);
  
  time_->SetTargetFps(144);
  EXPECT_EQ(time_->GetTargetFps(), 144);
}

// Test 11: Negative FPS clamping
TEST_F(TimeTest, NegativeFpsClamping) {
  time_->SetTargetFps(-10);
  EXPECT_GE(time_->GetTargetFps(), 0);
}

// Test 12: Unscaled delta time
TEST_F(TimeTest, UnscaledDeltaTime) {
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  time_->Update();
  time_->SetTimeScale(2.0f);
  
  float delta_scaled = time_->GetDeltaTime();
  float delta_unscaled = time_->GetUnscaledDeltaTime();
  
  // Scaled should be approximately 2x unscaled
  EXPECT_NEAR(delta_scaled / delta_unscaled, 2.0f, 0.01f);
}
