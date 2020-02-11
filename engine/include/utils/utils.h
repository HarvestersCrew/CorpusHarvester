#ifndef UTILS_H
#define UTILS_H

#include "utils/nlohmann/json.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>

#include "indexation/file.h"

std::string exec(string cmd);

// Returns a random string of length len from a-z characters.
string random_str(const unsigned int len);

void fill_file_randomly(File *file, bool tweet, bool is_even);

void print_sql_exception(sql::SQLException &e);

void print_if_verbose(string to_print, bool verbose);

nlohmann::json json_from_file(const std::string &path);

std::string get_current_time(const char *patern);

#endif
