#ifndef LIBTSLOG_H
#define LIBTSLOG_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

// --- Níveis de log ---
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

// --- Fila thread-safe ---
template <typename T> class ThreadSafeQueue {
public:
  ThreadSafeQueue() : done_(false) {}

  void push(T v) {
    std::lock_guard<std::mutex> lg(m_);
    q_.push(std::move(v));
    cv_.notify_one();
  }

  bool pop(T &out) {
    std::lock_guard<std::mutex> lg(m_);
    if (q_.empty())
      return false;
    out = std::move(q_.front());
    q_.pop();
    return true;
  }

  // pop bloqueante
  T wait_pop() {
    std::unique_lock<std::mutex> ul(m_);
    cv_.wait(ul, [&] { return !q_.empty() || done_; });
    if (q_.empty())
      return T();
    T v = std::move(q_.front());
    q_.pop();
    return v;
  }

  void notify_done() {
    std::lock_guard<std::mutex> lg(m_);
    done_ = true;
    cv_.notify_all();
  }

  bool empty() {
    std::lock_guard<std::mutex> lg(m_);
    return q_.empty();
  }

private:
  std::queue<T> q_;
  std::mutex m_;
  std::condition_variable cv_;
  bool done_;
};

// --- Logger thread-safe ---
class TSLogger {
public:
  TSLogger(const TSLogger &) = delete;
  TSLogger &operator=(const TSLogger &) = delete;

  static TSLogger &instance();

  void init(const std::string &logfile, bool append = true);
  void shutdown();

  void log(LogLevel level, const std::string &msg);
  void debug(const std::string &msg) { log(LogLevel::DEBUG, msg); }
  void info(const std::string &msg) { log(LogLevel::INFO, msg); }
  void warn(const std::string &msg) { log(LogLevel::WARN, msg); }
  void error(const std::string &msg) { log(LogLevel::ERROR, msg); }

  void flush();

private:
  TSLogger();
  ~TSLogger();

  void worker_thread_fn();

  std::atomic<bool> running_;
  ThreadSafeQueue<std::string> queue_;
  std::thread worker_;
  std::mutex file_mtx_;
  std::ofstream ofs_;
};

#endif // LIBTSLOG_H
