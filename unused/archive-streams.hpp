#pragma once
#include <archive.h>
#include <archive_entry.h>
#include <streams++/types.hpp>

namespace streams {

class ReadableArchiveStream : public ReadableStream {

public:
  ReadableArchiveStream(struct archive *a) : m_a(a) {}
  virtual ~ReadableArchiveStream() { archive_free(m_a); }

  int read(unsigned char *buffer, int max_len) const override {
    int ret = archive_read_data(m_a, buffer, max_len);

    if (ret == 0) {
      return EOS;
    } else if (ret == ARCHIVE_FATAL) {
      throw std::runtime_error("fatal error");
    } else if (ret == ARCHIVE_RETRY) {
      return read(buffer, max_len);
    }

    return ret;
  }

private:
  struct archive *m_a;
  ;
};

class WritableArchiveStream : public WritableStream {

public:
  WritableArchiveStream(struct archive *a, struct archive_entry *e)
      : m_a(a), m_e(e) {}
  ~WritableArchiveStream() {

    if (m_e)
      archive_entry_free(m_e);
    if (m_a)
      archive_write_free(m_a);
  }

  int write(unsigned char *buffer, int len) override {
    if (buffer == NULL) {
      archive_write_finish_entry(m_a);
      return EOS;
    }
    if (archive_write_data(m_a, buffer, len) < 0) {
      throw std::runtime_error("error");
    }
  }

private:
  struct archive *m_a = nullptr;
  struct archive_entry *m_e = nullptr;
};

} // namespace streams