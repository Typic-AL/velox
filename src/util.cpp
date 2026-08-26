#include "velox/util.h"

#include <iomanip>
#include <sstream>

namespace vl {

std::string floatToStr(float value) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6) << value; // Up to 6 decimals
  std::string str = oss.str();

  // Trim trailing zeros
  str.erase(str.find_last_not_of('0') + 1, std::string::npos);

  // If it ends with '.', remove that too
  if (!str.empty() && str.back() == '.')
    str.pop_back();

  return str;
}

} // namespace vl
