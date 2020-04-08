#ifndef API_PARAMETER_TEST_H
#define API_PARAMETER_TEST_H

#include "download/api_parameter.h"
#include "test/assertion.h"
#include "utils/nlohmann/json.hpp"

using nlohmann::json;

void test_api_parameter_base_construct();
void test_api_paramter_base_values();
void api_parameter_test();

#endif
