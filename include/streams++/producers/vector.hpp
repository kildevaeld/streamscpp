#pragma once
#include <streams++/chain.hpp>
#include <vector>

namespace streams {

namespace producers {

class VectorProducer : public PackageProducer {

public:
  VectorProducer(std::vector<Package> &&);
  ~VectorProducer();
  bool read(Package &pack) const override;

private:
  mutable std::vector<Package> m_packages;
};

} // namespace producers

} // namespace streams