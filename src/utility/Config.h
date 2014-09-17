#pragma once

#include<string>
#include<map>
#include<stdexcept>
#include<chrono>

#include "ConfigEvent.h"
#include "FileMonitor.h"
#include <autowiring/Autowired.h>
#include <autowiring/../contrib/json11/json11.hpp>

#define CONFIG_DEFAULT_NAME "config.json"

/// <summary>
/// Stores application-wide settings on a per-user basis.
/// Presently only allows numbers, bools, strings, and vectors or maps of json11::Json objects.
/// All strings are UTF-8 encoded.
/// </summary>
/// TODO: Consider making this a CoreThread so we have a dispatch queue & can queue save/loads.
/// TODO: Make typed ConfigVars that can be compile-time checked.
class Config {
public:
  /// <summary>
  /// Creates a config mapped to a given filename ("config.json" by default).
  /// The config will be saved to this file on modification, and the file will be
  /// watched for modifications.
  /// </summary>
  Config(const std::string& filename = CONFIG_DEFAULT_NAME) { SetPrimaryFile(filename); }

  /// <summary>
  /// Sets the file to watch and save changes to
  /// </summary>
  void SetPrimaryFile(const std::string& filename);
  const std::string& GetPrimaryFile() const { return m_fileName; }
  
  const std::string& GetDefaultFilename() const { 
    static const std::string name = CONFIG_DEFAULT_NAME;
    return name;
  }

  /// <summary>
  /// Performs a one-off save to the specified file
  /// </summary>
  void Save(const std::string& filename);

  /// <summary>
  /// Loads a file in a one-off manner.  If overwrite is set, it will
  /// Overwrite any settings it finds with the ones from the file, otherwise
  /// it will only load settings for which there is not yet a value
  /// </summary>
  bool Load(const std::string& filename, bool overwrite = true);

  /// <summary>
  /// Send out configuration change events, even if nothing has changed.
  /// </summary>
  void RebroadcastConfig();

  /// <summary>
  /// Clears all config data
  /// </summary>
  void Clear();

  template<typename T>
  T Get(const std::string& prop) const { }

  template<typename T>
  void Set(const std::string& prop, const T &val){
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      auto entry = m_data.find(prop);
      if (entry != m_data.end() && entry->second == val)
        return;

      if (entry == m_data.end())
        entry = m_data.emplace(prop, json11::Json(val)).first;
      else
        m_data[prop] = json11::Json(val);

      m_events(&ConfigEvent::ConfigChanged)(entry->first,entry->second);
    }

    Save(m_fileName);
  }

  /// <summary>
  /// Completely removes a property from the config list
  /// </summary>
  void Unset(const std::string& prop) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data.erase(prop);
  }

  bool Exists(const std::string& prop) const {
    return std::unique_lock<std::mutex>(m_mutex), m_data.count(prop) > 0;
  }

  json11::Json::object::const_iterator GetRef(const std::string& prop) const {
    return std::unique_lock<std::mutex>(m_mutex), m_data.find(prop);
  }

private:
  AutoRequired<FileMonitor> m_fileMonitor;
  AutoFired<ConfigEvent> m_events;

  std::shared_ptr<FileWatch> m_fileWatch;
  std::string m_fileName;

  json11::Json::object m_data;
  mutable std::mutex m_mutex;
  
  void WatchFile(const std::string& filename);

  json11::Json::object::const_iterator GetInternal(const std::string& prop) const {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto ref = m_data.find(prop);
    if (ref == m_data.end())
      throw std::runtime_error("Could not find property:" + prop);
    return ref;
  }

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
