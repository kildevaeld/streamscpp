#include <streams++/memory-streams.hpp>
#include <streams++/package.hpp>

namespace streams {

Package::Package() : m_content(new StringStream) {}

Package::~Package() {}

Package::Package(Package &&other)
    : m_content(std::move(other.m_content)), m_path(other.m_path) {}

Package &Package::operator=(Package &&other) {
  if (this != &other) {
    m_content.swap(other.m_content);
    m_path.swap(other.m_path);
  }
  return *this;
}

void Package::set_path(const std::string &path) {}

std::string Package::path() const { return m_path; }

void Package::set_content(std::unique_ptr<ReadableStream> &&stream) {
  m_content = std::move(stream);
}

void Package::set_content(const std::string &content) {
  m_content = std::unique_ptr<StringStream>(new StringStream(content));
}

ReadableStream &Package::content() const { return *m_content.get(); }

} // namespace streams