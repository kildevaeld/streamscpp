#include <streams++/file-streams.hpp>
#include <streams++/producers/directory.hpp>

namespace streams {
namespace producers {

GlobProducer::GlobProducer(const std::string &pattern) : m_pattern(pattern) {
  glob(pattern.c_str(), GLOB_TILDE | GLOB_BRACE, NULL, &m_result);
}

GlobProducer::~GlobProducer() { globfree(&m_result); }

bool GlobProducer::read(Package &pack) const {

  if (m_pos >= m_result.gl_pathc) {
    return false;
  }

  std::string path(m_result.gl_pathv[m_pos++]);

  pack.set_path(path);
  pack.set_content(read_file(path));

  return true;
}

} // namespace producers
} // namespace streams