#include <sstream>

#include <cppconn/prepared_statement.h>

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/file.h"

using std::ostringstream;

File::File(string path, string name, int size, int id) :
    DatabaseItem(id),
    _path(path),
    _name(name),
    _tags() {}

File::File() :
    DatabaseItem(0),
    _path(""),
    _name(""),
    _tags() {}

File::~File() {
    for (auto &tag : _tags) {
        tag.reset();
    }
}

string File::toString() const {
    ostringstream out;
    out << "File{_id=" << _id << ", _path=" << _path << ", _name=" << _name << ", _tags=[";
    for (const auto& tag: _tags) { 
        out << tag->toString();
    }
    out << "]}";
    return out.str();
}

void File::insert(sql::Connection *db) {
    sql::PreparedStatement *prep_stmt;
    list<File*> files;
    
    prep_stmt = db->prepareStatement("INSERT INTO File (path, name) VALUES(?, ?)");
    prep_stmt->setString(1, _path);
    prep_stmt->setString(2, _name);
    prep_stmt->execute();
    delete prep_stmt;
    
    this->_id = DatabaseItem::getLastInsertedId(db);
    for (const auto& tag :_tags) {
        tag->setFileId(this->_id);
        tag->insert(db);
    }
}

void File::fillFromStatement(sql::ResultSet *res) {
    this->_id = res->getInt("id");
    this->_path = res->getString("path");
    this->_name = res->getString("name");
}

void File::addTag(string name, string value) {
    _tags.push_back(unique_ptr<Tag>(new Tag(name, value)));
}
