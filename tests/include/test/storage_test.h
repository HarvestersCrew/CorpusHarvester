#include <iostream>
#include <sys/stat.h>

#include "indexation/file.h"
#include "storage/storage.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

#define INIT_DATA "/project/engine/script/init_storage_data 100"

using std::cerr;
using std::cout;
using std::endl;

void test_file_destination();
void test_move_one_file();
void test_store_one_file();
void storage_test();
