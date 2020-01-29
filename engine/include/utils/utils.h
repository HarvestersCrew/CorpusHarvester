#ifndef UTILS_H
#define UTILS_H

#include "utils/nlohmann/json.hpp"
#include <fstream>
#include <string>

#include "indexation/file.h"

using std::string;

// Returns a random string of length len from a-z characters.
string random_str(const unsigned int len);

void fillFileRandomly(File *file, bool tweet, bool isEven);

void printSQLException(sql::SQLException &e);

void print(string toPrint, bool verbose);

nlohmann::json json_from_file(const std::string &path);

#endif
