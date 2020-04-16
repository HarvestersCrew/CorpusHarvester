#include "test/database_test.h"

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
  PoolDB::init(2);
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
  PoolDB::init(2);

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

void test_drop_create_empty_pool() {
  PoolDB::init(1);
  PoolDB::drop();
  Assertion::assert_true(__FUNCTION__, PoolDB::empty());
  PoolDB::create();
  Assertion::assert_false(__FUNCTION__, PoolDB::empty());
}

void database_test() {
  std::cout << std::endl << "Database tests : " << std::endl;

  PoolDB::open_pool(1);
  PoolDB::drop();
  PoolDB::close_pool();

  Assertion::test(test_open_pool, "test_open_pool");
  Assertion::test(test_close_pool, "test_close_pool");
  Assertion::test(test_borrow_from_pool, "test_borrow_from_pool");
  Assertion::test(test_reassign_free_borrowed_pool,
                  "test_reassign_free_borrowed_pool");
  Assertion::test(test_drop_create_empty_pool, "test_drop_create_empty_pool");
}
