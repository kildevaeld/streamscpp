#pragma once
#include <streams++/package.hpp>

namespace streams {

namespace internal {
class ChainPrivate;
}

class PackageProducer {

public:
  virtual ~PackageProducer() {}
  virtual bool read(Package &pack) const = 0;
};

class PackageTransformer {

public:
  virtual ~PackageTransformer() {}
  virtual bool transform(Package &pack) const = 0;
};

class Chain {

public:
  Chain();
  ~Chain();

  void run();

  template <typename T, typename... Args> Chain &set_producer(Args... args) {
    PackageProducer *producer = new T(std::forward<Args>(args)...);
    return set_producer(std::unique_ptr<PackageProducer>(producer));
  }

  Chain &set_producer(std::unique_ptr<PackageProducer> &&);
  PackageProducer *producer() const;

  template <typename T, typename... Args> Chain &add(Args... args) {
    PackageTransformer *transformer = new T(std::forward<Args>(args)...);
    return add(std::unique_ptr<PackageTransformer>(transformer));
  }
  Chain &add(std::unique_ptr<PackageTransformer> &&);

private:
  std::unique_ptr<internal::ChainPrivate> d;
};

} // namespace streams