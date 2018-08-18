#include <streams++/memory-streams.hpp>
#include <streams++/package.hpp>

namespace streams {

Package::Package() : m_content(new StringStream) {}

Package::~Package() {}

Package::Package(Package &&other)
    : m_content(std::move(other.m_content)), m_path(other.m_path),
      m_base(std::move(other.m_base)) {}

Package &Package::operator=(Package &&other) {
  if (this != &other) {
    m_content.swap(other.m_content);
    m_path.swap(other.m_path);
    m_base.swap(other.m_base);
  }
  return *this;
}

void Package::set_path(const Path &path) { m_path = path; }

Path Package::path() const { return m_path; }

void Package::set_base(const std::string &base) { m_base = base; }
std::string Package::base() const { return m_base; }

void Package::set_cwd(const std::string &cwd) {}
std::string Package::cwd() const {}

void Package::set_content(std::unique_ptr<ReadableStream> &&stream) {
  m_content = std::move(stream);
}

void Package::set_content(const std::string &content) {
  m_content = std::unique_ptr<StringStream>(new StringStream(content));
}

ReadableStream &Package::content() const { return *m_content.get(); }

} // namespace streams