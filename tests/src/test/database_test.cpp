#include "test/database_test.h"

void test_close() {
  HarvesterDatabase::close();
  Assertion::assert_nullptr(__FUNCTION__, HarvesterDatabase::get());
}

void test_open() {
  HarvesterDatabase::open();
  Assertion::assert_not_nullptr(__FUNCTION__, HarvesterDatabase::get());
}

void test_drop_create_empty() {
  HarvesterDatabase::drop();
  Assertion::assert_true(__FUNCTION__, HarvesterDatabase::empty());
  HarvesterDatabase::create();
  Assertion::assert_false(__FUNCTION__, HarvesterDatabase::empty());
}

void test_closed_db_exceptions() {
  HarvesterDatabase::close();
  try {
    HarvesterDatabase::drop();
    Assertion::assert_throw(__FUNCTION__, "ClosedDatabaseException");
  } catch (ClosedDatabaseException &e) {
  }
  try {
    HarvesterDatabase::create();
    Assertion::assert_throw(__FUNCTION__, "ClosedDatabaseException");
  } catch (ClosedDatabaseException &e) {
  }
  try {
    HarvesterDatabase::empty();
    Assertion::assert_throw(__FUNCTION__, "ClosedDatabaseException");
  } catch (ClosedDatabaseException &e) {
  }
}

std::pair<int, int> database_test() {
  int function_count = 4;
  void (*test_functions[])(void) = {
      test_close, test_open, test_drop_create_empty, test_closed_db_exceptions};
  std::string test_functions_name[] = {"test_close", "test_open",
                                       "test_drop_create_empty",
                                       "test_closed_db_exceptions"};

  std::cout << std::endl << "Database tests : " << std::endl;
  return Assertion::test_all(test_functions, test_functions_name,
                             function_count);
}
