#include <boost/filesystem.hpp>
#include <streams++/file-streams.hpp>
#include <streams++/transformers/directory.hpp>
#include <streams++/utils.hpp>

namespace streams {

namespace fs = boost::filesystem;

namespace transformers {

DirectoryDestination::DirectoryDestination(const std::string &path)
    : m_path(path) {

  Path p(m_path);
  if (!p.is_absolute()) {
    m_path = p.resolve().str();
  }

  if (path[path.size() - 1] != '/')
    m_path += "/";

  if (!fs::is_directory(m_path)) {
    fs::create_directories(m_path);
  }
}

bool DirectoryDestination::transform(Package &package) const {

  Path dest(m_path);

  auto file = package.path().str();
  utils::replace(file, package.base(), "");

  dest.append(file);

  auto dir = dest.dir();
  if (!fs::is_directory(dir)) {
    fs::create_directories(dir);
  }

  WritableFileStream out(dest.str());

  pipe(package.content(), out);

  return false;
}

} // namespace transformers

} // namespace streams