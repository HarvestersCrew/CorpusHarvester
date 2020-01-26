#include <cstring>
#include <iostream>
#include <sstream>

#include "indexation/mysql/indexer.h"
#include "test/assertion.h"
#include "utils/exceptions.h"

#define FILE_COUNT 19
#define TWEET_COUNT 13
#define TAG_COUNT 5

using std::cout;
using std::endl;
using std::list;
using std::ostringstream;
using std::string;

Indexer indexer("harvester", false);

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
  string tweet_subject_values[tweet_subject_length] = {"I'm", "a", "tweet"};
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
    File *file = new File("/stockage/tweet" + i_str, "tweet" + i_str, 200);
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
}

void testFetchTweets() {
  list<File *> tweets = indexer.fetchFromTag("type", "tweet");
  Assertion::assertEquals(__FUNCTION__, TWEET_COUNT, tweets.size());
}

void testFetchEvenFiles() {
  list<File *> tweets = indexer.fetchFromTag("isEven", "1");
  Assertion::assertEquals(__FUNCTION__, FILE_COUNT / 2 + FILE_COUNT % 2,
                          tweets.size());
}

int main(int argc, char *argv[]) {
  cout << endl << "Indexation tests : " << endl;
  try {
    Assertion::test(testCreateDatabase, "testCreateDatabase");
    Assertion::test(testIndexation, "testIndexation");
    Assertion::test(testFetchTweets, "testFetchTweets");
    Assertion::test(testFetchEvenFiles, "testFetchEvenFiles");
  } catch (TestFailedException &e) {
    cout << e.what() << endl;
  } catch (sql::SQLException &e) {
    printSQLException(e);
  }
}