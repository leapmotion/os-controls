#pragma once

#include<string>
#include<map>
#include<stdexcept>

#include <autowiring/../contrib/json11/json11.hpp>

/// <summary>
/// Stores application-wide settings on a per-user basis
/// </summary>
class Config {
public:
  Config(void);

  void Save(const std::string& filename = "config.json") const;
  void Load(const std::string& filename = "config.json");

  template<typename T>
  T Get(const std::string& prop) const { static_assert("Unspecialized on type"); }

  bool Exists(const std::string& prop) const {
    return m_data.object_items().count(prop) > 0;
  }

  json11::Json::object::const_iterator GetRef(const std::string& prop) const {
    return m_data.object_items().find(prop);
  }

  std::chrono::microseconds GetFrameRate(void) const;

private:
  json11::Json m_data;
};

//Specializations for valid storage types

template<>
double Config::Get<double>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_number())
    throw std::runtime_error(prop + "Is not a number");

  return val.number_value();
}

template<>
float Config::Get<float>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_number())
    throw std::runtime_error(prop + "Is not a number");

  return static_cast<float>(val.number_value());
}

template<>
bool Config::Get<bool>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_bool())
    throw std::runtime_error(prop + "Is not a bool");

  return val.bool_value();
}

template<>
const std::string& Config::Get<const std::string&>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_string())
    throw std::runtime_error(prop + "Is not a string");

  return val.string_value();
}

template<>
const json11::Json::array& Config::Get<const json11::Json::array&>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_array())
    throw std::runtime_error(prop + "Is not an array");

  return val.array_items();
}

template<>
const json11::Json::object& Config::Get<const json11::Json::object&>(const std::string& prop) const {
  auto& val = m_data[prop];
  if (!val.is_object())
    throw std::runtime_error(prop + "Is not an object");

  return val.object_items();
}