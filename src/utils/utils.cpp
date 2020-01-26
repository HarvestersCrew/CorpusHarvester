#include <algorithm>
#include <iostream>
#include <random>
#include <utils/utils.h>

std::string random_str(const unsigned int len) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist('a', 'z');
  std::string res;
  std::generate_n(std::back_inserter(res), len, [&] { return dist(mt); });
  return res;
}

void print(string toPrint, bool verbose) {
  if (verbose) {
    std::cout << toPrint << std::endl;
  }
}
