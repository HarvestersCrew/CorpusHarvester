#ifndef DATABASE_ITEM_H
#define DATABASE_ITEM_H

#include <algorithm>
#include <iostream>
#include <string>
#include <sqlite3.h>

class DatabaseItem {

protected:	
    int _id;

public:
	DatabaseItem(int id);

    virtual ~DatabaseItem()=default;

    virtual std::string toString() const = 0;

    virtual int insert(sqlite3 *db) = 0;

    virtual void fillFromStatement(sqlite3_stmt *stmt) = 0;

    std::string columnReturnToString(const unsigned char *column);

    int id() const;

    void id(int id);
};

inline int DatabaseItem::id() const { return _id;}
inline void DatabaseItem::id(int id) { _id = id;}

#endif