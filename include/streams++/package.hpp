#pragma once
#include <streams++/types.hpp>

namespace streams {

class Package {

public:
  Package();
  ~Package();
  Package(const Package &) = delete;
  Package &operator=(const Package &) = delete;
  Package(Package &&);
  Package &operator=(Package &&);

  void set_path(const std::string &);
  std::string path() const;

  void set_content(const std::string &content);
  void set_content(std::unique_ptr<ReadableStream> &&);
  ReadableStream &content() const;

  template <typename T> static Package create(const std::string &path, T t) {
    Package pack;
    pack.set_path(path);
    pack.set_content(t);
    return std::move(pack)
  }

private:
  std::string m_path;
  std::unique_ptr<ReadableStream> m_content;
};

} // namespace streams