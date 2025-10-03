#ifndef LIBTSLOG_H
#define LIBTSLOG_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

class ThreadSafeQueue {
public:
  void push(const std::string &item);
  std::string wait_pop();
  void notify_done();
  bool empty() const;

private:
  mutable std::mutex mtx_;
  std::condition_variable cv_;
  std::queue<std::string> queue_;
  bool done_ = false;
};

class TSLogger {
public:
  static TSLogger &instance();

  void init(const std::string &logfile, bool append = false);
  void shutdown();

  void debug(const std::string &msg) { log(LogLevel::DEBUG, msg); }
  void info(const std::string &msg) { log(LogLevel::INFO, msg); }
  void warn(const std::string &msg) { log(LogLevel::WARN, msg); }
  void error(const std::string &msg) { log(LogLevel::ERROR, msg); }
  void flush();

private:
  TSLogger();
  ~TSLogger();
  TSLogger(const TSLogger &) = delete;
  TSLogger &operator=(const TSLogger &) = delete;

  void log(LogLevel level, const std::string &msg);
  void worker_thread_fn();

  std::ofstream ofs_;
  std::mutex file_mtx_;
  std::thread worker_;
  std::atomic<bool> running_;
  ThreadSafeQueue queue_;
};

#endif
