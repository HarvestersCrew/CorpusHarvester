#include <string>
#include "indexation/sqlite/database_item.h"

using std::string;

DatabaseItem::DatabaseItem(int id) : _id(id) {}

string DatabaseItem::columnReturnToString(const unsigned char *column) {
    return string(reinterpret_cast<const char *>(column));
}
