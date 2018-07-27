#include <streams++/archive.hpp>
#include <streams++/producers/directory.hpp>
#include <streams++/producers/vector.hpp>
#include <streams++/streams.hpp>
#include <streams++/transformers/directory.hpp>

using namespace streams;

class Transform : public PackageTransformer {

public:
  bool transform(Package &p) const override {
    // auto out = read_all(p.content());
    printf("pack %s\n", p.path().str().c_str());
    // std::cout << "PACK " << p.path().str() << std::endl;
    return true;
  }
};

int main() {

  std::vector<Package> packages;

  packages.emplace_back(
      Package::create("test", read_file("../CMakeLists.txt")));

  packages.emplace_back(
      Package::create("test2", read_file("../CMakeLists.txt")));
  packages.emplace_back(
      Package::create("test3", read_file("../CMakeLists.txt")));
  packages.emplace_back(
      Package::create("test4", read_file("../CMakeLists.txt")));

  packages.emplace_back(Package::create("http/response", http::get()));

  auto v = new producers::VectorProducer(std::move(packages));

  Chain chain;
  chain.set_producer<producers::GlobProducer>("../src/**/*.cpp")
      .set_producer<producers::GlobProducer>("../src/{,**/}*.cpp")
      //.set_producer(std::unique_ptr<producers::VectorProducer>(v))
      .add<Transform>();
  //.add<transformers::DirectoryDestination>("output");

  chain.run();

  // Archive a("rapper.tar.gz");

  // for (auto &entry : a) {
  //   std::cout << entry.path << std::endl;
  // }

  // {
  //   auto file = a.create_file("test.txt");

  //   file->write((unsigned char *)"Hello", 6);
  //   file->write(NULL, 0);
  // }

  // std::cout << "write " << std::endl;
  // auto stream = a.read_file("build/CMakeCache.txt");
  // std::cout << *stream << std::endl;
  // WritableFileStream out("test_out.txt");
  // ReadableFileStream in("../src/utils.cpp");
  // StringStream out;

  // pipe(in, out);
  // out.seek(0, std::ios::beg);
  // std::cout << "out " << out << std::endl;
  // return 0;

  // streams::HttpReadableStream stream;
  // streams::WritableFileStream file("test.html");
  // streams::pipe(stream, file);
}
