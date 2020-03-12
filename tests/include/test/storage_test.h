#ifndef STORAGE_TEST_H
#define STORAGE_TEST_H

#include "indexation/file.h"
#include "indexation/indexer.h"
#include "storage/storage.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"
#include <iostream>

#define INIT_DATA "/project/engine/script/init_storage_data 100"

void test_file_destination();
void test_move_one_file();
void test_store_one_file();
std::pair<int, int> storage_test();

#endif
