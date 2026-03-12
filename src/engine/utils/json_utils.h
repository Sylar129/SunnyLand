// Copyright Sylar129

#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

namespace engine::utils::json {

template <typename T, typename Enable = void>
struct Loader {
  static bool IsCompatible(const nlohmann::json& json_value) {
    return json_value.is_object();
  }

  static void Load(const nlohmann::json& json_value, T& target) {
    using ::nlohmann::from_json;
    from_json(json_value, target);
  }
};

template <>
struct Loader<bool> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    return json_value.is_boolean();
  }

  static void Load(const nlohmann::json& json_value, bool& target) {
    target = json_value.get<bool>();
  }
};

template <typename T>
struct Loader<T, std::enable_if_t<std::is_integral_v<T> &&
                                  !std::is_same_v<T, bool>>> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    return json_value.is_number_integer();
  }

  static void Load(const nlohmann::json& json_value, T& target) {
    target = json_value.get<T>();
  }
};

template <typename T>
struct Loader<T, std::enable_if_t<std::is_floating_point_v<T>>> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    return json_value.is_number();
  }

  static void Load(const nlohmann::json& json_value, T& target) {
    target = json_value.get<T>();
  }
};

template <>
struct Loader<std::string> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    return json_value.is_string();
  }

  static void Load(const nlohmann::json& json_value, std::string& target) {
    target = json_value.get<std::string>();
  }
};

template <typename T>
struct Loader<std::vector<T>> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    if (!json_value.is_array()) {
      return false;
    }

    for (const auto& item : json_value) {
      if (!Loader<T>::IsCompatible(item)) {
        return false;
      }
    }
    return true;
  }

  static void Load(const nlohmann::json& json_value, std::vector<T>& target) {
    std::vector<T> parsed_values;
    parsed_values.reserve(json_value.size());

    for (const auto& item : json_value) {
      T parsed_value{};
      Loader<T>::Load(item, parsed_value);
      parsed_values.push_back(std::move(parsed_value));
    }

    target = std::move(parsed_values);
  }
};

template <typename T>
struct Loader<std::unordered_map<std::string, T>> {
  static bool IsCompatible(const nlohmann::json& json_value) {
    if (!json_value.is_object()) {
      return false;
    }

    for (const auto& [key, item] : json_value.items()) {
      static_cast<void>(key);
      if (!Loader<T>::IsCompatible(item)) {
        return false;
      }
    }
    return true;
  }

  static void Load(const nlohmann::json& json_value,
                   std::unordered_map<std::string, T>& target) {
    std::unordered_map<std::string, T> parsed_values;

    for (const auto& [key, item] : json_value.items()) {
      T parsed_value{};
      Loader<T>::Load(item, parsed_value);
      parsed_values.emplace(key, std::move(parsed_value));
    }

    target = std::move(parsed_values);
  }
};

template <typename T>
void LoadField(const nlohmann::json& json_obj, std::string_view key,
               T& target) {
  auto it = json_obj.find(key);
  if (it != json_obj.end() && Loader<T>::IsCompatible(*it)) {
    Loader<T>::Load(*it, target);
  }
}

}  // namespace engine::utils::json

#define ENGINE_JSON_SAFE_FROM(v1) \
  ::engine::utils::json::LoadField(nlohmann_json_j, #v1, nlohmann_json_t.v1);

#define ENGINE_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, ...)               \
  template <typename BasicJsonType,                                            \
            nlohmann::detail::enable_if_t<                                     \
                nlohmann::detail::is_basic_json<BasicJsonType>::value, int> =  \
                0>                                                             \
  void to_json(BasicJsonType& nlohmann_json_j,                                 \
               const Type& nlohmann_json_t) {                                  \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))   \
  }                                                                            \
  template <typename BasicJsonType,                                            \
            nlohmann::detail::enable_if_t<                                     \
                nlohmann::detail::is_basic_json<BasicJsonType>::value, int> =  \
                0>                                                             \
  void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t) {\
    if (!nlohmann_json_j.is_object()) {                                        \
      return;                                                                  \
    }                                                                          \
    NLOHMANN_JSON_EXPAND(                                                      \
        NLOHMANN_JSON_PASTE(ENGINE_JSON_SAFE_FROM, __VA_ARGS__))               \
  }