#include <sstream>
#include "indexation/sqlite/database_item.h"
#include "indexation/sqlite/file.h"
#include "indexation/sqlite/sqlite_util.h"

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

int Tag::insert(sqlite3 *db) {
    int error;
    int code;
    sqlite3_stmt *stmt = nullptr;
    char const *insert_tag = "INSERT INTO Tag (file_id, name, value) VALUES(?, ?, ?)";
    code = sqlite3_prepare_v2(db, insert_tag, -1, &stmt, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_prepare_v2()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_int(stmt, 1, _file_id);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°1", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 2, _name.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°2", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 3, _value.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°3", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_step(stmt);
    error = manage_errors(code, SQLITE_DONE, "When executing sqlite3_step()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    this->_id = sqlite3_last_insert_rowid(db);
    return 0;
}

void Tag::fillFromStatement(sqlite3_stmt *stmt) {
    this->_id = sqlite3_column_int(stmt, 0);
    this->_file_id = sqlite3_column_int(stmt, 1);
    this->_name = columnReturnToString(sqlite3_column_text(stmt, 2));
    this->_value = columnReturnToString(sqlite3_column_text(stmt, 3));
}