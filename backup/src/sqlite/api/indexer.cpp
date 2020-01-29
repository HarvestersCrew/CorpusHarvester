#include <sstream>
#include <iostream>
#include <sqlite3.h>
#include "indexation/sqlite/indexer.h"
#include "indexation/sqlite/sqlite_util.h"
#include "indexation/sqlite/file.h"

using std::ostringstream;
using std::cout;
using std::endl;

Indexer::Indexer(string db_name) : 
    _db_name(db_name) {}

int Indexer::openDatabase(sqlite3 **db) {
    int error = sqlite3_open(_db_name.c_str(), db);
    error = manage_errors(error, SQLITE_OK, "Can't open database: ", sqlite3_errmsg(*db));
    return error;
}

int Indexer::insertDatabaseItem(sqlite3 *db, DatabaseItem *item) const {
    int error = item->insert(db);
    if (!error) {
        cout << "- Insert " << item->toString() << " : OK" << endl;
    }
    return error;
}

int Indexer::addFile(sqlite3 *db, string name) const {
    int error = 0;
    string path("data/" + name);
    File fileToAdd(path, name, 200);
    fileToAdd.addTag("type", "tweet");

    ostringstream rand_int;
    rand_int << rand() % 4;
    fileToAdd.addTag("rand_int", rand_int.str());

    int tweet_subject_length = 3;
    string tweet_subject_values[tweet_subject_length] = {"I'm", "a", "tweet"};
    for (int i = 0; i < tweet_subject_length; ++i) {
        fileToAdd.addTag("subject", tweet_subject_values[i]);
    }
    error = insertDatabaseItem(db, &fileToAdd);
    return error;
}

int Indexer::getFilesFromTag(sqlite3 *db, string tag_name, string tag_value) {
    sqlite3_stmt *stmt = 0;
    int code;
    int error;
    char const *select_from_tag = GET_FILES_FROM_TAG;
    code = sqlite3_prepare_v2(db, select_from_tag, -1, &stmt, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_prepare_v2()", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 1, tag_name.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°1", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_bind_text(stmt, 2, tag_value.c_str(), -1, 0);
    error = manage_errors(code, SQLITE_OK, "When executing sqlite3_bind_text() n°2", sqlite3_errmsg(db));
    if (error) {
        return error;
    }

    code = sqlite3_step(stmt);
    File file;
    while (code == SQLITE_ROW) { // While query has result-rows.
        file.fillFromStatement(stmt);
        cout << file.toString() << endl;
        code = sqlite3_step(stmt);
    }
    error = manage_errors(code, SQLITE_DONE, "When executing sqlite3_step()", sqlite3_errmsg(db));
    return error;
}

int Indexer::create_database(bool drop_table) {
    sqlite3 *db;
    int error;
    int code;

    error = openDatabase(&db);
    if (error) {
        return error;
    }

    if (drop_table) {
        char const *drop_tables = "DROP TABLE IF EXISTS File;DROP TABLE IF EXISTS Tag;";
        code = sqlite3_exec(db, drop_tables, NULL, 0, nullptr);
        error = manage_errors(code, SQLITE_OK, "When dropping tables : ", sqlite3_errmsg(db));
        if (error) {
            return error;
        }
        cout << "- Drop tables : OK" << endl;
    }

    /* Build a request to create the File table*/
    char const *create_file_table = FILE_CREATE_STATEMENT;
    /* Build a request to create the Tag table*/
    char const *create_tag_table = TAG_CREATE_STATEMENT;

    /* Executes SQL requests */
    code = sqlite3_exec(db, create_file_table, NULL, 0, nullptr);
    error = manage_errors(code, SQLITE_OK, "When creating table File", sqlite3_errmsg(db));
    if (error) {
        return error;
    }
    cout << "- Create File table : OK" << endl;
    code = sqlite3_exec(db, create_tag_table, NULL, 0, nullptr);
    error = manage_errors(code, SQLITE_OK, "When creating table Tag", sqlite3_errmsg(db));
    if (error) {
        return error;
    }
    cout << "- Create Tag table : OK" << endl;
    sqlite3_close(db);
    return error;
}

int Indexer::indexation() {
    srand(time(NULL));
    int error;
    sqlite3 *db;
    error = openDatabase(&db);
    if (error) {
        return error;
    }

    ostringstream name;
    for (int i = 0; i < 5; i++) {
        name << "tweet" << i;
        error = addFile(db, name.str());
        if (error) {
            return error;
        }
        name.str("");
        name.clear();
    }
    sqlite3_close(db);
    return error;
}

int Indexer::searchFromTag(string tag_name, string tag_value) {
    int error;
    sqlite3 *db;
    error = openDatabase(&db);
    if (error) {
        return error;
    }
    error = getFilesFromTag(db, tag_name, tag_value);
    return error;
}