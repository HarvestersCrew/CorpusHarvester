#include "indexation/tag.h"
// Put here to avoid circular inclusion, do not put in header
#include "database/pool_db.h"
#include "indexation/file.h"

Tag::Tag() {}

Tag::Tag(std::string name, std::string value, int id, int file_id)
    : DatabaseItem(id), _file_id(file_id), _name(name), _value(value) {}

Tag::~Tag() {}

std::string Tag::to_string() const {
  std::ostringstream out;
  out << "Tag{_id=" << _id << ", file_id=" << _file_id << ", _name=" << _name
      << ", _value=" << _value << "}";
  return out.str();
}

bool Tag::insert() {
  sql::PreparedStatement *prep_stmt;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt = con->prepareStatement(INSERT_TAG_STATEMENT);
  prep_stmt->setInt(1, _file_id);
  prep_stmt->setString(2, _name);
  prep_stmt->setString(3, _value);
  prep_stmt->execute();
  delete prep_stmt;

  this->_id = DatabaseItem::get_last_inserted_id(con);
  PoolDB::unborrow_from_pool(con);
  return true;
}

void Tag::fill_from_statement(sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_file_id = res->getInt("file_id");
  this->_name = res->getString("name");
  this->_value = res->getString("value");
}
