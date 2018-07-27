#include <streams++/memory-streams.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace streams {

namespace internal {
class StringStreamPrivate {

public:
  StringStreamPrivate(const std::string &str) : buffer(str) {}
  ~StringStreamPrivate() {}
  std::string buffer;
  size_type offset = 0;
};
} // namespace internal

StringStream::StringStream(const std::string &str)
    : d(new internal::StringStreamPrivate(str)) {}

StringStream::~StringStream() {}

int StringStream::read(unsigned char *buffer, int max_len) const {
  auto size = d->buffer.size();

  if (d->offset >= size) {
    return EOS;
  }

  int r = max_len;
  if (d->offset + r > size) {
    r = size - d->offset;
  }

  auto data = d->buffer.data();

  std::memcpy(buffer, data + d->offset, r);
  d->offset += r;

  return r;
}

int StringStream::write(unsigned char *buffer, int len) {
  if (d->offset == d->buffer.size()) {
    d->buffer.append((char *)buffer, len);
    d->offset += len;
    return len;
  }

  return EOS;
}

void StringStream::seek(size_type pos, std::ios::seekdir dir) {
  auto p = pos;
  switch (dir) {
  case std::ios::cur:
    p += d->offset;
    break;
  case std::ios::beg:
    p = pos;
    break;
  case std::ios::end:
    p = d->buffer.size() - pos;
    break;
  }
  if (p < 0 || p >= d->buffer.size()) {
    throw std::runtime_error("overflow");
  }

  d->offset = p;
}
size_type StringStream::tell() const { return d->offset; }

std::string StringStream::str() const { return d->buffer; }

size_t StringStream::size() const { return d->buffer.size(); }

namespace internal {

class BufferedMemoryStreamPrivate {
public:
  BufferedMemoryStreamPrivate(size_t m) : max(m) {}
  std::string buffer;
  std::mutex mutex;
  std::condition_variable cond;
  std::atomic<bool> done;
  size_t max;
};
} // namespace internal

BufferedMemoryStream::BufferedMemoryStream(size_t max)
    : d(new internal::BufferedMemoryStreamPrivate(max)) {}

BufferedMemoryStream ::~BufferedMemoryStream() {
  d->done = true;
  d->cond.notify_all();
}

int BufferedMemoryStream::read(unsigned char *buffer, int max_len) const {
  if (d->done)
    return EOS;

  std::unique_lock<std::mutex> lock(d->mutex);

  if (d->buffer.size() == 0) {
    d->cond.wait(lock, [this]() { return d->done || d->buffer.size() > 0; });
  }

  int read = std::min(d->buffer.size(), (size_t)max_len);
  std::memcpy(buffer, d->buffer.data(), read);
  if (read == d->buffer.size())
    d->buffer.clear();
  else
    d->buffer.erase(0, read);

  lock.unlock();
  d->cond.notify_all();

  return read;
}
int BufferedMemoryStream::write(unsigned char *buffer, int len) {

  if (buffer == nullptr && len == 0) {
    d->done = true;
    d->cond.notify_all();
    return 0;
  }
  std::unique_lock<std::mutex> lock(d->mutex);
  if (d->buffer.size() + len > d->max) {
    d->cond.wait(lock,
                 [this, len]() { return d->buffer.size() + len < d->max; });
  }
  d->buffer.append((char *)buffer, len);
  lock.unlock();
  d->cond.notify_all();

  return len;
}

} // namespace streams