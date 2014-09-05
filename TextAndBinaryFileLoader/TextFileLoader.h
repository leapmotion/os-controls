#pragma once

#include "ResourceManager.h"
#include "SDLController.h"
#include "TextFile.h"

// Template specialization of ResourceLoader<TextFile> which defines how to load such a resource.
template <>
struct ResourceLoader<TextFile> {
  static const bool exists = true;
  static std::shared_ptr<TextFile> LoadResource (const std::string &name, ResourceManager<TextFile> &calling_manager) {
    // TODO: do real filesystem path lookup, or have some sort of configuration singleton that has this path
    try {
      return std::make_shared<TextFile>(name);
    } catch (const std::exception &e) {
      // change the generic exception into a type-specific one.
      throw ResourceExceptionOfType<TextFile>(e.what());
    }
  }
};
