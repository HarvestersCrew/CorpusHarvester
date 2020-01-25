#include "utils/utils.h"
#include <algorithm>
#include <random>

std::string random_str(const unsigned int len) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist('a', 'z');
  std::string res;
  std::generate_n(std::back_inserter(res), len, [&] { return dist(mt); });
  return res;
}

nlohmann::json json_from_file(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open file: " + path);
  nlohmann::json res;
  in >> res;
  return res;
}
