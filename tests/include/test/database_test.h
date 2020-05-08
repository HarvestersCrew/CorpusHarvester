#ifndef DATABASE_TEST_H
#define DATABASE_TEST_H

#include "database/pool_db.h"
#include "test/assertion.h"
#include "utils/exceptions.h"

void test_open_pool();
void test_close_pool();
void test_borrow_from_pool();
void test_reassign_free_borrowed_pool();
void test_drop_create_empty_pool();
void database_test();

#endif
