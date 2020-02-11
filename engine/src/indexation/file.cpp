#include <sstream>

#include "mysql_connection.h"
#include <cppconn/prepared_statement.h>

#include "indexation/database_item.h"
#include "indexation/file.h"

using std::cout;
using std::endl;
using std::ostringstream;

File::File(string path, string name, int size, string source, int id)
    : DatabaseItem(id), _path(path), _name(name), _size(size), _source(source) {
}

File::File() : DatabaseItem(0), _path(""), _name(""), _tags() {}

File::~File() {
  for (auto &tag : _tags) {
    tag.reset();
  }
}

string File::to_string() const {
  ostringstream out;
  out << "File{_id=" << _id << ", _path=" << _path << ", _name=" << _name
      << ", _size=" << _size << ", _source=" << _source << ", _tags=[\n\t";
  for (const auto &tag : _tags) {
    out << tag->to_string() << "\n\t";
  }
  out << "]}";
  return out.str();
}

void File::insert(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  list<File *> files;

  prep_stmt = db->prepareStatement(INSERT_FILE_STATEMENT);
  prep_stmt->setString(1, _path);
  prep_stmt->setString(2, _name);
  prep_stmt->setInt(3, _size);
  prep_stmt->setString(4, _source);
  prep_stmt->execute();
  delete prep_stmt;

  this->_id = DatabaseItem::get_last_inserted_id(db);
  for (const auto &tag : _tags) {
    tag->set_file_id(this->_id);
    tag->insert(db);
  }
}

void File::fetch_tags(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement("SELECT * FROM Tag WHERE file_id = ?;");
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    Tag *tag = new Tag();
    tag->fill_from_statement(db, res);
    _tags.push_back(unique_ptr<Tag>(tag));
  }
  delete res;
}

void File::fill_from_statement(sql::Connection *db, sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_path = res->getString("path");
  this->_name = res->getString("name");
  this->_size = res->getInt("size");
  this->_source = res->getString("source");
  fetch_tags(db);
}

void File::add_tag(string name, string value) {
  _tags.push_back(unique_ptr<Tag>(new Tag(name, value)));
}
