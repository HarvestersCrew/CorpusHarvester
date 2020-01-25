#ifndef INDEXER_H
#define INDEXER_H

using std::string;

#define DROP_TABLE_STATEMENT "DROP TABLE IF EXISTS File;DROP TABLE IF EXISTS Tag;"

class Indexer {

    string _db_name;
    sql::Connection *db;

private:

    void openDatabase();

    void insertDatabaseItem(DatabaseItem* item) const;

    list<File*> getFilesFromTag(string tag_name, string tag_value);

public:

    Indexer(string db_name);

    void create_database(bool drop_table);

    void indexation(list<File*> files);

    void fetchFromTag(string tag_name, string tag_value);
};

#endif