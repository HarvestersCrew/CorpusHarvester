#include <iostream>
#include "indexation/sqlite/search_builder.h"
#include "indexation/sqlite/sqlite_util.h"

using std::cout;
using std::endl;

SearchBuilder::SearchBuilder() :
    _statement(STATEMENT_HEAD),
    _prepared_values() {}

SearchBuilder *SearchBuilder::fileTagEquals(string tag_name, string tag_value) {
    _statement += " (t.name = ? AND t.value = ?)";
    _prepared_values.push_back(tag_name);
    _prepared_values.push_back(tag_value);
    return this;
}

SearchBuilder *SearchBuilder::fileColumnEquals(string column_name, string column_value) {
    _statement += " (f." + column_name + " = ?)";
    _prepared_values.push_back(column_value);
    return this;
}

SearchBuilder *SearchBuilder::sqlAnd() {
    _statement += " AND";
    return this;
}

SearchBuilder *SearchBuilder::sqlOr() {
    _statement += " OR";
    return this;
}

int SearchBuilder::build(sqlite3 *db, sqlite3_stmt **stmt) {
    int error;
    int code;
    _statement += ");";
    cout << _statement << endl;
    code = sqlite3_prepare(db, _statement.c_str(), _statement.size() + 1, stmt, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_prepare()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    int i = 1;
    for (auto &value : _prepared_values) {
        code = sqlite3_bind_text(*stmt, i, value.c_str(), -1, 0);
        error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text()", sqlite3_errmsg(db));
        if (error) {
            return error;
        }
        i++;
    }
    clear();
    return error;
}

void SearchBuilder::clear() {
    _prepared_values.clear();
    _statement = STATEMENT_HEAD;
}


