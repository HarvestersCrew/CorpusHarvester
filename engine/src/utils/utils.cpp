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

std::string exec(std::string cmd_str) {
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

std::string vec_to_string(const std::vector<char> &vec) {
  std::string str;
  for (char c : vec) {
    str += c;
  }
  return str;
}

void fill_file_randomly(shared_ptr<File> file, bool tweet, bool is_even) {
  std::ostringstream is_evenStream;
  is_evenStream << is_even;
  file->add_tag("is_even", is_evenStream.str());
  if (tweet) {
    file->add_tag("type", "tweet");
  } else {
    file->add_tag("type", "article");
  }
  int tweet_subject_length = 3;
  std::string tweet_subject_values[tweet_subject_length] = {"tank", "kitty",
                                                            "crisquare"};
  for (int i = 0; i < tweet_subject_length; ++i) {
    file->add_tag("subject", tweet_subject_values[i]);
  }
}

void print_sql_exception(sql::SQLException &e) {
  std::cout << "# ERR: " << e.what();
  std::cout << " (MySQL error code: " << e.getErrorCode();
  std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

void log_sql_exception(sql::SQLException &e) {
  std::ostringstream sql_exception;
  sql_exception << "# ERR: " << e.what();
  sql_exception << " (MySQL error code: " << e.getErrorCode();
  sql_exception << ", SQLState: " << e.getSQLState() << " )";
  logger::error(sql_exception.str());
}

nlohmann::json json_from_file(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open file: " + path);
  nlohmann::json res;
  in >> res;
  in.close();
  return res;
}

std::string get_current_time(const char *patern) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, patern);
  return oss.str();
}

std::string add_string_every_n_chars(std::string input, std::string to_add,
                                     int n) {
  std::string split_string;
  std::string sub_str;
  for (size_t i = 0; i < input.length(); i += n) {
    sub_str = input.substr(i, n);
    split_string += sub_str + to_add;
  }
  return split_string;
}

std::list<string> split(const string &str, char delimiter) {
  std::list<string> elements;
  stringstream ss(str);
  string sub_str;
  while (getline(ss, sub_str, delimiter)) {
    elements.push_back(sub_str);
  }
  return elements;
}

std::string was_or_were(int count) { return count == 1 ? " was" : "s were"; }
