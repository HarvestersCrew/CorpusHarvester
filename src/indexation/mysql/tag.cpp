#include <sstream>

#include <cppconn/prepared_statement.h>

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/file.h"

using std::string;
using std::ostringstream;

Tag::Tag(string name, string value, int id, int file_id) : 
    DatabaseItem(id),
    _file_id(file_id),
    _name(name),
    _value(value) {}

Tag::~Tag() {}

string Tag::toString() const {
    ostringstream out;
    out << "Tag{_id=" << _id << ", file_id=" << _file_id << ", _name=" << _name << ", _value=" << _value << "}";
    return out.str();
}

void Tag::insert(sql::Connection *db) {
    sql::PreparedStatement *prep_stmt;
    list<File*> files;
    
    prep_stmt = db->prepareStatement("INSERT INTO Tag (file_id, name, value) VALUES(?, ?, ?)");
    prep_stmt->setInt(1, _file_id);
    prep_stmt->setString(2, _name);
    prep_stmt->setString(3, _value);
    prep_stmt->execute();
    delete prep_stmt;

    this->_id = DatabaseItem::getLastInsertedId(db);
}

void Tag::fillFromStatement(sql::ResultSet *res) {
    this->_id = res->getInt("id");
    this->_file_id = res->getInt("file_id");
    this->_name = res->getString("name");
    this->_value = res->getString("value");
}