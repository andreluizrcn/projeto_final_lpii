#include "libtslog.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void writer_thread(int id, int messages) {
  for (int i = 0; i < messages; ++i) {
    TSLogger::instance().info("worker " + std::to_string(id) + " message " +
                              std::to_string(i));
    std::this_thread::sleep_for(std::chrono::milliseconds(5 + (id % 5)));
  }
}

int main(int argc, char **argv) {
  int threads = 8;
  int msgs = 100;
  if (argc >= 2)
    threads = std::stoi(argv[1]);
  if (argc >= 3)
    msgs = std::stoi(argv[2]);

  try {
    TSLogger::instance().init("test.log", false);
  } catch (const std::exception &e) {
    std::cerr << "Logger init failed: " << e.what()
              << '\n'; // Fix: Use '\n' for newline
    return 1;
  }

  std::vector<std::thread> tv;
  for (int i = 0; i < threads; ++i)
    tv.emplace_back(writer_thread, i, msgs);
  for (auto &t : tv)
    if (t.joinable())
      t.join();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  TSLogger::instance().shutdown();
  std::cout << "done. logs in test.log\n"; // Fix: Terminate string with '"' and
                                           // add a newline
  return 0;
}
