#ifndef FILE_H
#define FILE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <sqlite3.h>
#include <list>
#include "database_item.h"
#include "tag.h"

#define FILE_CREATE_STATEMENT "CREATE TABLE IF NOT EXISTS File(id INTEGER NOT NULL,path TEXT NOT NULL,name TEXT NOT NULL,PRIMARY KEY (id));"
#define GET_FILES_FROM_TAG "SELECT * FROM File f, Tag t WHERE f.id = t.file_id AND t.name = ? AND t.value = ?;"


using std::string;
using std::list;
using std::unique_ptr;

class File : public DatabaseItem {
	
    string _path;
    string _name;
    int _size;
    list<unique_ptr<Tag>> _tags;

public:
	File(string path, string name, int size, int id = -1);

    File();

    ~File();

    string toString() const;

    int insert(sqlite3 *db);

    void fillFromStatement(sqlite3_stmt *stmt);

    void addTag(string name, string value);

    string path() const { return _path;}
    string name() const { return _name;}
    int size() const { return _size;}

};

#endif