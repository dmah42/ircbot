#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <string>
#include <vector>

namespace strings {

std::vector<std::string> split(const std::string& s, char delim);
std::string join(const std::vector<std::string>& s, const std::string& delim);

}  // end namespace strings

#endif  // _STRINGS_H_
