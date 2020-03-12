#include "database/harvester_database.h"

sql::Connection *HarvesterDatabase::_db;

void HarvesterDatabase::open() {
  if (_db == nullptr) {
    sql::Driver *driver = get_driver_instance();
    _db = driver->connect("db", "root", "1234");
    _db->setSchema("harvester");
  } else {
    logger::debug("Database is already opened");
  }
}

void HarvesterDatabase::drop() {
  if (_db == nullptr) {
    throw ClosedDatabaseException();
  }
  sql::Statement *stmt = _db->createStatement();
  std::string drop_statements[] = {DROP_CORPUS_FILES_STATEMENT,
                                   DROP_CORPUS_STATEMENT, DROP_TAG_STATEMENT,
                                   DROP_FILE_STATEMENT, DROP_SETTING_STATEMENT};
  for (auto &drop_statement : drop_statements) {
    stmt->execute(drop_statement);
  }
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
  return stmt->executeQuery("SHOW TABLES")->next() == 0;
}

sql::Connection *HarvesterDatabase::init() {
  HarvesterDatabase::open();
  HarvesterDatabase::create();
  return _db;
}

void HarvesterDatabase::close() {
  if (_db != nullptr) {
    _db->close();
    delete _db;
    _db = nullptr;
  } else {
    logger::debug("Database is already closed");
  }
}