#include "downloader.hpp"
#include <algorithm>

namespace streams {
namespace internal {

Downloader::Downloader() {
  m_curl = curl_easy_init();
  m_thread = new std::thread(std::bind(&Downloader::thread_handler, this));
}
Downloader::~Downloader() {

  m_state = quit;
  m_cond.notify_all();

  if (m_thread->joinable()) {
    m_thread->join();
  }
  delete m_thread;
  curl_easy_cleanup(m_curl);
}

int Downloader::read(unsigned char *buffer, int max_len) {
  if (m_state == idle) {
    m_state = running;
    m_cond.notify_one();
  }

  return m_buf.read(buffer, max_len);
}

void Downloader::thread_handler() {

  if (m_state != running) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock,
                [this]() { return m_state == running || m_state == quit; });
  }

  if (m_state != running)
    return;

  curl_easy_setopt(m_curl, CURLOPT_URL, "https://www.google.com/");

  /* Switch on full protocol/debug output while testing */
  // curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

  /* disable progress meter, set to 0L to enable and disable debug output */
  curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);

  /* send all data to this function  */
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);

  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

  curl_easy_perform(m_curl);

  std::unique_lock<std::mutex> lock(m_mutex);
  lock.unlock();

  m_buf.write(nullptr, 0);

  if (m_state == running) {
    m_state = done;
    m_cond.notify_one();
  }
}

size_t Downloader::write_data(void *ptr, size_t size, size_t nmemb,
                              void *stream) {

  Downloader *worker = reinterpret_cast<Downloader *>(stream);
  return worker->m_buf.write((unsigned char *)ptr, size * nmemb);
}

} // namespace internal
} // namespace streams