#include <streams++/file-streams.hpp>
#include <streams++/producers/directory.hpp>

namespace streams {
namespace producers {

GlobProducer::GlobProducer(const std::string &pattern) : m_pattern(pattern) {
  glob(pattern.c_str(), GLOB_TILDE | GLOB_BRACE, NULL, &m_result);
}

GlobProducer::~GlobProducer() { globfree(&m_result); }

static size_t findGlobStart(const std::string &path) {
  size_t i = 0;
  for (auto c : path) {
    switch (c) {
    case '{':
    case '*':
      return i;
    }
    i++;
  }
  return -1;
}

bool GlobProducer::read(Package &pack) const {

  if (m_pos >= m_result.gl_pathc) {
    return false;
  }

  std::string path(m_result.gl_pathv[m_pos++]);

  Path base(path.substr(0, findGlobStart(m_pattern)));

  Path p(path);
  pack.set_base(base.resolve().str());
  pack.set_path(p.resolve());
  pack.set_content(read_file(path));

  return true;
}

} // namespace producers
} // namespace streams