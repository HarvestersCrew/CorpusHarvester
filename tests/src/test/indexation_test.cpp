#include "test/indexation_test.h"

Indexer indexer("harvester", VERBOSE);

/* TESTS */

void testCreateDatabase() {
  indexer.createDatabase(true);
  sql::Connection *db = indexer.getDatabase();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res = stmt->executeQuery("SHOW TABLES");

  int rowCount = 0;
  string tables[] = {"Corpus", "File", "Tag", "CorpusFiles"};
  while (res->next()) {
    Assertion::assertContains(__FUNCTION__, TABLES_COUNT, tables,
                              res->getString(1));
    rowCount++;
  }
  if (rowCount != TABLES_COUNT) {
    throw TestFailedException(__FUNCTION__,
                              std::to_string(TABLES_COUNT) + " tables",
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
    file->addTag("retweet", i_str);
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
  cout << "1" << endl;
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

void testFetchByTagLowerThan() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> tweets = sb->addTagCondition("retweet", "15", "<")->build();
  Assertion::assertEquals(__FUNCTION__, 15, tweets.size());
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
  Assertion::assertEquals(__FUNCTION__, 1, files.size());
}

void testFetchSpecificFiles3() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> files = sb->addCondition("name", "file6", "!=")
                           ->logicalAnd()
                           ->addTagCondition("type", "tweet", "=")
                           ->logicalOr()
                           ->addTagCondition("subject", "tank", "=")
                           ->logicalAnd()
                           ->addCondition("size", "112", "<")
                           ->build();
  Assertion::assertEquals(__FUNCTION__, 11, files.size());
}

void testCreateCorpus() {
  SearchBuilder *sb = new SearchBuilder(indexer.getDatabase(), VERBOSE);
  list<File *> files = sb->addCondition("name", "file6", "=")
                           ->logicalOr()
                           ->addCondition("name", "file8", "=")
                           ->logicalOr()
                           ->addCondition("name", "file3", "=")
                           ->build();
  Corpus corpus("file 6/8/3", "01/02/2020", files);
  indexer.saveCorpus(corpus);
  sql::Connection *db = indexer.getDatabase();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *resFiles = stmt->executeQuery("SELECT * FROM Corpus");
  Assertion::assertEquals(__FUNCTION__, 1, resFiles->rowsCount());
  resFiles = stmt->executeQuery("SELECT * FROM CorpusFiles");
  Assertion::assertEquals(__FUNCTION__, 3, resFiles->rowsCount());
}

void indexation_test() {
  cout << endl << "Indexation tests : " << endl;
  try {
    Assertion::test(testCreateDatabase, "testCreateDatabase");
    Assertion::test(testIndexation, "testIndexation");
    Assertion::test(testFetchTweets, "testFetchTweets");
    Assertion::test(testFetchEvenFiles, "testFetchEvenFiles");
    Assertion::test(testFetchByTagLowerThan, "testFetchByTagLowerThan");
    Assertion::test(testFetchByName, "testFetchByName");
    Assertion::test(testFetchBySize, "testFetchBySize");
    Assertion::test(testFetchSpecificFiles, "testFetchSpecificFiles");
    Assertion::test(testFetchSpecificFiles2, "testFetchSpecificFiles2");
    Assertion::test(testFetchSpecificFiles3, "testFetchSpecificFiles3");
    Assertion::test(testCreateCorpus, "testCreateCorpus");
  } catch (TestFailedException &e) {
    cout << e.what() << endl;
  } catch (sql::SQLException &e) {
    printSQLException(e);
  }
}
