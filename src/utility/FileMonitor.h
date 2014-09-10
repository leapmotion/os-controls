// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include <autowiring/CoreThread.h>
#include <codecvt>
#include <locale>
#include <string>
#include MEMORY_HEADER

class FileWatch:
  public std::enable_shared_from_this<FileWatch>
{
  public:
    enum class State : uint32_t {
      NONE     =  0U,

      // Receive notifications of file name changes (not file creation)
      RENAMED  = (1U << 0),
      
      // Receive notifications when deletions take place
      DELETED  = (1U << 1),

      // Receive notifications any time an adjustment is made to the time of last write
      MODIFIED = (1U << 2),

      ALL      = (RENAMED | DELETED | MODIFIED)
    };

    FileWatch(const std::string& path) : m_path(path) {}
    FileWatch(const std::wstring& wpath) : m_path(converter.to_bytes(wpath)) {}

    virtual ~FileWatch() {}

    /// <summary>
    /// Retrieve file path being watched.
    /// </summary>
    const std::string& Path() const { return m_path; }

  protected:
    std::string m_path;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
};

inline FileWatch::State operator|(FileWatch::State a, FileWatch::State b) {
  return static_cast<FileWatch::State>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool operator&(FileWatch::State a, FileWatch::State b) {
  return !!(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

class FileMonitor :
  public CoreThread
{
  public:
    FileMonitor() {}
    virtual ~FileMonitor() {}

    typedef std::function<void(std::shared_ptr<FileWatch>, FileWatch::State)> t_callbackFunc;

    /// <summary>
    /// Begin watching for activity on a particular file.
    /// Path is interpreted as UTF-8.
    /// </summary>
    virtual std::shared_ptr<FileWatch> Watch(const std::string& path,
                                             const t_callbackFunc& callback,
                                             FileWatch::State states = FileWatch::State::ALL) = 0;

    /// <summary>
    /// As above, but calls the converstion function and takes a wide (UTF-16) path
    /// </summary>
    virtual std::shared_ptr<FileWatch> Watch(const std::wstring& path,
                                     const t_callbackFunc& callback,
                                     FileWatch::State states = FileWatch::State::ALL) 
    {
      return Watch(converter.to_bytes(path), callback, states);
    }

    /// <summary>
    /// Return the number of files being watched.
    /// </summary>
    virtual int WatchCount() const = 0;

    /// <summary>
    /// Creates a new FileMonitor instance
    /// </summary>
    static FileMonitor* New();
protected:
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
};
