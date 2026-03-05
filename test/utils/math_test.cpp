// Copyright Sylar129
// Math utilities unit tests

#include <gtest/gtest.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/utils/math.h"

namespace {
  const float EPSILON = 1e-6f;
}

class MathTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup for each test
  }

  void TearDown() override {
    // Cleanup after each test
  }
};

// Test 1: vec2 default construction
TEST_F(MathTest, Vec2DefaultConstruction) {
  glm::vec2 v;
  EXPECT_FLOAT_EQ(v.x, 0.0f);
  EXPECT_FLOAT_EQ(v.y, 0.0f);
}

// Test 2: vec2 parameterized construction
TEST_F(MathTest, Vec2ParameterizedConstruction) {
  glm::vec2 v(3.0f, 4.0f);
  EXPECT_FLOAT_EQ(v.x, 3.0f);
  EXPECT_FLOAT_EQ(v.y, 4.0f);
}

// Test 3: vec2 addition
TEST_F(MathTest, Vec2Addition) {
  glm::vec2 v1(1.0f, 2.0f);
  glm::vec2 v2(3.0f, 4.0f);
  glm::vec2 result = v1 + v2;
  
  EXPECT_FLOAT_EQ(result.x, 4.0f);
  EXPECT_FLOAT_EQ(result.y, 6.0f);
}

// Test 4: vec2 subtraction
TEST_F(MathTest, Vec2Subtraction) {
  glm::vec2 v1(5.0f, 7.0f);
  glm::vec2 v2(2.0f, 3.0f);
  glm::vec2 result = v1 - v2;
  
  EXPECT_FLOAT_EQ(result.x, 3.0f);
  EXPECT_FLOAT_EQ(result.y, 4.0f);
}

// Test 5: vec2 scalar multiplication
TEST_F(MathTest, Vec2ScalarMultiplication) {
  glm::vec2 v(2.0f, 3.0f);
  glm::vec2 result = v * 2.0f;
  
  EXPECT_FLOAT_EQ(result.x, 4.0f);
  EXPECT_FLOAT_EQ(result.y, 6.0f);
}

// Test 6: vec2 magnitude (length)
TEST_F(MathTest, Vec2Magnitude) {
  glm::vec2 v(3.0f, 4.0f);
  float magnitude = glm::length(v);
  
  // 3-4-5 triangle: magnitude should be 5
  EXPECT_FLOAT_EQ(magnitude, 5.0f);
}

// Test 7: vec2 normalization
TEST_F(MathTest, Vec2Normalization) {
  glm::vec2 v(3.0f, 4.0f);
  glm::vec2 normalized = glm::normalize(v);
  
  float magnitude = glm::length(normalized);
  EXPECT_NEAR(magnitude, 1.0f, EPSILON);
}

// Test 8: vec2 dot product
TEST_F(MathTest, Vec2DotProduct) {
  glm::vec2 v1(1.0f, 0.0f);
  glm::vec2 v2(0.0f, 1.0f);
  
  float dot = glm::dot(v1, v2);
  EXPECT_FLOAT_EQ(dot, 0.0f);  // Perpendicular vectors
}

// Test 9: vec2 distance calculation
TEST_F(MathTest, Vec2Distance) {
  glm::vec2 v1(0.0f, 0.0f);
  glm::vec2 v2(3.0f, 4.0f);
  
  float distance = glm::distance(v1, v2);
  EXPECT_FLOAT_EQ(distance, 5.0f);
}

// Test 10: vec2 zero vector
TEST_F(MathTest, Vec2ZeroVector) {
  glm::vec2 v(0.0f, 0.0f);
  float magnitude = glm::length(v);
  
  EXPECT_FLOAT_EQ(magnitude, 0.0f);
}

// Test 11: vec2 negative values
TEST_F(MathTest, Vec2NegativeValues) {
  glm::vec2 v(-3.0f, -4.0f);
  float magnitude = glm::length(v);
  
  EXPECT_FLOAT_EQ(magnitude, 5.0f);
}

// Test 12: vec2 large values
TEST_F(MathTest, Vec2LargeValues) {
  glm::vec2 v(1000.0f, 1000.0f);
  glm::vec2 normalized = glm::normalize(v);
  
  float magnitude = glm::length(normalized);
  EXPECT_NEAR(magnitude, 1.0f, EPSILON);
}

// Test 13: vec2 small values
TEST_F(MathTest, Vec2SmallValues) {
  glm::vec2 v(0.001f, 0.001f);
  glm::vec2 normalized = glm::normalize(v);
  
  float magnitude = glm::length(normalized);
  EXPECT_NEAR(magnitude, 1.0f, EPSILON);
}

// Test 14: Radian to degree conversion
TEST_F(MathTest, RadianToDegreeConversion) {
  float radians = 3.14159265f;  // Approximately PI
  float degrees = glm::degrees(radians);
  
  EXPECT_NEAR(degrees, 180.0f, 0.1f);
}

// Test 15: Degree to radian conversion
TEST_F(MathTest, DegreeToRadianConversion) {
  float degrees = 180.0f;
  float radians = glm::radians(degrees);
  
  EXPECT_NEAR(radians, 3.14159265f, 0.1f);
}
