#ifndef LOGGER_TEST_H
#define LOGGER_TEST_H

#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include <filesystem>
#include <fstream>
#include <sstream>

using std::endl;
using std::ifstream;
using std::stringstream;

void test_logger_set_level();
void test_logger_set_output();
void test_logger_set_output_path();
void test_logger_get_full_output_path();
void test_logger_set_default_values();
void test_logger_get_ostream();
void test_logger_output_log();
void test_logger_debug();
void test_logger_info();
void test_logger_warning();
void test_logger_error();
void logger_test();

#endif
