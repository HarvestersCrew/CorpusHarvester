#include "database/pool_db.h"

queue<shared_ptr<Connection>> PoolDB::_available_pool;
set<shared_ptr<Connection>> PoolDB::_borrowed_pool;
mutex PoolDB::_pool_mut;

Connection *PoolDB::get_connection() {
  sql::Driver *driver = get_driver_instance();
  Connection *con = driver->connect("db", "root", "1234");
  sql::Statement *stmt = con->createStatement();
  stmt->execute("CREATE DATABASE IF NOT EXISTS " HARVESTER_DATABASE_NAME ";");
  delete stmt;
  con->setSchema(HARVESTER_DATABASE_NAME);
  return con;
}

void PoolDB::create() {
  if (PoolDB::empty()) {
    auto con = PoolDB::borrow_from_pool();
    sql::Statement *stmt = con->createStatement();
    std::string create_statements[] = {
        CORPUS_CREATE_STATEMENT, FILE_CREATE_STATEMENT, TAG_CREATE_STATEMENT,
        CORPUS_FILES_CREATE_STATEMENT, SETTING_CREATE_STATEMENT};
    for (auto &create_statement : create_statements) {
      stmt->execute(create_statement);
    }
    logger::debug("Create tables : OK");

    Setting::init_settings(con.get());
    logger::debug("Init Setting tables : OK");
    delete stmt;
    PoolDB::unborrow_from_pool(con);
  }
}

bool PoolDB::empty() {
  auto con = PoolDB::borrow_from_pool();
  sql::Statement *stmt = con->createStatement();
  bool res = stmt->executeQuery("SHOW TABLES")->next() == 0;
  delete stmt;
  PoolDB::unborrow_from_pool(con);
  return res;
}

void PoolDB::init(unsigned int nbr) {
  PoolDB::open_pool(nbr);
  PoolDB::create();
  logger::start();
}

void PoolDB::open_pool(unsigned int nbr) {
  PoolDB::close_pool();
  lock_guard<mutex> lock(_pool_mut);
  for (unsigned int i = 0; i < nbr; ++i) {
    _available_pool.emplace(PoolDB::get_connection());
  }
}

void PoolDB::close_pool() {
  lock_guard<mutex> lock(_pool_mut);
  logger::stop();
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
    throw db_no_free_connection(_borrowed_pool.size());
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

void PoolDB::drop() {
  auto con = PoolDB::borrow_from_pool();
  logger::stop();
  sql::Statement *stmt = con->createStatement();
  std::string drop_statements[] = {DROP_CORPUS_FILES_STATEMENT,
                                   DROP_CORPUS_STATEMENT, DROP_TAG_STATEMENT,
                                   DROP_FILE_STATEMENT, DROP_SETTING_STATEMENT};
  for (auto &drop_statement : drop_statements) {
    stmt->execute(drop_statement);
  }
  delete stmt;
  PoolDB::unborrow_from_pool(con);
  logger::debug("Drop tables : OK");
}
