// Copyright Sylar129

#pragma once

/**
 * @file non_copyable.h
 * @brief Macro definitions for managing special member functions
 *
 * Provides convenient macros to delete copy/move constructors and
 * assignment operators for non-copyable and non-movable classes.
 */

/**
 * @def DISABLE_COPY_AND_MOVE
 * @brief Deletes all copy and move special member functions
 * @param ClassName The name of the class
 *
 * Usage:
 * @code
 * class MyClass {
 * public:
 *     MyClass() = default;
 * private:
 *     DISABLE_COPY_AND_MOVE(MyClass);
 * };
 * @endcode
 */
#define DISABLE_COPY_AND_MOVE(ClassName)           \
  ClassName(const ClassName&) = delete;            \
  ClassName& operator=(const ClassName&) = delete; \
  ClassName(ClassName&&) = delete;                 \
  ClassName& operator=(ClassName&&) = delete

/**
 * @def DISABLE_COPY
 * @brief Deletes only copy constructor and copy assignment operator
 * @param ClassName The name of the class
 */
#define DISABLE_COPY(ClassName)         \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(const ClassName&) = delete

/**
 * @def DISABLE_MOVE
 * @brief Deletes only move constructor and move assignment operator
 * @param ClassName The name of the class
 */
#define DISABLE_MOVE(ClassName)    \
  ClassName(ClassName&&) = delete; \
  ClassName& operator=(ClassName&&) = delete
