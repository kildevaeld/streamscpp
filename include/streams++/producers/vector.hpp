#pragma once
#include <streams++/chain.hpp>

namespace streams {

namespace producers {

class VectorProducer : public PackageProducer {

public:
  VectorProducer(std::vector<Package> &&);
  bool read(Package &pack) const override;
};

} // namespace producers

} // namespace streams