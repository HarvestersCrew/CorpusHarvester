#ifndef SQLITE_UTIL_H
#define SQLITE_UTIL_H

int manage_errors(const int error, const int expected_code, const char *error_label, const char *error_message);

#endif