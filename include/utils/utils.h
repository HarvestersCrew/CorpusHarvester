#ifndef UTILS_H
#define UTILS_H

#include <string>

using std::string;

// Returns a random string of length len from a-z characters.
string random_str(const unsigned int len);

void print(string toPrint, bool verbose);

#endif
