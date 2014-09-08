#include "stdafx.h"
#include "Config.h"

#include "autowiring/../contrib/json11/json11.hpp"
#include <fstream>

Config::Config(void) {

}

void Config::Save(const std::string& filename) const {
  auto data = m_data.dump();

  std::ofstream outFile(filename);
  outFile << data;
}

void Config::Load(const std::string& filename) {
  std::ifstream inFile(filename);
  std::string err;
  std::string data;

  inFile >> data;
  m_data = json11::Json::parse(data, err);
  if (!err.empty())
    throw std::runtime_error(std::string("Json parsing error:") + err);
}