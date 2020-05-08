#ifndef APIS_TEST_H
#define APIS_TEST_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "test/assertion.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

using std::ofstream;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::unordered_map;

#define APIS_TEST_CURRENT_APIS_NUMBER 4

void test_apis_get_apis();
void test_apis_get_names();
void test_apis_get_api();
void test_apis_get_type();
void apis_test();

#endif
