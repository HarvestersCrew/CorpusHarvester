#include "database/pool_db.h"

queue<shared_ptr<Connection>> PoolDB::_available_pool;
set<shared_ptr<Connection>> PoolDB::_borrowed_pool;
mutex PoolDB::_pool_mut;

void PoolDB::open_pool(unsigned int nbr) {
  PoolDB::close_pool();
  lock_guard<mutex> lock(_pool_mut);
  for (unsigned int i = 0; i < nbr; ++i) {
    sql::Driver *driver = get_driver_instance();
    _available_pool.emplace(driver->connect("db", "root", "1234"));
  }
}

void PoolDB::close_pool() {
  lock_guard<mutex> lock(_pool_mut);
  while (_available_pool.size() != 0) {
    auto ptr = _available_pool.front();
    _available_pool.pop();
    if (ptr != nullptr) {
      ptr->close();
    }
  }
  for (auto ptr : _borrowed_pool) {
    if (ptr != nullptr) {
      ptr->close();
    }
  }
  _borrowed_pool.clear();
  logger::debug("DB pool closed");
}

shared_ptr<Connection> PoolDB::borrow_from_pool() {
  lock_guard<mutex> lock(_pool_mut);

  if (_available_pool.size() == 0) {
    for (auto it = _borrowed_pool.begin(); it != _borrowed_pool.end();) {
      if (it->use_count() == 1) {
        _available_pool.push(*it);
        it = _borrowed_pool.erase(it);
      } else {
        ++it;
      }
    }
  }

  if (_available_pool.size() > 0) {
    auto ptr = _available_pool.front();
    _available_pool.pop();
    _borrowed_pool.insert(ptr);
    return ptr;
  } else {
    throw db_no_free_connection();
  }
}

void PoolDB::unborrow_from_pool(shared_ptr<Connection> &ptr) {
  lock_guard<mutex> lock(_pool_mut);
  if (_borrowed_pool.find(ptr) != _borrowed_pool.end()) {
    _borrowed_pool.erase(ptr);
    _available_pool.push(ptr);
    ptr.reset();
  }
}
