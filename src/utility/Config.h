#pragma once

#include<string>
#include<map>
#include<stdexcept>
#include<chrono>

#include <autowiring/../contrib/json11/json11.hpp>

/// <summary>
/// Stores application-wide settings on a per-user basis.
/// Presently only allows numbers, bools, strings, and vectors or maps of json11::Json objects.
/// </summary>
class Config {
public:
  Config(void);

  void Save(const std::string& filename = "config.json") const;
  void Load(const std::string& filename = "config.json");

  template<typename T>
  T Get(const std::string& prop) const { static_assert(false,"Unspecialized on type"); return T(); }

  template<typename T>
  void Set(const std::string& prop, const T &val){
    m_data[prop] = json11::Json(val);
  }

  bool Exists(const std::string& prop) const {
    return m_data.count(prop) > 0;
  }

  json11::Json::object::const_iterator GetRef(const std::string& prop) const {
    return m_data.find(prop);
  }

  std::chrono::microseconds GetFrameRate(void) const;

private:

  json11::Json::object::const_iterator GetInternal(const std::string& prop) const {
    auto ref = m_data.find(prop);
    if (ref == m_data.end())
      throw std::runtime_error("Could not find property:" + prop);
    return ref;
  }

  json11::Json::object m_data;
};

//Specializations for valid storage types

template<>
inline double Config::Get<double>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_number())
    throw std::runtime_error(prop + "Is not a number");

  return val->second.number_value();
}

template<>
inline float Config::Get<float>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_number())
    throw std::runtime_error(prop + "Is not a number");

  return static_cast<float>(val->second.number_value());
}

template<>
inline int Config::Get<int>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_number())
    throw std::runtime_error(prop + "Is not a number");

  return static_cast<int>(val->second.number_value());
}

template<>
inline bool Config::Get<bool>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_bool())
    throw std::runtime_error(prop + "Is not a bool");

  return val->second.bool_value();
}

template<>
inline const std::string& Config::Get<const std::string&>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_string())
    throw std::runtime_error(prop + "Is not a string");

  return val->second.string_value();
}

template<>
inline const json11::Json::array& Config::Get<const json11::Json::array&>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_array())
    throw std::runtime_error(prop + "Is not an array");

  return val->second.array_items();
}

template<>
inline const json11::Json::object& Config::Get<const json11::Json::object&>(const std::string& prop) const {
  auto val = GetInternal(prop);
  if (!val->second.is_object())
    throw std::runtime_error(prop + "Is not an object");

  return val->second.object_items();
}