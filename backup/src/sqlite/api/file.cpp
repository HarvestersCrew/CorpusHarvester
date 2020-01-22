#include <sstream>
#include <memory>
#include "indexation/sqlite/database_item.h"
#include "indexation/sqlite/file.h"
#include "indexation/sqlite/sqlite_util.h"

using std::string;
using std::ostringstream;
using std::unique_ptr;

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

File::~File() {}

string File::toString() const {
    ostringstream out;
    out << "File{_id=" << _id << ", _path=" << _path << ", _name=" << _name << ", _tags=[";
    for (const auto& tag: _tags) { 
        out << tag->toString();
    }
    out << "]}";
    return out.str();
}

int File::insert(sqlite3 *db) {
    int error;
    int code;
    sqlite3_stmt *stmt = nullptr;
    const char *insert_file = "INSERT INTO File (path, name) VALUES(?, ?)";
    code = sqlite3_prepare_v2(db, insert_file, -1, &stmt, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_prepare_v2()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 1, _path.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°1", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 2, _name.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°2", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_step(stmt);
    error = manage_errors(code, SQLITE_DONE, "When executing sqlite3_step()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }
    
    this->_id = sqlite3_last_insert_rowid(db);

    for (const auto& tag :_tags) {
        tag->setFileId(this->_id);
        error = tag->insert(db);
        if (error) {
            return error;
        }
    }
    return 0;
}

void File::fillFromStatement(sqlite3_stmt *stmt) {
    this->_id = sqlite3_column_int(stmt, 0);
    this->_path = this->columnReturnToString(sqlite3_column_text(stmt, 1));
    this->_name = this->columnReturnToString(sqlite3_column_text(stmt, 2));
}

void File::addTag(string name, string value) {
    _tags.push_back(unique_ptr<Tag>(new Tag(name, value)));
}
