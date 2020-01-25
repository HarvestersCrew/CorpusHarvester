#include <sstream>
#include <iostream>

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#include "indexation/mysql/indexer.h"
#include "indexation/mysql/database_item.h"
#include "indexation/mysql/file.h"

using std::cout;
using std::endl;
using std::list;

Indexer::Indexer(string db_name) : 
    _db_name(db_name) {}

void Indexer::openDatabase() {
    if (db == nullptr) {
        sql::Driver *driver = get_driver_instance();
        db = driver->connect("TO DO", "root", "root");
        db->setSchema(_db_name);
    }
}

void Indexer::insertDatabaseItem(DatabaseItem *item) const {
    item->insert(db);
    // cout << "- Insert " << item->toString() << " : OK" << endl;
}

list<File*> Indexer::getFilesFromTag(string tag_name, string tag_value) {
    sql::PreparedStatement *prep_stmt;
    sql::ResultSet *res;
    list<File*> files;
    prep_stmt = db->prepareStatement(GET_FILES_FROM_TAG);

    prep_stmt->setString(1, tag_name);
    prep_stmt->setString(2, tag_name);
    res = prep_stmt->executeQuery();
    
    while (res->next()) {
        File *file = new File();
        file->fillFromStatement(res);
        files.push_back(file);
    }
    delete prep_stmt;
    delete res;
    return files;
}



void Indexer::create_database(bool drop_table) {
    openDatabase();
    sql::Statement *stmt;
    if (drop_table) {
        stmt->executeQuery(DROP_TABLE_STATEMENT);
        // cout << "- Drop tables : OK" << endl;
    }

    stmt->executeQuery(FILE_CREATE_STATEMENT);
    // cout << "- Create File table : OK" << endl;
    stmt->executeQuery(TAG_CREATE_STATEMENT);
    // cout << "- Create Tag table : OK" << endl;
}

void Indexer::indexation(list<File*> files) {
    openDatabase();
    for (File* file : files) {
        insertDatabaseItem(file);
    }
}

void Indexer::fetchFromTag(string tag_name, string tag_value) {
    openDatabase();
    list<File*> files = getFilesFromTag(tag_name, tag_value);
    for (File *file : files) {
        cout << file->toString() << endl;
        delete file;
    }
}