#include <streams++/streams.hpp>

using namespace streams;

int main() {

  // WritableFileStream out("test_out.txt");
  ReadableFileStream in("../src/utils.cpp");
  StringStream out;

  pipe(in, out);
  out.seek(0, std::ios::beg);
  std::cout << "out " << out << std::endl;
  return 0;
}
