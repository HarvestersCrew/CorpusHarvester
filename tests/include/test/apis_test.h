#ifndef APIS_TEST_H
#define APIS_TEST_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "test/assertion.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

using std::ofstream;
using std::shared_ptr;
using std::stringstream;
using std::vector;

#define APIS_TEST_CURRENT_APIS_NUMBER 4

void test_apis_get_apis();
void test_apis_get_names();
void apis_test();

#endif
