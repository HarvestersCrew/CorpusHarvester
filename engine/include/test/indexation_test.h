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
#define VERBOSE true
#define EVEN_FILES FILE_COUNT / 2 + FILE_COUNT % 2
#define EVEN_TWEETS TWEET_COUNT / 2 + FILE_COUNT % 2

using std::cout;
using std::endl;
using std::list;
using std::ostringstream;
using std::string;

void testCreateDatabase();
void testIndexation();
void testFetchTweets();
void testFetchEvenFiles();
void testFetchByName();
void testFetchBySize();
void testFetchSpecificFiles();
void testFetchSpecificFiles2();
void testFetchSpecificFiles3();
void testCreateCorpus();
void indexation_test();
