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

void discover_from_path_test();
void apis_test();

#endif
