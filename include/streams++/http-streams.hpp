#pragma once
#include <streams++/types.hpp>

namespace streams {

namespace internal {
class Downloader;
}

class HttpReadableStream : public ReadableStream {

public:
  HttpReadableStream();
  ~HttpReadableStream();
  int read(unsigned char *buffer, int max_len) const override;

private:
  std::unique_ptr<internal::Downloader> m_worker;
};

namespace http {
std::unique_ptr<HttpReadableStream> get();
} // namespace http

} // namespace streams