#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

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
    if (error != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    }

    if (argc == 5) {
        int tag_name_index = 4;
        int tag_value_index = 2;
        if (!strcmp(argv[1], "--tag-name") && !strcmp(argv[3], "--tag-value")) {
            tag_name_index = 2;
            tag_value_index = 4;
        } else if (strcmp(argv[1], "--tag-value") || strcmp(argv[3], "--tag-name")) {
            fprintf(stderr, "print_usage 2\n");
            return (0);
        }
        sqlite3_stmt *stmt = 0;
        int error;
        /* Build a request to create the File table*/
        char const *select_from_tag = "SELECT f.path, f.name FROM File f, Tag t "
                                      "WHERE f.path = t.file_path AND "
                                      "t.name = ? AND "
                                      "t.value = ?;";
        error = sqlite3_prepare_v2(db, select_from_tag, -1, &stmt, 0);
        if (error != SQLITE_OK) {
            fprintf(stderr, "sqlite3_prepare_v2 error %d: %s\n", error, sqlite3_errmsg(db));
            exit(0);
        }
        error = sqlite3_bind_text(stmt, 1, argv[tag_name_index], -1, 0);
        if (error != SQLITE_OK) {
            fprintf(stderr, "sqlite3_bind_text n°1 error %d: %s\n", error, sqlite3_errmsg(db));
            exit(0);
        }
        error = sqlite3_bind_text(stmt, 2, argv[tag_value_index], -1, 0);
        if (error != SQLITE_OK) {
            fprintf(stderr, "sqlite3_bind_text n°2 error %d: %s\n", error, sqlite3_errmsg(db));
            exit(0);
        }
        error = sqlite3_step(stmt);
        while (error == SQLITE_ROW) { // While query has result-rows.
            cout << "File { path : " << sqlite3_column_text(stmt, 0) << ", ";
            cout << "name : " << sqlite3_column_text(stmt, 1) << " }" << endl
                 << endl;
            error = sqlite3_step(stmt);
        }
        if (error != SQLITE_DONE) {
            fprintf(stderr, "sqlite3_step error %d: %s\n", error, sqlite3_errmsg(db));
            exit(0);
        }
    } else {
        fprintf(stderr, "print_usage\n");
    }

    sqlite3_close(db);
}