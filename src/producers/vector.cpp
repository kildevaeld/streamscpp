#include <streams++/producers/vector.hpp>

namespace streams {
namespace producers {

VectorProducer::VectorProducer(std::vector<Package> &&p)
    : m_packages(std::move(p)) {}

VectorProducer::~VectorProducer() {}

bool VectorProducer::read(Package &pack) const {
  if (m_packages.size() == 0)
    return false;

  pack = std::move(m_packages[m_packages.size() - 1]);
  m_packages.pop_back();

  return true;
}

} // namespace producers
} // namespace streams