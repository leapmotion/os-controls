#pragma once

#include <fstream>
#include <stdexcept>

// A simple container for loading and storing text file contents.
class TextFile {
public:

  TextFile () { }
  TextFile (const std::string &path) { Load(path); }

  bool IsLoaded () const { return !m_path.empty(); }
  const std::string &Path () const { return m_path; }
  const std::string &Contents () const { return m_contents; }
  std::string &Contents () { return m_contents; }

  void Clear () {
    m_path.clear();
    m_contents.clear();
  }
  void Load (const std::string &path) {
    if (path.empty()) {
      throw std::invalid_argument("must specify a nonempty path to load");
    }

    std::ifstream in(path);             // Attempt to open the file
    if (!in.is_open()) {                // Throw exception if that didn't work.
      throw std::domain_error("error encountered while attempting to open file \"" + path + "\"");
    }

    in.seekg(0, std::ios_base::end);    // Determine the size of the file.
    auto filesize = in.tellg();
    in.seekg(0, std::ios_base::beg);    // Reset to beginning of file.

    m_contents.resize(filesize);        // Make sure the buffer is the right size.
    in.read(&m_contents[0], filesize);  // Read the file contents.
    in.close();                         // Close the file

    m_path = path;                      // Set the path name, completing the operation.
  }

private:

  std::string m_path;
  std::string m_contents;
};
