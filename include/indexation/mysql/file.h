#ifndef FILE_H
#define FILE_H

#include <cppconn/connection.h>
#include <cppconn/resultset.h>

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/tag.h"

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

    void insert(sql::Connection *db);

    void fillFromStatement(sql::ResultSet *res);

    void addTag(string name, string value);

    string getPath() const { return _path;}
    string getName() const { return _name;}
    int getSize() const { return _size;}

};

#endif