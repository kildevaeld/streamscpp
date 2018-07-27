#pragma once
#include <streams++/path.hpp>
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

  void set_path(const Path &);
  Path path() const;

  void set_content(const std::string &content);
  void set_content(std::unique_ptr<ReadableStream> &&);
  ReadableStream &content() const;

  static Package create(const Path &path, const std::string &str) {
    Package pack;
    pack.set_path(path);
    pack.set_content(str);
    return std::move(pack);
  }

  static Package create(const Path &path,
                        std::unique_ptr<ReadableStream> &&stream) {
    Package pack;
    pack.set_path(path);
    pack.set_content(std::move(stream));
    return std::move(pack);
  }

private:
  Path m_path;
  std::unique_ptr<ReadableStream> m_content;
};

} // namespace streams