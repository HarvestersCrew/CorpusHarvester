#ifndef TAG_H
#define TAG_H

#include <algorithm>
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "database_item.h"

#define TAG_CREATE_STATEMENT "CREATE TABLE IF NOT EXISTS Tag(id INTEGER,file_id INTEGER NOT NULL,name TEXT NOT NULL,value TEXT NOT NULL,PRIMARY KEY (id),FOREIGN KEY (file_id) REFERENCES File(id));"

using std::string;

class Tag : public DatabaseItem {
	
    int _file_id;
    string _name;
    string _value;

public:
	Tag(string name, string value, int id = -1, int file_id = -1);

    ~Tag();

    string toString() const;

    int insert(sqlite3 *db);

    void fillFromStatement(sqlite3_stmt *stmt);

    int getFileId() const { return _file_id; }
    string getName() const { return _name; }
    string getValue() const { return _value; }

    void setFileId(int file_id) { _file_id = file_id; }
};

#endif