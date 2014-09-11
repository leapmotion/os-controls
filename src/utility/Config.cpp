#include "stdafx.h"
#include "Config.h"

#include "autowiring/../contrib/json11/json11.hpp"
#include <fstream>
#include <sstream>

Config::Config(void) {
}

void Config::Save(const std::string& filename) const {
  std::ofstream outFile(filename);
  outFile << json11::Json(m_data).dump();;
}

bool Config::Load(const std::string& filename) {
  std::string err;
  std::string data;

  {
    std::ifstream inFile(filename);
    if (inFile.bad())
      return false;

    std::stringstream dataStream;
    dataStream << inFile.rdbuf();
    data = dataStream.str();
  }
  
  if (data.empty())
    return false;

  auto newData = json11::Json::parse(data, err).object_items();
  if (!err.empty())
    throw std::runtime_error(std::string("Json parsing error:") + err);

  //insert does not overwrite items in the set with the input
  //We want the new data to take precidence, so insert old into the new set.
  newData.insert(m_data.begin(), m_data.end());
  std::swap(newData, m_data);

  return true;
}

std::chrono::microseconds Config::GetFrameRate() const {
  return std::chrono::microseconds(static_cast<long>(Get<double>("framerate")));
}

void Config::Clear(){
  m_data.clear();
}