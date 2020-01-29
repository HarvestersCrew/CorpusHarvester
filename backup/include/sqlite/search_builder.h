#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <list>
#include <string>
#include <sqlite3.h>

using std::string;
using std::list;

class SearchBuilder {

    string STATEMENT_HEAD = "SELECT f.id, f.path, f.name FROM File f, Tag t WHERE f.id = t.file_id AND (";

    string _statement;
    list<string> _prepared_values;

public:

    SearchBuilder();

    SearchBuilder *fileTagEquals(string tag_name, string tag_value);

    SearchBuilder *fileColumnEquals(string column_name, string column_value);

    SearchBuilder *sqlAnd();

    SearchBuilder *sqlOr();

    int build(sqlite3 *db, sqlite3_stmt **stmt);

    void clear();

};

#endif