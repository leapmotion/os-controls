// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include "FileMonitor.h"
#include "HandleUtilitiesWin.h"

#include <codecvt>
#include <locale>
#include <unordered_set>

#include ATOMIC_HEADER

class FileWatchWin :
  public FileWatch
{
  public:
    FileWatchWin(const std::string& path, const FileMonitor::t_callbackFunc& callback, DWORD dwNotifyFilter, HANDLE hDirectory);
    virtual ~FileWatchWin();

  private:
    const FileMonitor::t_callbackFunc m_callback;
    const DWORD m_dwNotifyFilter;
    const unique_ptr_of<HANDLE> m_hDirectory;
    const LPOVERLAPPED m_pOverlapped;
    
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

    // Filename (without full path)
    std::string m_filename;

    // Indicates whether or not the file being monitored in a directory
    bool m_isDirectory;

    // Becomes false when this file watcher has experienced an error
    bool m_ok;

    /// <summary>
    /// Queues up another ReadDirectoryChanges operation
    /// </summary>
    void ReadDirectoryChanges(void);

    /// <summary>
    /// Called when a ReadDirectory operation has completed
    /// </summary>
    void OnReadDirectoryComplete(void);

    /// <summary>
    /// Called when a directory operation has failed
    /// </summary>
    void OnReadDirectoryDeleted(void);

    union {
      // Hold at least three entries
      BYTE m_padding[3*(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * sizeof(WCHAR))];
      FILE_NOTIFY_INFORMATION m_notify;
    };

    friend class FileMonitorWin;
};

class FileMonitorWin :
  public FileMonitor
{
  public:
    FileMonitorWin();
    virtual ~FileMonitorWin();

    // FileMonitor overrides:
    std::shared_ptr<FileWatch> Watch(const std::string& path, const t_callbackFunc& callback, FileWatch::State states) override;
    int WatchCount() const override;

  protected:
    // CoreThread overrides:
    void OnStop() override;
    void Run() override;

    // Completion port, used to gather up all of our read operatoins
    const unique_ptr_of<HANDLE> m_hCompletion;

    // Allocated overlapped structures that we have to track in the event of an async termination
    std::unordered_set<std::unique_ptr<OVERLAPPED>> m_outstanding;

  private:
    std::atomic<int> m_numWatchers;
};
