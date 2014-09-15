// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FileMonitorTest.h"
#include "FileMonitor.h"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>
#include THREAD_HEADER

TEST_F(FileMonitorTest, MonitorFileStateChanges) {
  AutoCreateContext serverContext;
  AutoRequired<FileMonitor> fm;
  serverContext->Initiate();

  boost::filesystem::path original = GetTemporaryPath();
  boost::filesystem::path renamed  = GetTemporaryPath();

  ASSERT_TRUE(SetFileContent(original, "")) << "Unable to create temporary file";

  std::mutex mutex;
  std::condition_variable cond;
  std::atomic<FileWatch::State> eventStates{FileWatch::State::NONE}, matchStates{FileWatch::State::NONE};

  auto watcher = fm->Watch(original,
                           [&mutex, &cond, &matchStates, &eventStates]
                           (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                             std::unique_lock<std::mutex> lock(mutex);
                             if (states & matchStates) {
                               eventStates = states;
                               cond.notify_all();
                             }
                           },
                           FileWatch::State::ALL);
  // Validate basic behavior
  ASSERT_TRUE(nullptr != watcher.get());
  ASSERT_TRUE(watcher.unique());
  ASSERT_EQ(original.wstring(), watcher->Path().wstring());
  ASSERT_EQ(1, fm->WatchCount());

  // Make sure that we don't receive any events before anything happens to the file
  std::unique_lock<std::mutex> lock(mutex);
  cond.wait_for(lock, std::chrono::milliseconds(10),
                [&eventStates] { return eventStates != FileWatch::State::NONE; });
  ASSERT_EQ(FileWatch::State::NONE, eventStates);
  // For next test, check for all (we should only receive MODIFIED)
  matchStates = FileWatch::State::ALL;
  lock.unlock();

  // ACTION: Write to the file
  SetFileContent(original, "content");
  // Wait for the notification, or timeout due to it not happening
  lock.lock();
  cond.wait_for(lock, std::chrono::milliseconds(500),
                [&eventStates] { return eventStates == FileWatch::State::MODIFIED; });
  ASSERT_EQ(FileWatch::State::MODIFIED, eventStates) << "State is not solely in the MODIFIED state";
  // For next test, only check for RENAMED and DELETED (we may receive MODIFIED, so ignore those)
  matchStates = FileWatch::State::RENAMED | FileWatch::State::DELETED;
  eventStates = FileWatch::State::NONE;
  lock.unlock();

  // ACTION: Rename the file
  boost::filesystem::rename(original, renamed);
  // Wait for the notification, or timeout due to it not happening
  lock.lock();
  cond.wait_for(lock, std::chrono::milliseconds(500),
                [&eventStates] { return eventStates == FileWatch::State::RENAMED; });
  ASSERT_EQ(FileWatch::State::RENAMED, eventStates) << "State is not solely in the RENAMED state";
  // For next test, only check for DELETED events (we may receive RENAMED OR MODIFIED, so ignore those)
  matchStates = FileWatch::State::DELETED;
  eventStates = FileWatch::State::NONE;
  lock.unlock();

  // ACTION: Delete the file
  boost::filesystem::remove(renamed);
  // Wait for the notification, or timeout due to it not happening
  lock.lock();
  cond.wait_for(lock, std::chrono::milliseconds(500),
                [&eventStates] { return eventStates == FileWatch::State::DELETED; });
  ASSERT_EQ(FileWatch::State::DELETED, eventStates) << "State is not solely in the DELETED state";
  lock.unlock();

  std::weak_ptr<FileWatch> watcherWeak = watcher;
  watcher.reset();

  auto start = std::chrono::system_clock::now();
  while(!watcherWeak.expired())
    ASSERT_TRUE(start + std::chrono::seconds(1) > std::chrono::system_clock::now()) << "Released watcher shared pointer took too long to expire";
  ASSERT_EQ(0, fm->WatchCount()) << "Did not properly cleanup after removing watcher";
}
