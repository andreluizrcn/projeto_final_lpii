#include "libtslog.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <thread>

TSLogger &TSLogger::instance() {
  static TSLogger logger;
  return logger;
}

TSLogger::TSLogger() : running_(false) {}

TSLogger::~TSLogger() {
  if (running_)
    shutdown();
}

void TSLogger::init(const std::string &logfile, bool append) {
  std::lock_guard<std::mutex> lg(file_mtx_);
  ofs_.open(logfile, append ? std::ios::app : std::ios::trunc);
  if (!ofs_.is_open())
    throw std::runtime_error("Failed to open log file");

  running_ = true;
  worker_ = std::thread(&TSLogger::worker_thread_fn, this);
}

void TSLogger::shutdown() {
  running_ = false;
  queue_.notify_done();
  if (worker_.joinable())
    worker_.join();

  std::lock_guard<std::mutex> lg(file_mtx_);
  if (ofs_.is_open())
    ofs_.close();
}

static std::string level_to_string(LogLevel level) {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  }
  return "UNKNOWN";
}

void TSLogger::log(LogLevel level, const std::string &msg) {
  if (!running_)
    throw std::runtime_error("Logger not initialized");

  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [T"
      << std::this_thread::get_id() << "] "
      << "[" << level_to_string(level) << "] " << msg << '\n';

  queue_.push(oss.str());
}

void TSLogger::flush() {
  std::lock_guard<std::mutex> lg(file_mtx_);
  if (ofs_.is_open())
    ofs_.flush();
}

void TSLogger::worker_thread_fn() {
  while (true) {
    std::string line = queue_.wait_pop();
    if (line.empty()) {
      if (!running_ && queue_.empty())
        break;
      if (queue_.empty())
        continue;
    }
    std::lock_guard<std::mutex> lg(file_mtx_);
    if (ofs_.is_open())
      ofs_ << line;
  }

  std::lock_guard<std::mutex> lg(file_mtx_);
  if (ofs_.is_open())
    ofs_.flush();
}
