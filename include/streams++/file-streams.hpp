#pragma once
#include <fstream>
#include <streams++/types.hpp>
#include <string>

namespace streams {
namespace internal {
class ReadableFileStreamPrivate;
}

class ReadableFileStream : public ReadableStream, public SeekableStream {

public:
  ReadableFileStream(const std::string &path);
  ReadableFileStream(const ReadableFileStream &) = delete;
  ReadableFileStream &operator=(const ReadableFileStream &) = delete;
  ~ReadableFileStream();

  virtual int read(unsigned char *buffer, int max_len) const;
  virtual void seek(size_type pos, std::ios::seekdir dir);
  virtual size_type tell() const;

private:
  std::unique_ptr<internal::ReadableFileStreamPrivate> d;
};

class WritableFileStream : public WritableStream {

public:
  WritableFileStream(const std::string &path, int mode = std::fstream::out);

  virtual int write(unsigned char *buffer, int len);

private:
  std::string m_path;
  std::fstream m_stream;
};

} // namespace streams