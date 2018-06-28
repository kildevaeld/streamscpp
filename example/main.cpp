#include <streams++/archive.hpp>
#include <streams++/streams.hpp>

using namespace streams;

int main() {

  Archive a("rapper.zip");

  // for (auto &entry : a) {
  //   std::cout << entry.path << std::endl;
  // }

  {
    auto file = a.create_file("test.txt");

    file->write((unsigned char *)"Hello", 6);
    file->write(NULL, 0);
  }

  std::cout << "write " << std::endl;
  // auto stream = a.read_file("build/CMakeCache.txt");
  // std::cout << *stream << std::endl;
  // WritableFileStream out("test_out.txt");
  // ReadableFileStream in("../src/utils.cpp");
  // StringStream out;

  // pipe(in, out);
  // out.seek(0, std::ios::beg);
  // std::cout << "out " << out << std::endl;
  return 0;
}
