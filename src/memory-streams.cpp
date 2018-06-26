#include <streams++/memory-streams.hpp>

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
    std::cout << d->offset << " " << size << std::endl;
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

} // namespace streams