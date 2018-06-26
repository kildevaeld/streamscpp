#pragma once
#include <iostream>
#include <stddef.h>

namespace streams {

static const int EOS = -1;
using size_type = size_t;

class ReadableStream {

public:
  virtual ~ReadableStream() {}
  virtual int read(unsigned char *buffer, int max_len) const = 0;
};

class SeekableStream {

public:
  virtual ~SeekableStream(){};

  virtual void seek(size_type pos, std::ios::seekdir dir) = 0;
  virtual size_type tell() const = 0;
  virtual size_type size() {
    auto offset = tell();
    seek(0, std::ios::end);
    auto size = tell();
    seek(offset, std::ios::beg);
    return size;
  }
};

class WritableStream {

public:
  virtual ~WritableStream(){};
  virtual int write(unsigned char *buffer, int len) = 0;
};
} // namespace streams