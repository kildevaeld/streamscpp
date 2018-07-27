#pragma once
#include <streams++/chain.hpp>
#include <string>

namespace streams {

namespace transformers {

class DirectoryDestination : public PackageTransformer {

public:
  DirectoryDestination(const std::string &path);
  bool transform(Package &package) const override;

private:
  std::string m_path;
};

} // namespace transformers

} // namespace streams