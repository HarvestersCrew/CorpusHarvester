#include <iostream>
#include <string.h>
#include <sqlite3.h>
#include <stdio.h>

using namespace std;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int error;

    /* Opens the database */
    error = sqlite3_open("database/corpus_harvester.db", &db);
    if (error!= SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    }

    if (argc > 1) {
        if (!strcmp(argv[1], "-d")) {
            char const *drop_tables = "DROP TABLE IF EXISTS File;DROP TABLE IF EXISTS Tag;";
            error = sqlite3_exec(db, drop_tables, callback, 0, &zErrMsg);
            if (error != SQLITE_OK) {
                cerr << "SQL error when executing" << drop_tables << ": " << zErrMsg << endl;
                sqlite3_free(zErrMsg);
                return (0);
            } else {
                cout << "- Drop tables : OK" << endl;
            }
        }
    }

    /* Build a request to create the File table*/
    char const *create_file_table = "CREATE TABLE IF NOT EXISTS File("
                                    "path TEXT NOT NULL,"
                                    "name TEXT NOT NULL,"
                                    "PRIMARY KEY (path));";

    /* Build a request to create the Tag table*/
    char const *create_tag_table = "CREATE TABLE IF NOT EXISTS Tag("
                                   "id INTEGER,"
                                   "file_path TEXT NOT NULL,"
                                   "name   TEXT NOT NULL,"
                                   "value  TEXT NOT NULL,"
                                   "PRIMARY KEY (id),"
                                   "FOREIGN KEY (file_path) REFERENCES File(path));";

    /* Executes SQL requests */
    error = sqlite3_exec(db, create_file_table, callback, 0, &zErrMsg);
    if (error != SQLITE_OK) {
        fprintf(stderr, "SQL error when executing %s: %s\n", create_file_table, zErrMsg);
        sqlite3_free(zErrMsg);
        return (0);
    } else {
        cout << "- Create File table : OK" << endl;
    }
    error = sqlite3_exec(db, create_tag_table, callback, 0, &zErrMsg);
    if (error != SQLITE_OK) {
        fprintf(stderr, "SQL error when executing %s: %s\n", create_tag_table, zErrMsg);
        sqlite3_free(zErrMsg);
        return (0);
    } else {
        cout << "- Create Tag table : OK" << endl;
    }

    sqlite3_close(db);
}