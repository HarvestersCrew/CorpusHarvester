#ifndef DATABASE_TEST_H
#define DATABASE_TEST_H

#include "database/harvester_database.h"
#include "test/assertion.h"
#include "utils/exceptions.h"

void test_close();
void test_open();
void test_drop_create_empty();
void test_closed_db_exceptions();
void database_test();

#endif
