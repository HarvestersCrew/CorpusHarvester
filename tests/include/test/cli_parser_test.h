#ifndef CLI_PARSER_TEST_H
#define CLI_PARSER_TEST_H

#include "test/assertion.h"
#include "utils/cli_parser.h"
#include "utils/exceptions.h"
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using std::map;
using std::string;
using std::vector;

void test_cli_parser_construction();
void test_cli_parser_is_terminal();
void test_cli_parser_add_command();
void test_cli_parser_add_option();
void test_cli_parser_parser();
void cli_parser_test();

#endif
