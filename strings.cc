#include "strings.h"

#include <iterator>
#include <sstream>

namespace strings {

std::vector<std::string> split(const std::string& s, const char delim) {
  std::stringstream ss(s);
  std::vector<std::string> items;
  std::string item;
  while (std::getline(ss, item, delim)) {
    items.push_back(item);
  }
  return items;
}

std::string join(const std::vector<std::string>& v, const std::string& delim) {
  std::ostringstream os;
  std::copy(v.begin(), v.end()-1, std::ostream_iterator<std::string>(os, " "));
  os << *(v.rbegin());
  return os.str();
}

}  // end namespace strings
