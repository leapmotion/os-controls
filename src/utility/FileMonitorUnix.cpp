// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FileMonitorUnix.h"
#include ATOMIC_HEADER

#include <boost/filesystem/operations.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/inotify.h>

FileWatchUnix::FileWatchUnix(const boost::filesystem::path& path) : FileWatch(path), m_key(-1)
{
}

FileWatchUnix::~FileWatchUnix()
{
}

//
// FileMonitorUnix
//

FileMonitorUnix::FileMonitorUnix() : m_inotify(::inotify_init()), m_pipes{-1, -1}, m_moveCookie(0)
{
  if (pipe(m_pipes) == -1) {
    // Got an error, not much we can do about it
  }
}

FileMonitorUnix::~FileMonitorUnix()
{
  ::close(m_inotify);
  ::close(m_pipes[1]);
  ::close(m_pipes[0]);
  m_inotify = -1;
}

FileMonitor* FileMonitor::New() {
  return new FileMonitorUnix();
}

void FileMonitorUnix::OnStop()
{
  // Closing the inotify file descriptor doesn't wake the poll, using a
  // secondary file descriptor to do the job.
  ::close(m_pipes[1]);
  ::close(m_inotify);
  m_inotify = -1;
}

void FileMonitorUnix::Run()
{
  char buffer[sizeof(inotify_event) + NAME_MAX + 1]; // Enough room for one event
  int offset = 0;

  while (!ShouldStop()) {
    struct pollfd fds[2] = { { m_inotify, POLLIN | POLLERR, 0 },
                             { m_pipes[0], POLLIN | POLLERR, 0 } };
    if (fds[0].fd < 0 || poll(fds, sizeof(fds)/sizeof(fds[0]), -1) <= 0 || fds[0].revents != POLLIN) {
      break;
    }
    // Read data and append it to any partial event that may already be in the buffer
    int length = ::read(fds[0].fd, buffer + offset, sizeof(buffer) - offset);
    if (length < 0) {
      break;
    }
    // Adjust length and reset offset in an attempt to process complete events
    length += offset;
    offset = 0;

    // Process the events
    while ((length - offset) >= static_cast<int>(sizeof(inotify_event))) {
      struct inotify_event& event = *reinterpret_cast<inotify_event*>(&buffer[offset]);

      if ((length - offset) < (static_cast<int>(sizeof(inotify_event) + event.len))) {
        break; // We have a partial event, deal with it when we have all of the data
      }
      std::unique_lock<std::mutex> lock(m_mutex);
      auto found = m_watchers.find(event.wd);
      if (found != m_watchers.end()) {
        auto entries = found->second; // Make copy of the entries, as we will give up the lock
        for (auto& entry : entries) {
          if (entry.unique()) { // Entry is in the process of going away, so don't call callback
            continue;
          }
          auto fileWatch = entry->fileWatch.lock();
          if (fileWatch) {
            auto callback = entry->callback;
            FileWatch::State states = FileWatch::State::NONE;
            if (event.mask & IN_MOVE_SELF) {
              states = states | FileWatch::State::RENAMED;
            }
            if (event.mask & IN_DELETE_SELF) {
              states = states | FileWatch::State::DELETED;
            }
            if (event.mask & (IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE)) {
              if ((event.mask & IN_MOVED_TO) != IN_MOVED_TO || !event.cookie || event.cookie != m_moveCookie) {
                if (event.mask & IN_MOVED_FROM) {
                  m_moveCookie = event.cookie;
                }
                states = states | FileWatch::State::MODIFIED;
              }
            }
            lock.unlock();
            if (states != FileWatch::State::NONE) {
              try {
                callback(fileWatch, states);
              } catch (...) {}
            }
            fileWatch.reset(); // Mutex must be unlocked when the fileWatch goes out of scope
            lock.lock();
          }
        }
      }
      lock.unlock();
      offset += sizeof(inotify_event) + event.len;
    }
    if (offset < length && offset > 0) {
      // If we have any partial event data, move it to the beginning of the buffer
      ::memmove(buffer, buffer + offset, length - offset);
      offset = length - offset;
    } else {
      offset = 0;
    }
  }
}

std::shared_ptr<FileWatch> FileMonitorUnix::Watch(const boost::filesystem::path& path,
                                                  const t_callbackFunc& callback,
                                                  FileWatch::State states)
{
  std::shared_ptr<FileWatchUnix> fileWatch;

  if (states == FileWatch::State::NONE ||
      !boost::filesystem::exists(path)) {
    return fileWatch;
  }
  fileWatch = std::shared_ptr<FileWatchUnix>(new FileWatchUnix(path),
                                             [this] (FileWatchUnix* fileWatch) {
                                               const int wd = fileWatch->m_key;
                                               std::unique_lock<std::mutex> lock(m_mutex);
                                               auto found = m_watchers.find(wd);
                                               if (found != m_watchers.end()) {
                                                 auto& watchers = found->second;
                                                 for (auto iter = watchers.begin(); iter != watchers.end(); ++iter) {
                                                   if ((*iter)->fileWatch.expired()) {
                                                     watchers.erase(iter);
                                                     break;
                                                   }
                                                 }
                                                 if (watchers.empty()) {
                                                   // Remove from inotify
                                                   ::inotify_rm_watch(m_inotify, wd);
                                                   m_watchers.erase(found);
                                                 }
                                               }
                                               lock.unlock();
                                               delete fileWatch;
                                             });
  if (!fileWatch) {
    return fileWatch;
  }
  uint32_t mask = 0;

  if (states & FileWatch::State::RENAMED) {
    mask |= IN_MOVE_SELF;
  }
  if (states & FileWatch::State::DELETED) {
    mask |= IN_DELETE_SELF;
  }
  if (states & FileWatch::State::MODIFIED) {
    if (boost::filesystem::is_directory(fileWatch->Path())) {
      mask |= IN_CREATE;
      mask |= IN_DELETE;
      mask |= IN_MOVE;
    } else {
      mask |= IN_MODIFY;
    }
  }
  if (mask == 0) {
    fileWatch.reset();
    return fileWatch;
  }
  // Add to inotify
  std::unique_lock<std::mutex> lock(m_mutex);
  int wd = ::inotify_add_watch(m_inotify, fileWatch->Path().c_str(), mask);
  if (wd < 0) {
    lock.unlock();
    fileWatch.reset();
    return fileWatch;
  }
  fileWatch->m_key = wd;
  auto found = m_watchers.find(wd);
  auto watcher = std::make_shared<Watcher>(Watcher{fileWatch, callback});
  if (found == m_watchers.end()) {
    std::vector<std::shared_ptr<Watcher>> watchers;
    watchers.reserve(1);
    watchers.push_back(watcher);
    m_watchers[wd] = std::move(watchers);
  } else {
    found->second.push_back(watcher);
  }
  lock.unlock();
  return fileWatch;
}

int FileMonitorUnix::WatchCount() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_watchers.size();
}
