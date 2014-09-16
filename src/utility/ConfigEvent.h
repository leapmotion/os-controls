#pragma once

#include <string>
#include <../autowiring/contrib/json11/json11.hpp>

struct ConfigEvent {
  /// <summary>
  /// Event fired when a config value is added or changed - fired per config value.
  virtual void ConfigChanged(const std::string& config, const json11::Json& value) = 0;
};
