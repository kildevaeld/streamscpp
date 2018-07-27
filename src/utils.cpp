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
} // namespace streams