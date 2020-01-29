#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <sqlite3.h>
#include "database_item.h"

using std::string;

class Indexer {

    string _db_name;

private:

    int openDatabase(sqlite3 **db);

    int insertDatabaseItem(sqlite3 *db, DatabaseItem *item) const;

    int addFile(sqlite3 *db, string name) const;

    int getFilesFromTag(sqlite3 *db, string tag_name, string tag_value);


public:

    Indexer(string db_name);

    int create_database(bool drop_table);

    int indexation();
    
    int searchFromTag(string tag_name, string tag_value);

};

#endif