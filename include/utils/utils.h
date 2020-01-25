#ifndef UTILS_H
#define UTILS_H

#include "utils/nlohmann/json.hpp"
#include <fstream>
#include <string>

// Returns a random string of length len from a-z characters.
std::string random_str(const unsigned int len);

nlohmann::json json_from_file(const std::string &path);

#endif
