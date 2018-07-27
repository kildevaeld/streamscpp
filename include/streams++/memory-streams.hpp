#pragma once
#include <memory>
#include <streams++/types.hpp>
#include <string>

namespace streams {

namespace internal {
class StringStreamPrivate;
class BufferedMemoryStreamPrivate;
} // namespace internal

class StringStream : public ReadableStream,
                     public WritableStream,
                     public SeekableStream {

public:
  StringStream(const std::string &str = std::string());
  StringStream(const StringStream &) = delete;
  ~StringStream();
  virtual int read(unsigned char *buffer, int max_len) const;
  virtual int write(unsigned char *buffer, int len);
  virtual void seek(size_type pos, std::ios::seekdir dir);
  virtual size_type tell() const;

  size_t size() const;

  std::string str() const;

private:
  std::unique_ptr<internal::StringStreamPrivate> d;
};

class BufferedMemoryStream : public ReadableStream, public WritableStream {
public:
  BufferedMemoryStream(size_t max = 4096);
  BufferedMemoryStream(const BufferedMemoryStream &) = delete;
  ~BufferedMemoryStream();
  virtual int read(unsigned char *buffer, int max_len) const;
  virtual int write(unsigned char *buffer, int len);

private:
  std::unique_ptr<internal::BufferedMemoryStreamPrivate> d;
};

} // namespace streams