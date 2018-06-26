#pragma once
#include <ostream>
#include <streams++/types.hpp>

namespace streams {

void pipe(const ReadableStream &source, WritableStream &dest);
void read_all(ReadableStream &source, std::string &out);
std::string read_all(ReadableStream &source);
std::ostream &operator<<(std::ostream &, const ReadableStream &);
} // namespace streams