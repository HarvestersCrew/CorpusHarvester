#include <cstring>
#include "indexation/sqlite/indexer.h"

using std::cerr;
using std::endl;

// int main(int argc, char *argv[]) {
//     Indexer indexer("database/corpus_harvester.db");
//     if (argc == 5) {
//         int tag_name_index = 4;
//         int tag_value_index = 2;
//         if (!strcmp(argv[1], "--tag-name") && !strcmp(argv[3], "--tag-value")) {
//             tag_name_index = 2;
//             tag_value_index = 4;
//         } else if (strcmp(argv[1], "--tag-value") || strcmp(argv[3], "--tag-name")) {
//             cerr << "print_usage" << endl;
//             return (0);
//         }
//         indexer.searchFromTag(argv[tag_name_index], argv[tag_value_index]);
//     } else {
//         cerr << "print_usage" << endl;
//     }
// }