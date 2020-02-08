#include "utils/utils.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

std::string exec(string cmd_str) {
  const char *cmd = cmd_str.c_str();
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

std::string random_str(const unsigned int len) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist('a', 'z');
  std::string res;
  std::generate_n(std::back_inserter(res), len, [&] { return dist(mt); });
  return res;
}

void fillFileRandomly(File *file, bool tweet, bool isEven) {
  std::ostringstream isEvenStream;
  isEvenStream << isEven;
  file->addTag("isEven", isEvenStream.str());
  if (tweet) {
    file->addTag("type", "tweet");
  } else {
    file->addTag("type", "article");
  }
  int tweet_subject_length = 3;
  string tweet_subject_values[tweet_subject_length] = {"tank", "kitty",
                                                       "crisquare"};
  for (int i = 0; i < tweet_subject_length; ++i) {
    file->addTag("subject", tweet_subject_values[i]);
  }
}

void printIfVerbose(string toPrint, bool verbose) {
  if (verbose) {
    std::cout << toPrint << std::endl;
  }
}

void printSQLException(sql::SQLException &e) {
  std::cout << "# ERR: " << e.what();
  std::cout << " (MySQL error code: " << e.getErrorCode();
  std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

nlohmann::json json_from_file(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open file: " + path);
  nlohmann::json res;
  in >> res;
  return res;
}

std::string get_current_time(const char *patern) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, patern);
  return oss.str();
}
