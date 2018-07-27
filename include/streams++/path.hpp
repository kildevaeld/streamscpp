#pragma once
#include <streams++/utils.hpp>
#include <string>
#include <syrup/path.h>

namespace streams {

static inline std::string path_join(const std::string &path,
                                    const std::string &comp) {
  if (path.size() == 0)
    return comp;
  else if (comp.size() == 0)
    return path;
  auto out = sy_path_join(NULL, path.c_str(), comp.c_str(), NULL);
  std::string p(out);
  free(out);
  return p;
}

struct Path {

public:
  Path() {}
  Path(const Path &path) : m_path(path.m_path) {}
  Path(Path &&path) : m_path(std::move(path.m_path)) {}
  Path &operator=(const Path &path) {
    if (this != &path) {

      m_path = path.m_path;
    }
    return *this;
  }

  Path(const std::string &path) : m_path(path) {}
  Path(const char *path) : m_path(path) {}

  void set_dir(const std::string &dir) { m_path = path_join(dir, base()); }
  void set_base(const std::string &base) { m_path = path_join(dir(), base); }
  void set_ext(const std::string &e) {
    if (ext().size() != 0) {
      utils::replace(m_path, ext(), e);
    } else {
      m_path += e;
    }
  }

  std::string dir() const {
    auto idx = sy_path_dir(m_path.c_str());
    return m_path.substr(0, idx);
  }
  std::string base() const {
    size_t idx;
    auto size = sy_path_base(m_path.c_str(), &idx);

    if (size == 0) {
      return m_path;
    }

    return m_path.substr(idx);
  }

  std::string ext() const {
    size_t idx;
    auto size = sy_path_ext(m_path.c_str(), &idx);
    if (size == 0)
      return "";
    return m_path.substr(idx);
  }

  void append(const Path &comp) { m_path = path_join(m_path, comp.str()); }

  std::string str() const { return m_path; }

  void swap(Path &path) { m_path.swap(path.m_path); }
  std::string::iterator begin() { return m_path.begin(); }
  std::string::iterator end() { return m_path.end(); }

private:
  std::string m_path;
}; // namespace gencore
} // namespace streams