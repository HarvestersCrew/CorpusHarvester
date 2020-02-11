#include <iostream>
#include <sys/stat.h>

#include "indexation/file.h"
#include "storage/storage.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

using std::cerr;
using std::cout;
using std::endl;

void test_file_destination();
void test_move_one_file();
void test_store_one_file();
void storage_test();
