#include <fstream>
#include <streams++/file-streams.hpp>

namespace streams {

namespace internal {
class ReadableFileStreamPrivate {

public:
  ReadableFileStreamPrivate(const std::string &p)
      : path(p), stream(p, std::ios::binary | std::ios::in) {
    if (!stream.is_open()) {
      throw std::runtime_error("could not open file");
    }
    stream.seekg(0, std::ios::end);
    len = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  std::string path;
  std::fstream stream;
  size_t offset = 0;
  size_t len = 0;
};
} // namespace internal

ReadableFileStream::ReadableFileStream(const std::string &path)
    : d(new internal::ReadableFileStreamPrivate(path)) {}

ReadableFileStream::~ReadableFileStream() {}

int ReadableFileStream::read(unsigned char *buffer, int max_len) const {

  if (d->offset >= d->len)
    return EOS;

  d->stream.seekg(d->offset);
  int r = max_len;
  auto e = max_len + d->offset;
  if (e > d->len) {
    r = d->len - d->offset;
  }

  d->stream.read((char *)buffer, r);

  if (!d->stream) {
    r = d->stream.gcount();
  }

  d->offset += r;

  return r;
}

void ReadableFileStream::seek(size_t pos, std::ios::seekdir dir) {
  d->stream.seekg((std::streampos)pos, dir);
  d->offset = d->stream.tellg();
}

size_type ReadableFileStream::tell() const { return d->stream.tellg(); }

WritableFileStream::WritableFileStream(const std::string &path, int mode)
    : m_path(path), m_stream(path, mode) {
  if (!m_stream.is_open()) {
    throw std::runtime_error("could not open file");
  }
}

int WritableFileStream::write(unsigned char *buffer, int len) {
  m_stream.write((char *)buffer, len);
  return len;
}

} // namespace streams