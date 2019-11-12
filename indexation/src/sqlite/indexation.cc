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

void insert_file(sqlite3 *db, const char *file_path, const char *file_name) {
    cout << file_path << endl
         << file_name << endl;
    int error;
    sqlite3_stmt *stmt = 0;
    char const *insert_file = "INSERT INTO File (path, name) VALUES(?, ?)";
    error = sqlite3_prepare_v2(db, insert_file, -1, &stmt, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_prepare_v2 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_bind_text(stmt, 1, file_path, -1, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_bind_text n°1 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_bind_text(stmt, 2, file_name, -1, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_bind_text n°2 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "sqlite3_step error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    } else {
        cout << "- Insert File{" << file_path << ", " << file_name << "} : OK" << endl;
    }
}

void insert_tag(sqlite3 *db, const char *file_path, const char *tag_name, const char *tag_value) {
    sqlite3_stmt *stmt = 0;
    int error;
    char const *insert_tag = "INSERT INTO Tag (file_path, name, value) VALUES(?, ?, ?)";
    error = sqlite3_prepare_v2(db, insert_tag, -1, &stmt, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_prepare_v2 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_bind_text(stmt, 1, file_path, -1, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_bind_text n°1 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_bind_text(stmt, 2, tag_name, -1, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_bind_text n°2 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_bind_text(stmt, 3, tag_value, -1, 0);
    if (error != SQLITE_OK) {
        fprintf(stderr, "sqlite3_bind_text n°3 error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    }
    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "sqlite3_step error %d: %s\n", error, sqlite3_errmsg(db));
        exit(0);
    } else {
        cout << "- Insert Tag{" << file_path << ", " << tag_name << ", " << tag_value << "} : OK" << endl;
    }
}

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int error;

    /* Opens the database */
    error = sqlite3_open("database/corpus_harvester.db", &db);
    if (error != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    }

    const char *tweet_file_path = "a/path/to/a/tweet/twt2";
    const char *tweet_file_name = "twt2";
    insert_file(db, tweet_file_path, tweet_file_name);

    const char *tweet_tag_name = "type";
    const char *tweet_tag_value = "tweet";
    insert_tag(db, tweet_file_path, tweet_tag_name, tweet_tag_value);

    int tweet_content_length = 3;
    const char *tweet_content_values[tweet_content_length] = {"Obiwan", "there", "Kenobi"};
    const char *tweet_content = "content";
    for (int i = 0; i < tweet_content_length; ++i) {
        insert_tag(db, tweet_file_path, tweet_content, tweet_content_values[i]);
    }

    const char *image_file_path = "a/path/to/an/image/img2";
    const char *image_file_name = "img2";
    insert_file(db, image_file_path, image_file_name);

    const char *image_tag_name = "type";
    const char *image_tag_value = "image";
    insert_tag(db, image_file_path, image_tag_name, image_tag_value);

    int image_content_length = 2;
    const char *image_content_values[image_content_length] = {"dog", "garden"};
    const char *image_content = "content";
    for (int i = 0; i < image_content_length; ++i) {
        insert_tag(db, image_file_path, image_content, image_content_values[i]);
    }

    sqlite3_close(db);
}