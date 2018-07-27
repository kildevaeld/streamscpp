#pragma once
#include <glob.h>
#include <streams++/chain.hpp>
#include <vector>

namespace streams {

namespace producers {

class GlobProducer : public PackageProducer {

public:
  GlobProducer(const std::string &pattern);
  ~GlobProducer();
  bool read(Package &pack) const override;

private:
  std::string m_pattern;
  glob_t m_result;
  mutable unsigned int m_pos = 0;
};

} // namespace producers

} // namespace streams