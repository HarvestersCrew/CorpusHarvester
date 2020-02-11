#include "test/indexation_test.h"

Indexer indexer("harvester", VERBOSE);

/* TESTS */

void test_create_database() {
  indexer.create_database(true);
  sql::Connection *db = indexer.get_database();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res = stmt->executeQuery("SHOW TABLES");

  int row_count = 0;
  string tables[] = {"Corpus", "File", "Tag", "CorpusFiles"};
  while (res->next()) {
    Assertion::assert_contains(__FUNCTION__, TABLES_COUNT, tables,
                               res->getString(1));
    row_count++;
  }
  if (row_count != TABLES_COUNT) {
    throw TestFailedException(__FUNCTION__,
                              std::to_string(TABLES_COUNT) + " tables",
                              "only " + std::to_string(row_count) + " tables");
  }
}

void test_indexation() {
  list<File *> files;
  for (int i = 0; i < FILE_COUNT; i++) {
    string i_str = std::to_string(i);
    File *file =
        new File("/stockage/file" + i_str, "file" + i_str, i + 100, "Tweeter");
    fill_file_randomly(file, i < TWEET_COUNT, i % 2 == 0);
    file->add_tag("retweet", i_str);
    files.push_back(file);
  }
  indexer.indexation(files);

  sql::Connection *db = indexer.get_database();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res_files = stmt->executeQuery("SELECT * FROM File");
  sql::ResultSet *res_tags = stmt->executeQuery("SELECT * FROM Tag");
  Assertion::assert_equals(__FUNCTION__, FILE_COUNT, res_files->rowsCount());
  Assertion::assert_equals(__FUNCTION__, FILE_COUNT * TAG_COUNT,
                           res_tags->rowsCount());
  delete stmt;
  delete res_files;
  delete res_tags;
}

void test_fetch_tweets() {
  list<File *> tweets = indexer.fetch_from_tag("type", "tweet");
  Assertion::assert_equals(__FUNCTION__, TWEET_COUNT, tweets.size());
}

void test_fetch_even_files() {
  list<File *> tweets = indexer.fetch_from_tag("is_even", "1");
  Assertion::assert_equals(__FUNCTION__, EVEN_FILES, tweets.size());
}

void test_fetch_by_name() {
  list<File *> tweets = indexer.fetch_from_attribute("name", "file12");
  Assertion::assert_equals(__FUNCTION__, 1, tweets.size());
  File *tweet = *(tweets.begin());
  Assertion::assert_equals(__FUNCTION__, tweet->get_name(), "file12");
}

void test_fetch_by_size() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> tweets = sb->add_condition("size", "110", "<=")->build();
  Assertion::assert_equals(__FUNCTION__, 11, tweets.size());
}

void test_fetch_by_tag_lt() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> tweets = sb->add_tag_condition("retweet", "15", "<")->build();
  Assertion::assert_equals(__FUNCTION__, 15, tweets.size());
}

void test_fetch_specific_files() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> files = sb->add_tag_condition("is_even", "1", "=")
                           ->logical_and()
                           ->add_tag_condition("type", "tweet", "=")
                           ->logical_or()
                           ->add_tag_condition("subject", "kitty", "=")
                           ->logical_and()
                           ->add_condition("name", "file8", "=")
                           ->build();
  Assertion::assert_equals(__FUNCTION__, 1, files.size());
}

void test_fetch_specific_files2() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> files = sb->add_tag_condition("is_even", "1", "=")
                           ->logical_and()
                           ->add_condition("name", "file6", "=")
                           ->logical_and()
                           ->add_tag_condition("subject", "kitty", "=")
                           ->logical_or()
                           ->add_tag_condition("subject", "tank", "=")
                           ->build();
  Assertion::assert_equals(__FUNCTION__, 1, files.size());
}

void test_fetch_specific_files3() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> files = sb->add_condition("name", "file6", "!=")
                           ->logical_and()
                           ->add_tag_condition("type", "tweet", "=")
                           ->logical_or()
                           ->add_tag_condition("subject", "tank", "=")
                           ->logical_and()
                           ->add_condition("size", "112", "<")
                           ->build();
  Assertion::assert_equals(__FUNCTION__, 11, files.size());
}

void test_create_corpus() {
  SearchBuilder *sb = new SearchBuilder(indexer.get_database(), VERBOSE);
  list<File *> files = sb->add_condition("name", "file6", "=")
                           ->logical_or()
                           ->add_condition("name", "file8", "=")
                           ->logical_or()
                           ->add_condition("name", "file3", "=")
                           ->build();
  Corpus corpus("file 6/8/3", "01/02/2020", files);
  indexer.save_corpus(corpus);
  sql::Connection *db = indexer.get_database();
  sql::Statement *stmt = db->createStatement();
  sql::ResultSet *res_files = stmt->executeQuery("SELECT * FROM Corpus");
  Assertion::assert_equals(__FUNCTION__, 1, res_files->rowsCount());
  res_files = stmt->executeQuery("SELECT * FROM CorpusFiles");
  Assertion::assert_equals(__FUNCTION__, 3, res_files->rowsCount());
}

void indexation_test() {
  cout << endl << "Indexation tests : " << endl;
  try {
    Assertion::test(test_create_database, "test_create_database");
    Assertion::test(test_indexation, "test_indexation");
    Assertion::test(test_fetch_tweets, "test_fetch_tweets");
    Assertion::test(test_fetch_even_files, "test_fetch_even_files");
    Assertion::test(test_fetch_by_tag_lt, "test_fetch_by_tag_lt");
    Assertion::test(test_fetch_by_name, "test_fetch_by_name");
    Assertion::test(test_fetch_by_size, "test_fetch_by_size");
    Assertion::test(test_fetch_specific_files, "test_fetch_specific_files");
    Assertion::test(test_fetch_specific_files2, "test_fetch_specific_files2");
    Assertion::test(test_fetch_specific_files3, "test_fetch_specific_files3");
    Assertion::test(test_create_corpus, "test_create_corpus");
  } catch (TestFailedException &e) {
    cout << e.what() << endl;
  } catch (sql::SQLException &e) {
    print_sql_exception(e);
  }
}
