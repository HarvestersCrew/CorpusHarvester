#ifndef UTILS_H
#define UTILS_H

#include "indexation/file.h"
#include "utils/nlohmann/json.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;

std::string exec(std::string cmd);

// Returns a random string of length len from a-z characters.
std::string random_str(const unsigned int len);

std::string vec_to_string(const std::vector<char> &vec);

void fill_file_randomly(shared_ptr<File> file, bool tweet, bool is_even);

void print_sql_exception(sql::SQLException &e);

void print_if_verbose(std::string to_print, bool verbose);

nlohmann::json json_from_file(const std::string &path);

std::string get_current_time(const char *patern);

#endif
