#include <cstring>
#include <iostream>
#include <sstream>

#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

#define TABLES_COUNT 4
#define FILE_COUNT 50
#define TWEET_COUNT 26
#define TAG_COUNT 6
#define VERBOSE false
#define EVEN_FILES FILE_COUNT / 2 + FILE_COUNT % 2
#define EVEN_TWEETS TWEET_COUNT / 2 + FILE_COUNT % 2

using std::cout;
using std::endl;
using std::list;
using std::ostringstream;
using std::string;

void test_create_database();
void test_indexation();
void test_create_database2();
void test_fetch_tweets();
void test_fetch_even_files();
void test_fetch_by_name();
void test_fetch_by_size();
void test_fetch_by_tag_lt();
void test_fetch_specific_files();
void test_fetch_specific_files2();
void test_fetch_specific_files3();
void test_create_corpus();
void test_wrong_search();
void test_wrong_search2();
void indexation_test();
