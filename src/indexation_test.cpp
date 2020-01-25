#include <cstring>
#include <iostream>
#include <sstream>

#include "indexation/mysql/indexer.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::ostringstream;

Indexer indexer("harvester");

void printExceptionTrace(sql::SQLException &e) {
  cerr << "# ERR: " << e.what();
  cerr << " (MySQL error code: " << e.getErrorCode();
  cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
}

void fillFileRandomly(File *file) {
  ostringstream rand_int;
  rand_int << rand() % 5;
  file->addTag("rand_int", rand_int.str());
  file->addTag("type", "tweet");
  int tweet_subject_length = 3;
  string tweet_subject_values[tweet_subject_length] = {"I'm", "a", "tweet"};
  for (int i = 0; i < tweet_subject_length; ++i) {
    file->addTag("subject", tweet_subject_values[i]);
  }
}

void testCreateDatabase() { indexer.create_database(true); }

void testIndexation() {
  list<File *> files;
  for (int i = 1; i < 6; i++) {
    string i_str = std::to_string(i);
    File *file = new File("/stockage/tweet" + i_str, "tweet" + i_str, 200);
    fillFileRandomly(file);
    files.push_back(file);
  }
  indexer.indexation(files);
}

void testFetchFromTags(string tag_name, string tag_value) {
  list<File *> files = indexer.fetchFromTag(tag_name, tag_value);
  for (File *file : files) {
    cout << file->toString() << endl;
    delete file;
  }
}

int main(int argc, char *argv[]) {
  try {
    testCreateDatabase();
    testIndexation();
    testFetchFromTags("type", "tweet");
    testFetchFromTags("rand_int", "3");
  } catch (sql::SQLException &e) {
    printExceptionTrace(e);
  }
}