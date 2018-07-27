#pragma once
#include <ostream>
#include <streams++/types.hpp>

namespace streams {

void pipe(const ReadableStream &source, WritableStream &dest);
void read_all(const ReadableStream &source, std::string &out);
std::string read_all(const ReadableStream &source);
std::ostream &operator<<(std::ostream &, const ReadableStream &);

namespace utils {
bool replace(std::string &, const std::string &, const std::string &);
bool replace_all(std::string &str, const std::string &from,
                 const std::string &to);
} // namespace utils

} // namespace streams