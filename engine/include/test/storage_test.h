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

void testFileDestination();
void testMoveOneFile();
void testStoreOneFile();
void storage_test();
