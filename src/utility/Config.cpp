#include "stdafx.h"
#include "Config.h"

#include "autowiring/../contrib/json11/json11.hpp"
#include <fstream>
#include <sstream>

void Config::SetPrimaryFile(const std::string& filename)
{
  Load(filename);
  m_fileName = filename;
  WatchFile(filename);
}

void Config::Save(const std::string& filename) {
  std::unique_lock<std::mutex> lock(m_mutex);

  {
    std::ofstream outFile(filename);
    outFile << json11::Json(m_data).dump();;
  }
  if (!m_fileWatch) {
    WatchFile(filename);
  }
}

void Config::WatchFile(const std::string& filename) {
  m_fileWatch = m_fileMonitor->Watch(filename,
    [this](std::shared_ptr<FileWatch> fileWatch, FileWatch::State state) {
    this->Load(fileWatch->Path());
  });
}

bool Config::Load(const std::string& filename, bool overwrite) {
  std::unique_lock<std::mutex> lock(m_mutex);

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

  //Add the new data(only overwriting old values if overwrite is set) and fire the event
  for (auto& newEntry : newData) {
    auto oldEntry = m_data.find(newEntry.first);
    if (oldEntry == m_data.end()){
      m_data[newEntry.first] = newEntry.second;
      m_events(&ConfigEvent::ConfigChanged)(newEntry.first, newEntry.second);
    }
    else if (overwrite && newEntry.second != oldEntry->second) {
      oldEntry->second = newEntry.second;
      m_events(&ConfigEvent::ConfigChanged)(newEntry.first, newEntry.second);
    }
  }

  return true;
}

void Config::RebroadcastConfig(){
  std::unique_lock<std::mutex> lock(m_mutex);
  for (auto& entry : m_data) {
    m_events(&ConfigEvent::ConfigChanged)(entry.first, entry.second);
  }
}

void Config::Clear(){
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_data.clear();
  }
  Save(m_fileName);
}
