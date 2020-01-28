#include <cstring>
#include <iostream>
#include <sstream>

#include "indexation/mysql/indexer.h"
#include "indexation/mysql/search_builder.h"
#include "test/assertion.h"
#include "utils/exceptions.h"

#define FILE_COUNT 50
#define TWEET_COUNT 26
#define TAG_COUNT 5
#define VERBOSE false
#define EVEN_FILES FILE_COUNT / 2 + FILE_COUNT % 2
#define EVEN_TWEETS TWEET_COUNT / 2 + FILE_COUNT % 2

using std::cout;
using std::endl;
using std::list;
using std::ostringstream;
using std::string;

Indexer indexer("harvester", VERBOSE);

void printSQLException(sql::SQLException &e) {
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

void fillFileRandomly(File *file, bool tweet, bool isEven) {
  ostringstream isEvenStream;
  isEvenStream << isEven;
  file->addTag("isEven", isEvenStream.str());
  if (tweet) {
    file->addTag("type", "tweet");
  } else {
    file->addTag("type", "article");
  }
  int tweet_subject_length = 3;
  string tweet_subject_values[tweet_subject_length] = {"tank", "kitty",
                                                       "crisquare"};
  for (int i = 0; i < tweet_subject_length; ++i) {
    file->addTag("subject", tweet_subject_values[i]);
  }
}

/* TESTS */

void testCreateDatabase() {
  indexer.createDatabase(true);
  sql::Connection *db = indexer.getDatabase();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res = stmt->executeQuery("SHOW TABLES");

  int rowCount = 0;
  int tablesCount = 2;
  string tables[tablesCount] = {"File", "Tag"};
  while (res->next()) {
    Assertion::assertEquals(__FUNCTION__, tables[rowCount], res->getString(1));
    rowCount++;
  }
  if (rowCount != tablesCount) {
    throw TestFailedException(__FUNCTION__,
                              std::to_string(tablesCount) + " tables",
                              "only " + std::to_string(rowCount) + " tables");
  }
}

void testIndexation() {
  list<File *> files;
  for (int i = 0; i < FILE_COUNT; i++) {
    string i_str = std::to_string(i);
    File *file =
        new File("/stockage/file" + i_str, "file" + i_str, i + 100, "Tweeter");
    fillFileRandomly(file, i < TWEET_COUNT, i % 2 == 0);
    files.push_back(file);
  }
  indexer.indexation(files);

  sql::Connection *db = indexer.getDatabase();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *resFiles = stmt->executeQuery("SELECT * FROM File");
  sql::ResultSet *resTags = stmt->executeQuery("SELECT * FROM Tag");
  Assertion::assertEquals(__FUNCTION__, FILE_COUNT, resFiles->rowsCount());
  Assertion::assertEquals(__FUNCTION__, FILE_COUNT * TAG_COUNT,
                          resTags->rowsCount());
  delete stmt;
  delete resFiles;
  delete resTags;
}

void testFetchTweets() {
  list<File *> tweets = indexer.fetchFromTag("type", "tweet");
  Assertion::assertEquals(__FUNCTION__, TWEET_COUNT, tweets.size());
}

void testFetchEvenFiles() {
  list<File *> tweets = indexer.fetchFromTag("isEven", "1");
  Assertion::assertEquals(__FUNCTION__, EVEN_FILES, tweets.size());
}

void testFetchByName() {
  list<File *> tweets = indexer.fetchFromAttribute("name", "file12");
  Assertion::assertEquals(__FUNCTION__, 1, tweets.size());
  File *tweet = *(tweets.begin());
  Assertion::assertEquals(__FUNCTION__, tweet->getName(), "file12");
}

void testFetchBySize() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> tweets = sb->addCondition("size", "110", "<=")->build();
  Assertion::assertEquals(__FUNCTION__, 11, tweets.size());
}

void testFetchSpecificFiles() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> files = sb->addTagCondition("isEven", "1", "=")
                           ->logicalAnd()
                           ->addTagCondition("type", "tweet", "=")
                           ->logicalOr()
                           ->addTagCondition("subject", "kitty", "=")
                           ->logicalAnd()
                           ->addCondition("name", "file8", "=")
                           ->build();
  ;
  Assertion::assertEquals(__FUNCTION__, 1, files.size());
}

void testFetchSpecificFiles2() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> files = sb->addTagCondition("isEven", "1", "=")
                           ->logicalAnd()
                           ->addCondition("name", "file6", "=")
                           ->logicalAnd()
                           ->addTagCondition("subject", "kitty", "=")
                           ->logicalOr()
                           ->addTagCondition("subject", "tank", "=")
                           ->build();
  ;
  Assertion::assertEquals(__FUNCTION__, 1, files.size());
}

int main(int argc, char *argv[]) {
  cout << endl << "Indexation tests : " << endl;
  try {
    Assertion::test(testCreateDatabase, "testCreateDatabase");
    Assertion::test(testIndexation, "testIndexation");
    Assertion::test(testFetchTweets, "testFetchTweets");
    Assertion::test(testFetchEvenFiles, "testFetchEvenFiles");
    Assertion::test(testFetchByName, "testFetchByName");
    Assertion::test(testFetchBySize, "testFetchBySize");
    Assertion::test(testFetchSpecificFiles, "testFetchSpecificFiles");
    Assertion::test(testFetchSpecificFiles2, "testFetchSpecificFiles2");
  } catch (TestFailedException &e) {
    cout << e.what() << endl;
  } catch (sql::SQLException &e) {
    printSQLException(e);
  }
}