#pragma once
#include <atomic>
#include <condition_variable>
#include <curl/curl.h>
#include <mutex>
#include <streams++/memory-streams.hpp>
#include <streams++/types.hpp>
#include <string>
#include <thread>

namespace streams {

namespace internal {

class Downloader {
  enum State { idle, running, done, quit };

public:
  Downloader();
  ~Downloader();

  int read(unsigned char *buffer, int max_len);

private:
  void thread_handler();
  static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

  std::thread *m_thread = nullptr;
  CURL *m_curl;
  std::mutex m_mutex;
  std::condition_variable m_cond;
  BufferedMemoryStream m_buf;
  std::atomic<State> m_state = {idle};
};
} // namespace internal

} // namespace streams