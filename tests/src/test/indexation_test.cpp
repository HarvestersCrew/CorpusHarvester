#include "test/indexation_test.h"

sql::Connection *indexer_db = HarvesterDatabase::init();
Indexer indexer = Indexer(indexer_db);
std::string tables[] = {"Corpus", "File", "Tag", "CorpusFiles", "Setting"};
int tables_row_count[] = {0, 0, 0, 0, Setting::get_init_settings_count()};

/* TESTS */

void test_create_database() {
  HarvesterDatabase::open();
  HarvesterDatabase::drop();
  HarvesterDatabase::create();
  sql::Statement *stmt = indexer_db->createStatement();
  sql::ResultSet *res_show = stmt->executeQuery("SHOW TABLES");

  int row_count = 0;
  std::string table;
  sql::ResultSet *res_select;
  int index;
  // Tests if no unexpected tables were created and if the expected one are
  // empty
  while (res_show->next()) {
    table = res_show->getString(1);
    index =
        Assertion::assert_contains(__FUNCTION__, TABLES_COUNT, tables, table);
    res_select = stmt->executeQuery("SELECT * FROM " + table);
    Assertion::assert_equals(__FUNCTION__, tables_row_count[index],
                             res_select->rowsCount());
    row_count++;
  }
  // Tests if every expected tables are created
  if (row_count != TABLES_COUNT) {
    throw TestFailedException(__FUNCTION__,
                              std::to_string(TABLES_COUNT) + " tables",
                              "only " + std::to_string(row_count) + " tables");
  }

  delete stmt;
  delete res_select;
  delete res_show;
}

void test_indexation() {
  std::list<shared_ptr<File>> files;
  for (int i = 0; i < FILE_COUNT; i++) {
    std::string i_str = std::to_string(i);
    std::string i_plus_50_str = std::to_string(i + 50);
    shared_ptr<File> up_file = std::make_shared<File>(File(
        "/stockage/file" + i_str, "file" + i_str, i + 100, "Tweeter", ".txt"));
    fill_file_randomly(up_file, i < TWEET_COUNT, i % 2 == 0);
    up_file->add_tag("retweet", i_plus_50_str);
    up_file->add_tag("_api_id", i_str);
    files.push_back(up_file);
  }
  indexer.indexation(files);

  sql::Statement *stmt = indexer_db->createStatement();
  sql::ResultSet *res_files = stmt->executeQuery("SELECT * FROM File");
  sql::ResultSet *res_tags = stmt->executeQuery("SELECT * FROM Tag");
  // Tests if every expected files were inserted
  Assertion::assert_equals(__FUNCTION__, FILE_COUNT, res_files->rowsCount());
  // Tests if every expected tags were inserted
  Assertion::assert_equals(__FUNCTION__, FILE_COUNT * TAG_COUNT,
                           res_tags->rowsCount());
  delete stmt;
  delete res_files;
  delete res_tags;
}

void test_api_id_exists() {
  shared_ptr<File> file = std::make_shared<File>(
      File("api_id_exists", "api_id_exists", 100, "Tweeter", ".txt"));
  file->add_tag("_api_id", "0");
  bool inserted = indexer.insert_file(file);
  Assertion::assert_false(__FUNCTION__, inserted);
}

void test_same_api_id_different_source() {
  shared_ptr<File> file =
      std::make_shared<File>(File("path", "name", 150, "Tmdb", ".txt"));
  file->add_tag("_api_id", "0");
  bool inserted = indexer.insert_file(file);
  Assertion::assert_true(__FUNCTION__, inserted);
}

void test_create_database2() {
  sql::Statement *stmt = indexer_db->createStatement();
  std::string request;
  for (int i = 0; i < TABLES_COUNT - 1; i++) {
    request += tables[i] + ", ";
  }
  request = "SELECT * FROM " + request + tables[TABLES_COUNT - 1];
  sql::ResultSet *res_select = stmt->executeQuery(request);
  int db_length = res_select->rowsCount();
  HarvesterDatabase::create();
  res_select = stmt->executeQuery(request);
  // Tests if the amount of data in the database doesn't change after a
  // no-dropping database creation
  Assertion::assert_equals(__FUNCTION__, db_length, res_select->rowsCount());
  delete stmt;
  delete res_select;
}

void test_get_setting() {
  std::string name = "storage_root";
  Setting setting = Setting(name, indexer.get_database());
  Assertion::assert_equals(__FUNCTION__, name, setting.get_name());
  Assertion::assert_equals(__FUNCTION__,
                           Setting::get_default_value(Setting::STORAGE_ROOT),
                           setting.get_value());
}

void test_get_wrong_setting() {
  try {
    std::string name = "wrong_setting";
    Setting setting = Setting(name, indexer.get_database());
    Assertion::assert_throw(__FUNCTION__, "SettingNotFoundException");
  } catch (SettingNotFoundException &e) {
    return;
  }
}

void test_fetch_tweets() {
  std::list<shared_ptr<File>> tweets = indexer.fetch_from_tag("type", "tweet");
  Assertion::assert_equals(__FUNCTION__, TWEET_COUNT, tweets.size());
}

void test_fetch_even_files() {
  std::list<shared_ptr<File>> tweets = indexer.fetch_from_tag("is_even", "1");
  Assertion::assert_equals(__FUNCTION__, EVEN_FILES, tweets.size());
}

void test_fetch_by_name() {
  std::list<shared_ptr<File>> tweets =
      indexer.fetch_from_attribute("name", "file12");
  Assertion::assert_equals(__FUNCTION__, 1, tweets.size());
  shared_ptr<File> &tweet = *(tweets.begin());
  Assertion::assert_equals(__FUNCTION__, tweet->get_name(), "file12");
}

void test_fetch_by_size() {
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> tweets =
      sb.add_condition("size", "110", "<=")->build();
  Assertion::assert_equals(__FUNCTION__, "size <= 110", sb.get_filters());
  Assertion::assert_equals(__FUNCTION__, 11, tweets.size());
}

void test_fetch_by_tag_lt() {
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> tweets =
      sb.add_tag_condition("retweet", "65", "<")->build();
  Assertion::assert_equals(__FUNCTION__, 15, tweets.size());
}

void test_fetch_specific_files() {
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> files =
      sb.add_tag_condition("is_even", "1", "=")
          ->logical_and()
          ->add_tag_condition("type", "tweet", "=")
          ->logical_or()
          ->add_tag_condition("subject", "kitty", "=")
          ->logical_and()
          ->add_condition("name", "file8", "=")
          ->build();
  Assertion::assert_equals(
      __FUNCTION__,
      "is_even = 1 AND type = tweet OR subject = kitty AND name = file8",
      sb.get_filters());
  Assertion::assert_equals(__FUNCTION__, 1, files.size());
}

void test_fetch_specific_files2() {
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> files =
      sb.add_tag_condition("is_even", "1", "=")
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
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> files =
      sb.add_condition("name", "file6", "!=")
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
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> files = sb.add_condition("name", "file6", "=")
                                          ->logical_or()
                                          ->add_condition("name", "file8", "=")
                                          ->logical_or()
                                          ->add_condition("name", "file3", "=")
                                          ->build();
  Corpus corpus("file 6/8/3", "01/02/2020", files, sb.get_filters());
  Corpus corpus2("empty", "02/03/2020");
  indexer.save_corpus(corpus);
  indexer.save_corpus(corpus2);
  sql::ResultSet *res_files;
  sql::Statement *stmt = indexer_db->createStatement();
  std::list<Corpus *> corpuses = Corpus::get_all_corpuses(indexer_db);
  Assertion::assert_equals(__FUNCTION__, 2, corpuses.size());
  Corpus *first_element = *corpuses.begin();
  Assertion::assert_equals(__FUNCTION__, "file 6/8/3",
                           first_element->get_title());
  Assertion::assert_false(__FUNCTION__, first_element->has_file());
  res_files = stmt->executeQuery(GET_ALL_CORPUS_FILES);
  Assertion::assert_equals(__FUNCTION__, 3, res_files->rowsCount());
  delete stmt;
  delete res_files;
}

void test_wrong_search() {
  try {
    SearchBuilder sb = indexer.get_search_builder();
    sb.add_condition("name", "file6", "=")->logical_or()->logical_or()->build();
    Assertion::assert_throw(__FUNCTION__, "SQLException");
  } catch (sql::SQLException &e) {
    return;
  }
}

void test_wrong_search2() {
  try {
    SearchBuilder sb = indexer.get_search_builder();
    sb.add_condition("name", "file6", "=")
        ->add_condition("name", "file3", "=")
        ->logical_or()
        ->add_condition("name", "file7", "=")
        ->build();
    Assertion::assert_throw(__FUNCTION__, "SQLException");
  } catch (sql::SQLException &e) {
    return;
  }
}

void test_update_setting() {
  std::string name = "storage_root";
  std::string new_value = "/new/path";
  Setting setting = Setting(name, new_value);
  setting.update(indexer.get_database());
  Setting updated_setting(name, indexer.get_database());
  Assertion::assert_equals(__FUNCTION__, new_value,
                           updated_setting.get_value());
}

void test_insert_setting() {
  std::string name = "new_setting";
  std::string value = "value";
  Setting setting = Setting(name, value);
  bool inserted = setting.insert(indexer.get_database());
  Setting inserted_setting(name, indexer.get_database());
  Assertion::assert_true(__FUNCTION__, inserted);
  Assertion::assert_equals(__FUNCTION__, name, inserted_setting.get_name());
  Assertion::assert_equals(__FUNCTION__, value, inserted_setting.get_value());
}

void test_insert_existing_setting() {
  std::string name = "storage_root";
  Setting existing_setting(name, indexer.get_database());
  std::string value = "value";
  Setting setting = Setting(name, value);
  bool inserted = setting.insert(indexer.get_database());
  Assertion::assert_false(__FUNCTION__, inserted);
  Assertion::assert_not_equals(__FUNCTION__, value,
                               existing_setting.get_value());
}

void indexation_test() {
  std::cout << std::endl << "Indexation tests : " << std::endl;
  Assertion::test(test_create_database, "test_create_database");
  Assertion::test(test_indexation, "test_indexation");
  Assertion::test(test_api_id_exists, "test_api_id_exists");
  Assertion::test(test_same_api_id_different_source,
                  "test_same_api_id_different_source");
  Assertion::test(test_create_database2, "test_create_database2");
  Assertion::test(test_get_setting, "test_get_setting");
  Assertion::test(test_get_wrong_setting, "test_get_wrong_setting");
  Assertion::test(test_fetch_tweets, "test_fetch_tweets");
  Assertion::test(test_fetch_even_files, "test_fetch_even_files");
  Assertion::test(test_fetch_by_tag_lt, "test_fetch_by_tag_lt");
  Assertion::test(test_fetch_by_name, "test_fetch_by_name");
  Assertion::test(test_fetch_by_size, "test_fetch_by_size");
  Assertion::test(test_fetch_specific_files, "test_fetch_specific_files");
  Assertion::test(test_fetch_specific_files2, "test_fetch_specific_files2");
  Assertion::test(test_fetch_specific_files3, "test_fetch_specific_files3");
  Assertion::test(test_create_corpus, "test_create_corpus");
  Assertion::test(test_wrong_search, "test_wrong_search");
  Assertion::test(test_wrong_search2, "test_wrong_search2");
  Assertion::test(test_update_setting, "test_update_setting");
  Assertion::test(test_insert_setting, "test_insert_setting");
  Assertion::test(test_insert_existing_setting, "test_insert_existing_setting");
  HarvesterDatabase::close();
}
