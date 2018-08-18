#include "archive-streams.hpp"
#include <archive.h>
#include <archive_entry.h>
#include <streams++/archive.hpp>

namespace streams {

namespace internal {

struct MStore {
  std::unique_ptr<char> buffer;
  size_t len;
  size_t used;
};

union storage {
  std::string path;
  std::unique_ptr<MStore> mem;
  std::unique_ptr<Store> custom;
  ~storage() {}
  storage() {}
};

struct Storage {
  Storage() {}
  ArchiveStoreType type;
  storage store;
};

class ArchivePrivate {
public:
  ArchivePrivate() {
    auto s = new Storage();
    s->type = ArchiveStoreType::Memory;
    s->store.mem = std::make_unique<MStore>();
    s->store.mem->buffer = std::make_unique<char>(4096);
    s->store.mem->len = 4096;
    storage.reset(s);
  }

  ArchivePrivate(const std::string &path) {
    auto s = new Storage();
    s->type = ArchiveStoreType::File;
    s->store.path = path;
    storage.reset(s);
  }

  ~ArchivePrivate() {}

  std::unique_ptr<Storage> storage;
};

class ArchiveIteratorPrivate {

public:
  ArchiveIteratorPrivate() {}
  ~ArchiveIteratorPrivate() {
    if (ptr) {
      archive_read_free(ptr);
    }
  }

  struct archive *ptr = nullptr;
  struct archive_entry *entry = nullptr;
  EntryInfo current;
  size_t index = 0;
};

} // namespace internal

static struct archive *
get_read_archive(std::unique_ptr<internal::Storage> &storage) {
  struct archive *a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);

  int ret = 0;

  switch (storage->type) {
  case ArchiveStoreType::Memory:
    // auto &mem = storage->store.mem;
    ret = archive_read_open_memory(a, storage->store.mem->buffer.get(),
                                   storage->store.mem->len);
    break;
  case ArchiveStoreType::File:
    ret = archive_read_open_filename(a, storage->store.path.c_str(), 1024);
    break;
  default:
    throw std::runtime_error("invalid storage");
  }

  if (ret != ARCHIVE_OK) {
    archive_read_free(a);
    throw new std::runtime_error("could not open storage");
  }

  return a;
}

static struct archive *
get_write_archive(std::unique_ptr<internal::Storage> &storage) {
  struct archive *a = archive_write_new();
  archive_write_add_filter_compress(a);
  archive_write_set_format_zip(a);

  int ret = 0;

  switch (storage->type) {
  case ArchiveStoreType::Memory:
    // auto &mem = storage->store.mem;
    ret = archive_write_open_memory(a, storage->store.mem->buffer.get(),
                                    storage->store.mem->len,
                                    &storage->store.mem->used);
    break;
  case ArchiveStoreType::File:
    ret = archive_write_open_filename(a, storage->store.path.c_str());
    break;
  default:
    throw std::runtime_error("invalid storage");
  }

  if (ret != ARCHIVE_OK) {
    archive_write_free(a);
    throw new std::runtime_error("could not open storage");
  }

  return a;
}

Archive::iterator::iterator(Archive *ptr)
    : d(new internal::ArchiveIteratorPrivate()) {
  if (ptr == nullptr) {
    // m_idx = -1;
    d->index = -1;
  } else {

    auto a = get_read_archive(ptr->d->storage);

    this->d->ptr = a;

    if (archive_read_next_header(a, &this->d->entry) != ARCHIVE_OK) {
      archive_read_free(a);
      throw std::runtime_error("could not read archive");
    }

    auto name = archive_entry_pathname(d->entry);
    d->current.path = name;
    d->current.size = archive_entry_size(d->entry);
    d->current.mode = archive_entry_mode(d->entry);

    archive_read_data_skip(a);
  }
}

Archive::iterator::iterator(iterator &&other) : d(std::move(other.d)) {}
Archive::iterator &Archive::iterator::operator=(iterator &&other) {
  if (this != &other) {
    this->d.swap(other.d);
  }

  return *this;
}

Archive::iterator::~iterator() {}

Archive::iterator::self_type &Archive::iterator::operator++() {

  if (!d->ptr)
    throw std::runtime_error("overflow");

  if (archive_read_next_header(d->ptr, &d->entry) != ARCHIVE_OK) {
    archive_read_free(d->ptr);
    d->ptr = nullptr;
    d->index = -1;
    return *this;
  }

  auto name = archive_entry_pathname(d->entry);
  d->current.path = name;
  d->current.size = archive_entry_size(d->entry);
  archive_read_data_skip(d->ptr);
  d->index++;
  return *this;
}
Archive::iterator::self_type &Archive::iterator::operator++(int junk) {
  if (!d->ptr)
    throw std::runtime_error("overflow");

  if (archive_read_next_header(d->ptr, &d->entry) != ARCHIVE_OK) {
    archive_read_free(d->ptr);
    d->ptr = nullptr;
    d->index = -1;
    return *this;
  }

  auto name = archive_entry_pathname(d->entry);
  d->current.path = name;
  d->current.size = archive_entry_size(d->entry);
  archive_read_data_skip(d->ptr);
  d->index++;
  return *this;
}
Archive::iterator::reference Archive::iterator::operator*() {
  return d->current;
}
Archive::iterator::pointer Archive::iterator::operator->() {
  return &d->current;
}
bool Archive::iterator::operator==(const Archive::iterator::self_type &rhs) {
  return d->index == rhs.d->index;
}
bool Archive::iterator::operator!=(const Archive::iterator::self_type &rhs) {
  return d->index != rhs.d->index;
}

Archive::Archive() : d(new internal::ArchivePrivate()) {}
Archive::Archive(const std::string &path)
    : d(new internal::ArchivePrivate(path)) {}

Archive::~Archive() {}

void Archive::mkdir(const std::string &path) {
  struct archive *a = get_write_archive(d->storage);

  struct archive_entry *entry = archive_entry_new();
  archive_entry_set_pathname(entry, path.c_str());
  archive_entry_set_mode(entry, S_IFDIR);
  if (archive_write_header(a, entry) != ARCHIVE_OK) {

    throw std::runtime_error("could not write header");
  }
  archive_entry_free(entry);
  archive_write_free(a);
}

std::unique_ptr<ReadableStream>
Archive::read_file(const std::string &path) const {
  struct archive *a = get_read_archive(d->storage);

  struct archive_entry *entry = nullptr;
  auto found = false;
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    auto name = archive_entry_pathname(entry);
    if (path == name) {
      found = true;
      break;
    }
    archive_read_data_skip(a);
  }

  if (found) {
    mode_t mode = archive_entry_filetype(entry);
    if (S_ISDIR(mode))
      throw std::runtime_error("not a file");
    return std::make_unique<ReadableArchiveStream>(a);
  }

  archive_read_free(a);

  return nullptr;
}

std::unique_ptr<WritableStream> Archive::create_file(const std::string &path) {

  struct archive *a = get_write_archive(d->storage);

  struct archive_entry *entry = archive_entry_new();
  archive_entry_set_pathname(entry, path.c_str());
  archive_entry_set_mode(entry, S_IFREG);
  if (archive_write_header(a, entry) != ARCHIVE_OK) {
    archive_entry_free(entry);
    archive_write_free(a);
    throw std::runtime_error("could not write header");
  }

  return std::make_unique<WritableArchiveStream>(a, entry);
}

} // namespace streams