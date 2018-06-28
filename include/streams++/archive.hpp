#pragma once
#include <memory>
#include <streams++/types.hpp>
#include <string>

namespace streams {

namespace internal {
class ArchivePrivate;
class ArchiveIteratorPrivate;
} // namespace internal

class Store {

public:
  virtual ~Store() {}

  virtual bool open_read() = 0;
  virtual bool open_write() = 0;
  virtual bool close_read() = 0;
  virtual bool close_write() = 0;
  virtual int read() = 0;
  virtual int write(void *buffer, size_t len) = 0;
};

enum class ArchiveStoreType { File, Memory, Custom };

struct EntryInfo {
  std::string path;
  int64_t size;
  mode_t mode;
};

class Archive {

public:
  class iterator {
  public:
    typedef iterator self_type;
    typedef EntryInfo value_type;
    typedef EntryInfo &reference;
    typedef EntryInfo *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(Archive *ptr);
    iterator(const iterator &) = delete;
    iterator &operator=(const iterator &) = delete;
    iterator(iterator &&);
    iterator &operator=(iterator &&);
    ~iterator();
    self_type &operator++();
    self_type &operator++(int junk);
    reference operator*();
    pointer operator->();
    bool operator==(const self_type &rhs);
    bool operator!=(const self_type &rhs);

  private:
    std::unique_ptr<internal::ArchiveIteratorPrivate> d;
  };
  Archive();
  Archive(const std::string &path);
  ~Archive();
  void mkdir(const std::string &path);
  std::unique_ptr<ReadableStream> read_file(const std::string &path) const;
  std::unique_ptr<WritableStream> create_file(const std::string &path);
  size_t size() const;
  iterator begin() { return iterator(this); }
  iterator end() { return iterator(nullptr); }

private:
  std::unique_ptr<internal::ArchivePrivate> d;
};

} // namespace streams