#include "database/database_item.h"
// This include below is here to avoid circular inclusions, do not put it back
// in the header file
#include "database/pool_db.h"

int DatabaseItem::get_last_inserted_id(shared_ptr<sql::Connection> db) {
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS id");
  res->next();
  int to_return = res->getInt("id");
  delete stmt;
  delete res;
  return to_return;
}
