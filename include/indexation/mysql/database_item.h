#ifndef DATABASE_ITEM_H
#define DATABASE_ITEM_H

#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class DatabaseItem {

protected:
  int _id;

public:
  DatabaseItem() {}

  DatabaseItem(int id) : _id(id) {}

  virtual ~DatabaseItem() = default;

  virtual std::string toString() const = 0;

  virtual void insert(sql::Connection *db) = 0;

  virtual void fillFromStatement(sql::ResultSet *res) = 0;

  static int getLastInsertedId(sql::Connection *db) {
    sql::Statement *stmt = db->createStatement();
    sql::ResultSet *res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS id");
    res->next();
    return res->getInt("id");
  }

  int getId() const { return _id; };
  void setId(int id) { _id = id; };
};

#endif