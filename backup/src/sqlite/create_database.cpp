#include "indexation/sqlite/indexer.h"
#include <iostream>
#include <cstring>

using std::cerr;
using std::endl;

// int main(int argc, char *argv[]) {
//     Indexer indexer("database/corpus_harvester.db");
//     bool drop_table = false;  
//     if (argc > 1) {
//         if (!strcmp(argv[1], "-d")) {
//             drop_table = true;
//         }
//     }
//     int error = indexer.create_database(drop_table);
//     if (error) {
//         cerr << "An error occured during create_database()" << endl;
//     }
// }