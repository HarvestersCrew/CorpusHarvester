#ifndef API_REQUEST_BUILDER_TEST_H
#define API_REQUEST_BUILDER_TEST_H

#include "download/api_download_builder.h"
#include "download/api_loader.h"
#include "test/assertion.h"
#include "utils/api_request_builder.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

using std::cout;
using std::endl;
using std::make_pair;
using std::pair;
using std::string;
using std::unordered_map;

void test_types();
void test_requests();
void test_api_dl_remove_ops();
void api_request_builder_test();

#endif
