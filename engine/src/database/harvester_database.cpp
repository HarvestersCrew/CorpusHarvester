#include "database/harvester_database.h"

sql::Connection *HarvesterDatabase::_db;
queue<shared_ptr<Connection>> HarvesterDatabase::_available_pool;
set<shared_ptr<Connection>> HarvesterDatabase::_borrowed_pool;
mutex HarvesterDatabase::_pool_mut;

void HarvesterDatabase::open() {
  if (_db == nullptr) {
    sql::Driver *driver = get_driver_instance();
    _db = driver->connect("db", "root", "1234");

    sql::Statement *stmt = _db->createStatement();
    stmt->execute("CREATE DATABASE IF NOT EXISTS " HARVESTER_DATABASE_NAME ";");
    delete stmt;

    _db->setSchema(HARVESTER_DATABASE_NAME);
    logger::debug("Database opened");
  }
}

void HarvesterDatabase::open_pool(unsigned int nbr) {
  HarvesterDatabase::close_pool();
  lock_guard<mutex> lock(_pool_mut);
  for (unsigned int i = 0; i < nbr; ++i) {
    sql::Driver *driver = get_driver_instance();
    _available_pool.emplace(driver->connect("db", "root", "1234"));
  }
}

void HarvesterDatabase::drop() {
  if (_db == nullptr) {
    throw ClosedDatabaseException();
  }
  logger::stop();
  sql::Statement *stmt = _db->createStatement();
  std::string drop_statements[] = {DROP_CORPUS_FILES_STATEMENT,
                                   DROP_CORPUS_STATEMENT, DROP_TAG_STATEMENT,
                                   DROP_FILE_STATEMENT, DROP_SETTING_STATEMENT};
  for (auto &drop_statement : drop_statements) {
    stmt->execute(drop_statement);
  }
  delete stmt;
  logger::debug("Drop tables : OK");
}

void HarvesterDatabase::create() {
  if (_db == nullptr) {
    throw ClosedDatabaseException();
  }
  if (HarvesterDatabase::empty()) {
    sql::Statement *stmt = _db->createStatement();
    std::string create_statements[] = {
        CORPUS_CREATE_STATEMENT, FILE_CREATE_STATEMENT, TAG_CREATE_STATEMENT,
        CORPUS_FILES_CREATE_STATEMENT, SETTING_CREATE_STATEMENT};
    for (auto &create_statement : create_statements) {
      stmt->execute(create_statement);
    }
    logger::debug("Create tables : OK");

    Setting::init_settings(_db);
    logger::debug("Init Setting tables : OK");
    delete stmt;
  }
}

bool HarvesterDatabase::empty() {
  if (_db == nullptr) {
    throw ClosedDatabaseException();
  }
  sql::Statement *stmt = _db->createStatement();
  bool res = stmt->executeQuery("SHOW TABLES")->next() == 0;
  delete stmt;
  return res;
}

sql::Connection *HarvesterDatabase::init() {
  HarvesterDatabase::open();
  HarvesterDatabase::create();
  logger::start();
  return _db;
}

void HarvesterDatabase::close() {
  logger::stop();
  if (_db != nullptr) {
    _db->close();
    logger::debug("Database closed");
    delete _db;
    _db = nullptr;
  } else {
    logger::debug("Database is already closed");
  }
}

void HarvesterDatabase::close_pool() {
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

shared_ptr<Connection> HarvesterDatabase::borrow_from_pool() {
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

void HarvesterDatabase::unborrow_from_pool(shared_ptr<Connection> &ptr) {
  lock_guard<mutex> lock(_pool_mut);
  if (_borrowed_pool.find(ptr) != _borrowed_pool.end()) {
    _borrowed_pool.erase(ptr);
    _available_pool.push(ptr);
    ptr.reset();
  }
}
