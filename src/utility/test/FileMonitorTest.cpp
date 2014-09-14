// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FileMonitorTest.h"
#include "FileMonitor.h"
#include <autowiring/autowiring.h>
#include <autowiring/CoreContext.h>
#include THREAD_HEADER

TEST_F(FileMonitorTest, MonitorDirectory) {
  AutoCurrentContext serverContext;
  AutoRequired<FileMonitor> fm;
  serverContext->Initiate();

  boost::filesystem::path parent = GetTemporaryPath();
  boost::filesystem::path one = parent / GetTemporaryName();
  boost::filesystem::path two = parent / GetTemporaryName();

  ASSERT_TRUE(boost::filesystem::create_directory(parent));
  ASSERT_TRUE(boost::filesystem::exists(parent));

  std::mutex mutex;
  std::condition_variable cond;

  std::atomic<bool> dirModified{false}, dirDeleted{false};

  auto dirWatcher = fm->Watch(parent.string(),
                              [&mutex, &cond, &dirModified, &dirDeleted]
                              (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                std::unique_lock<std::mutex> lock(mutex);
                                if (states & FileWatch::State::MODIFIED) {
                                  dirModified = true;
                                }
                                if (states & FileWatch::State::DELETED) {
                                  dirDeleted = true;
                                }
                              },
                              FileWatch::State::MODIFIED | FileWatch::State::DELETED);

  std::unique_lock<std::mutex> lock(mutex);
  cond.wait_for(lock, std::chrono::milliseconds(10),
                [&dirModified, &dirDeleted] { return (dirModified || dirDeleted); });
  ASSERT_FALSE(dirModified);
  ASSERT_FALSE(dirDeleted);

  // Create first file in parent directory
  SetFileContent(one, "one");
  ASSERT_TRUE(boost::filesystem::exists(one));
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirModified] () -> bool { return dirModified; });
  ASSERT_TRUE(dirModified);
  ASSERT_FALSE(dirDeleted);
  dirModified = false;

  // Create second file in parent directory
  SetFileContent(two, "two");
  ASSERT_TRUE(boost::filesystem::exists(two));
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirModified] () -> bool { return dirModified; });
  ASSERT_TRUE(dirModified);
  ASSERT_FALSE(dirDeleted);
  dirModified = false;

  // Remove first file
  boost::filesystem::remove(one);
  ASSERT_FALSE(boost::filesystem::exists(one));
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirModified] () -> bool { return dirModified; });
  ASSERT_TRUE(dirModified);
  ASSERT_FALSE(dirDeleted);
  dirModified = false;

  // Rename second file to first file
  boost::filesystem::rename(two, one);
  ASSERT_TRUE(boost::filesystem::exists(one));
  ASSERT_FALSE(boost::filesystem::exists(two));
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirModified] () -> bool { return dirModified; });
  ASSERT_TRUE(dirModified);
  ASSERT_FALSE(dirDeleted);
  dirModified = false;

  // Remove first file (which used to be second file)
  boost::filesystem::remove(one);
  ASSERT_FALSE(boost::filesystem::exists(one));
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirModified] () -> bool { return dirModified; });
  ASSERT_TRUE(dirModified);
  ASSERT_FALSE(dirDeleted);
  dirModified = false;

  // Remove parent directory
  boost::filesystem::remove(parent);
  try {
    ASSERT_FALSE(boost::filesystem::exists(parent));
  } catch(...) {}
  cond.wait_for(lock, std::chrono::milliseconds(100), [&dirDeleted] () -> bool { return dirDeleted; });
  ASSERT_TRUE(dirDeleted);
  ASSERT_FALSE(dirModified);
  dirDeleted = false;
}

TEST_F(FileMonitorTest, DISABLED_MonitorMultipleDeletes) {
  AutoCurrentContext serverContext;
  AutoRequired<FileMonitor> fm;
  serverContext->Initiate();

  boost::filesystem::path one = GetTemporaryPath();
  boost::filesystem::path two  = GetTemporaryPath();

  SetFileContent(one, "one");
  SetFileContent(two, "two");

  std::atomic<bool> oneDeleted{false}, twoDeleted{false};

  auto oneWatcher = fm->Watch(one.string(),
                              [&oneDeleted]
                              (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                if (states == FileWatch::State::DELETED) { oneDeleted = true; }
                              },
                              FileWatch::State::DELETED);
  auto twoWatcher = fm->Watch(two.string(),
                              [&twoDeleted]
                              (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                if (states == FileWatch::State::DELETED) { twoDeleted = true; }
                              },
                              FileWatch::State::DELETED);

  boost::filesystem::remove(two);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_TRUE(twoDeleted);
  ASSERT_FALSE(oneDeleted);
  boost::filesystem::remove(one);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_TRUE(oneDeleted);
}

TEST_F(FileMonitorTest, DISABLED_MultipleWatchersSameFile) {
  AutoCreateContext serverContext;
  AutoRequired<FileMonitor> fm;
  serverContext->Initiate();

  std::mutex mutex;
  std::condition_variable cond;

  boost::filesystem::path one = GetTemporaryPath();
  boost::filesystem::path two = GetTemporaryPath();
  SetFileContent(one, "one");
  SetFileContent(two, "two");
  ASSERT_TRUE(boost::filesystem::exists(two));

  std::atomic<int> oneOneDeleted{0}, twoOneDeleted{0}, oneTwoDeleted{0}, twoTwoDeleted{0};

  auto oneOneWatcher = fm->Watch(one.string(),
                                 [&mutex, &cond, &oneOneDeleted]
                                 (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                   ASSERT_EQ(FileWatch::State::DELETED, states);
                                   std::unique_lock<std::mutex> lock(mutex);
                                   ++oneOneDeleted;
                                   cond.notify_all();
                                 },
                                 FileWatch::State::DELETED);
  auto twoOneWatcher = fm->Watch(one.string(),
                                 [&mutex, &cond, &twoOneDeleted]
                                 (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                   ASSERT_EQ(FileWatch::State::DELETED, states);
                                   std::unique_lock<std::mutex> lock(mutex);
                                   ++twoOneDeleted;
                                   cond.notify_all();
                                 },
                                 FileWatch::State::DELETED);
  auto oneTwoWatcher = fm->Watch(two.string(),
                                 [&mutex, &cond, &oneTwoDeleted]
                                 (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                   ASSERT_EQ(FileWatch::State::DELETED, states);
                                   std::unique_lock<std::mutex> lock(mutex);
                                   ++oneTwoDeleted;
                                   cond.notify_all();
                                 },
                                 FileWatch::State::DELETED);
  auto twoTwoWatcher = fm->Watch(two.string(),
                                 [&mutex, &cond, &twoTwoDeleted]
                                 (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                                   ASSERT_EQ(FileWatch::State::DELETED, states);
                                   std::unique_lock<std::mutex> lock(mutex);
                                   ++twoTwoDeleted;
                                   cond.notify_all();
                                 },
                                 FileWatch::State::DELETED);

  std::unique_lock<std::mutex> lock(mutex);
  ASSERT_TRUE(nullptr != oneOneWatcher.get());
  ASSERT_TRUE(nullptr != twoOneWatcher.get());
  ASSERT_TRUE(nullptr != oneTwoWatcher.get());
  ASSERT_TRUE(nullptr != twoTwoWatcher.get());
  boost::filesystem::remove(one);
  cond.wait_for(lock, std::chrono::milliseconds(500),
                [&oneOneDeleted, &twoOneDeleted] { return oneOneDeleted && twoOneDeleted; });
  ASSERT_EQ(1, oneOneDeleted);
  ASSERT_EQ(1, twoOneDeleted);
  ASSERT_EQ(0, oneTwoDeleted);
  ASSERT_EQ(0, twoTwoDeleted);
  oneOneDeleted = 0;
  twoOneDeleted = 0;
  boost::filesystem::remove(two);
  cond.wait_for(lock, std::chrono::milliseconds(500),
                [&oneTwoDeleted, &twoTwoDeleted] { return oneTwoDeleted && twoTwoDeleted; });
  ASSERT_EQ(0, oneOneDeleted);
  ASSERT_EQ(0, twoOneDeleted);
  ASSERT_EQ(1, oneTwoDeleted);
  ASSERT_EQ(1, twoTwoDeleted);
}

TEST_F(FileMonitorTest, FileNotFound) {
  AutoCreateContext serverContext;
  AutoRequired<FileMonitor> fm;
  serverContext->Initiate();

  boost::filesystem::path missing = GetTemporaryPath();

  auto watcher = fm->Watch(missing.string(),
                           []
                           (std::shared_ptr<FileWatch> fileWatch, FileWatch::State states) {
                           },
                           FileWatch::State::ALL);
  ASSERT_TRUE(nullptr == watcher.get());
}
