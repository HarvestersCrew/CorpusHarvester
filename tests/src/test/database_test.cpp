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

void test_open_pool() {
  PoolDB::open_pool(5);
  Assertion::assert_equals(__FUNCTION__, 5, PoolDB::_available_pool.size());
  Assertion::assert_equals(__FUNCTION__, 0, PoolDB::_borrowed_pool.size());
}

void test_close_pool() {
  PoolDB::close_pool();
  Assertion::assert_equals(__FUNCTION__, 0, PoolDB::_available_pool.size());
  Assertion::assert_equals(__FUNCTION__, 0, PoolDB::_borrowed_pool.size());
}

void test_borrow_from_pool() {
  PoolDB::open_pool(2);
  auto ptr1 = PoolDB::borrow_from_pool();
  auto ptr2 = PoolDB::borrow_from_pool();
  Assertion::assert_equals(__FUNCTION__, 2, ptr1.use_count());
  Assertion::assert_equals(__FUNCTION__, 2, ptr2.use_count());
  try {
    PoolDB::borrow_from_pool();
    Assertion::assert_throw(__FUNCTION__, "db_no_free_connection");
  } catch (const db_no_free_connection &e) {
  }
  PoolDB::unborrow_from_pool(ptr1);
  auto ptr3 = PoolDB::borrow_from_pool();
  Assertion::assert_equals(__FUNCTION__, 2, ptr3.use_count());
  Assertion::assert_equals(__FUNCTION__, 0, ptr1.use_count());
}

void test_reassign_free_borrowed_pool() {
  PoolDB::open_pool(2);

  PoolDB::borrow_from_pool();
  Assertion::assert_equals(__FUNCTION__, 1, PoolDB::_available_pool.size());
  Assertion::assert_equals(__FUNCTION__, 1, PoolDB::_borrowed_pool.size());

  PoolDB::borrow_from_pool();
  Assertion::assert_equals(__FUNCTION__, 0, PoolDB::_available_pool.size());
  Assertion::assert_equals(__FUNCTION__, 2, PoolDB::_borrowed_pool.size());

  PoolDB::borrow_from_pool();
  Assertion::assert_equals(__FUNCTION__, 1, PoolDB::_available_pool.size());
  Assertion::assert_equals(__FUNCTION__, 1, PoolDB::_borrowed_pool.size());
}

void database_test() {
  std::cout << std::endl << "Database tests : " << std::endl;
  Assertion::test(test_close, "test_close");
  Assertion::test(test_open, "test_open");
  Assertion::test(test_drop_create_empty, "test_drop_create_empty");
  Assertion::test(test_closed_db_exceptions, "test_closed_db_exceptions");
  Assertion::test(test_open_pool, "test_open_pool");
  Assertion::test(test_close_pool, "test_close_pool");
  Assertion::test(test_borrow_from_pool, "test_borrow_from_pool");
  Assertion::test(test_reassign_free_borrowed_pool,
                  "test_reassign_free_borrowed_pool");
}
