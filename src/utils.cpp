#include <streams++/utils.hpp>

namespace streams {

void pipe(const ReadableStream &source, WritableStream &dest) {
  int read = 0;
  unsigned char buffer[2024];
  while (read != EOS) {
    read = source.read(buffer, 2024);
    if (read == EOS || read == 0)
      break;
    dest.write(buffer, read);
  }
}

void read_all(const ReadableStream &source, std::string &out) {
  int read = 0;
  unsigned char buffer[2024];
  while (read != EOS) {
    read = source.read(buffer, 2024);
    if (read == EOS)
      break;
    out.append((char *)buffer, read);
  }
}

std::string read_all(const ReadableStream &source) {
  std::string out;
  read_all(source, out);
  return std::move(out);
}

std::ostream &operator<<(std::ostream &os, const ReadableStream &source) {
  int read = 0;
  unsigned char buffer[2024];
  while (read != EOS) {
    read = source.read(buffer, 2024);
    if (read == EOS) {
      break;
    }

    os.write((char *)buffer, read);
  }
  return os;
}

namespace utils {
bool replace(std::string &str, const std::string &from, const std::string &to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

bool replace_all(std::string &str, const std::string &from,
                 const std::string &to) {
  if (from.empty())
    return false;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing
                              // 'x' with 'yx'
  }
  return true;
}
} // namespace utils

} // namespace streams