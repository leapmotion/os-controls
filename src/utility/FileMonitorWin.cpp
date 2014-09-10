// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FileMonitorWin.h"

#include <Shlwapi.h>

//
// FileWatchWin
//

FileWatchWin::FileWatchWin(const std::string& path, const FileMonitor::t_callbackFunc& callback, DWORD dwNotifyFilter, HANDLE hDirectory) :
  FileWatch(path),
  m_callback(callback),
  m_dwNotifyFilter(dwNotifyFilter),
  m_hDirectory(hDirectory),
  m_pOverlapped(new OVERLAPPED),
  m_ok(true)
{
  std::wstring widepath = converter.from_bytes(path);
  
  std::wstring widefilename(PathFindFileNameW(widepath.c_str()));
  m_filename = converter.to_bytes(widefilename);
  m_isDirectory = (bool)(PathIsDirectoryW(widepath.c_str()));

  memset(m_pOverlapped, 0, sizeof(*m_pOverlapped));
  m_pOverlapped->hEvent = CreateEvent(nullptr, true, false, nullptr);
}

FileWatchWin::~FileWatchWin(void) {
  CancelIoEx(m_hDirectory.get(), m_pOverlapped);
}

void FileWatchWin::ReadDirectoryChanges(void) {
  m_ok = m_ok && ReadDirectoryChangesW(
    m_hDirectory.get(),
    &m_notify,
    sizeof(m_padding),
    false,
    m_dwNotifyFilter,
    nullptr,
    m_pOverlapped,
    nullptr
  );
}

void FileWatchWin::OnReadDirectoryComplete(void) {
  PFILE_NOTIFY_INFORMATION notify = nullptr, nextNotify = &m_notify;
  bool isMoving = false;

  while (nextNotify) {
    notify = nextNotify;
    nextNotify = notify->NextEntryOffset > 0 ?
         reinterpret_cast<PFILE_NOTIFY_INFORMATION>(reinterpret_cast<PBYTE>(notify) + notify->NextEntryOffset) :
         nullptr;

    State state;
    if(!m_isDirectory && (!isMoving || notify->Action != FILE_ACTION_RENAMED_NEW_NAME)) {
      if(static_cast<DWORD>(m_filename.size()*sizeof(WCHAR)) != notify->FileNameLength)
        continue;
      if(::memcmp(m_filename.c_str(), notify->FileName, notify->FileNameLength) != 0)
        continue;
    }
    switch(notify->Action) {
    case FILE_ACTION_ADDED:
      state = State::MODIFIED;
      break;
    case FILE_ACTION_REMOVED:
      state = m_isDirectory ? State::MODIFIED : State::DELETED;
      break;
    case FILE_ACTION_MODIFIED:
      state = State::MODIFIED;
      break;
    case FILE_ACTION_RENAMED_OLD_NAME:
      state = m_isDirectory ? State::MODIFIED : State::RENAMED;
      isMoving = true;
      break;
    case FILE_ACTION_RENAMED_NEW_NAME:
      if(isMoving) {
        m_filename = converter.to_bytes(std::wstring(notify->FileName, notify->FileNameLength/sizeof(WCHAR)));
        isMoving = false;
        continue;
      } else {
        state = m_isDirectory ? State::MODIFIED : State::RENAMED;
      }
      break;
    default:
      // No idea what to do here
      continue;
    }

    try {
      m_callback(shared_from_this(), state);
    } catch (...) {}
  }
}

void FileWatchWin::OnReadDirectoryDeleted(void) {
  m_callback(shared_from_this(), State::DELETED);
}

//
// FileMonitorWin
//

FileMonitorWin::FileMonitorWin(void):
  m_hCompletion(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, (ULONG_PTR)this, 0))
{
}

FileMonitorWin::~FileMonitorWin()
{
}

FileMonitor* FileMonitor::New() {
  return new FileMonitorWin();
}

std::shared_ptr<FileWatch> FileMonitorWin::Watch(const std::string& path, const t_callbackFunc& callback, FileWatch::State states) {
  std::wstring widepath(converter.from_bytes(path));
  try {
    if (!(bool)PathFileExistsW(widepath.c_str()))
      return nullptr;
  } catch (...) {
    return nullptr;
  }

  WCHAR parent[MAX_PATH * 2];
  memcpy(parent, widepath.c_str(), widepath.size());
  bool gotParent = (bool)PathRemoveFileSpecW(parent);

  auto directory = (PathIsDirectoryW(widepath.c_str()) || !gotParent) ? widepath : parent;

  // Open the handle to the directory we were asked to watch with the
  // correct permissions so that we can actually conduct asynchronous
  // read operations.
  HANDLE hFile = CreateFileW(
    directory.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    nullptr,
    OPEN_EXISTING,
    FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS,
    nullptr
  );
  if(hFile == INVALID_HANDLE_VALUE)
    // No such file, what else can we do?
    return nullptr;

  // Compose the notification filter based on the user's request
  DWORD dwNotifyFilter = 0;
  if(states & FileWatch::State::RENAMED || states & FileWatch::State::DELETED)
    dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME;
  if(states & FileWatch::State::MODIFIED)
    dwNotifyFilter |= FILE_NOTIFY_CHANGE_LAST_WRITE;

  // Need an overlapped structure to track this operation.  We'll also be using this to decide
  // how to notify the true caller.
  auto watcher = std::shared_ptr<FileWatchWin>(new FileWatchWin(path, callback, dwNotifyFilter, hFile),
                                               [this] (FileWatchWin* fileWatch) {
                                                 --m_numWatchers;
                                                 delete fileWatch;
                                               });
  m_outstanding.emplace(watcher->m_pOverlapped);

  // Need a second-order shared pointer so we can preserve references
  std::weak_ptr<FileWatchWin>* pWatcherWeak = new std::weak_ptr<FileWatchWin>(watcher);

  // Attach to the completion port with the FileWatchWin we just constructed
  if(!CreateIoCompletionPort(hFile, m_hCompletion.get(), (ULONG_PTR) pWatcherWeak, 0)) {
    // Something went wrong, can't attach a watcher at this point
    delete pWatcherWeak;
    return nullptr;
  }

  // Initial pend, and then return to the controller
  watcher->ReadDirectoryChanges();
  ++m_numWatchers;
  return watcher;
}

int FileMonitorWin::WatchCount() const {
  return m_numWatchers;
}

void FileMonitorWin::OnStop() {
  // Time to go away
  PostQueuedCompletionStatus(m_hCompletion.get(), 0, 0, nullptr);
}

void FileMonitorWin::Run()
{
  // Alertable wait ad infinitum
  while(!ShouldStop()) {
    // Wait for something to happen
    DWORD dwBytesTransferred;
    std::weak_ptr<FileWatchWin>* pWatcherWeak;
    LPOVERLAPPED lpOverlapped;
    BOOL status;
    
    status = GetQueuedCompletionStatus(
      m_hCompletion.get(),
      &dwBytesTransferred,
      (PULONG_PTR) &pWatcherWeak,
      &lpOverlapped,
      INFINITE
    );

    // If the overlapped structure doesn't exist, verify that we haven't been
    // told to stop
    if(!lpOverlapped)
      continue;

    // Dereference shared pointer, decide what to do with this entity:
    auto sp = pWatcherWeak->lock();
    
    // Failure occurred?
    if(!status) {
      switch(GetLastError()) {
      case ERROR_ACCESS_DENIED:
        // File is now gone, report to the user.
        sp->OnReadDirectoryDeleted();
        break;
      }

      // Something went wrong, invariably we're going to release this pointer and
      // anything that ties it down.
      sp.reset();
    }

    if(!sp) {
      delete pWatcherWeak;

      // Already 404, can't do anything, destroy overlapped structure
      std::unique_ptr<OVERLAPPED> ovl(lpOverlapped);
      m_outstanding.erase(ovl);
      ovl.release();
      continue;
    }

    // Process the notification if any bytes were transferred
    if(dwBytesTransferred)
      sp->OnReadDirectoryComplete();

    // Schedule another operation
    sp->ReadDirectoryChanges();
  }
}
