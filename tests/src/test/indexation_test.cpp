#include "test/indexation_test.h"

std::string tables[] = {"Corpus", "File", "Tag", "CorpusFiles", "Setting"};
int tables_row_count[] = {0, 0, 0, 0, Setting::get_init_settings_count()};

/* TESTS */

void test_create_database() {
  PoolDB::init(1);
  PoolDB::drop();
  PoolDB::init(1);
  auto con = PoolDB::borrow_from_pool();
  logger::set_level(logger::level::NONE);
  sql::Statement *stmt = con->createStatement();
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
  PoolDB::unborrow_from_pool(con);
}

void test_indexation() {
  std::list<shared_ptr<File>> files;
  for (int i = 0; i < FILE_COUNT; i++) {
    std::string i_str = std::to_string(i);
    std::string i_plus_50_str = std::to_string(i + 50);
    shared_ptr<File> up_file =
        std::make_shared<File>(File("/stockage/file" + i_str, "file" + i_str,
                                    i + 100, "Tweeter", ".txt", "text"));
    fill_file_randomly(up_file, i < TWEET_COUNT, i % 2 == 0);
    up_file->add_tag("retweet", i_plus_50_str);
    up_file->add_tag("_api_id", i_str);
    files.push_back(up_file);
  }
  auto con = PoolDB::borrow_from_pool();
  Indexer().indexation(files);

  sql::Statement *stmt = con->createStatement();
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
  PoolDB::unborrow_from_pool(con);
}

void test_api_id_exists() {
  shared_ptr<File> file = std::make_shared<File>(
      File("api_id_exists", "api_id_exists", 100, "Tweeter", ".png", "image"));
  file->add_tag("_api_id", "0");
  auto con = PoolDB::borrow_from_pool();
  bool inserted = Indexer().insert_file(file);
  Assertion::assert_false(__FUNCTION__, inserted);
  PoolDB::unborrow_from_pool(con);
}

void test_same_api_id_different_source() {
  shared_ptr<File> file = std::make_shared<File>(
      File("path", "name", 150, "Tmdb", ".jpg", "image"));
  file->add_tag("_api_id", "0");
  auto con = PoolDB::borrow_from_pool();
  bool inserted = Indexer().insert_file(file);
  Assertion::assert_true(__FUNCTION__, inserted);
  PoolDB::unborrow_from_pool(con);
}

void test_create_database2() {
  auto con = PoolDB::borrow_from_pool();
  sql::Statement *stmt = con->createStatement();
  std::string request;
  for (int i = 0; i < TABLES_COUNT - 1; i++) {
    request += tables[i] + ", ";
  }
  request = "SELECT * FROM " + request + tables[TABLES_COUNT - 1];
  sql::ResultSet *res_select = stmt->executeQuery(request);
  int db_length = res_select->rowsCount();
  logger::set_level(logger::level::NONE);
  res_select = stmt->executeQuery(request);
  // Tests if the amount of data in the database doesn't change after a
  // no-dropping database creation
  Assertion::assert_equals(__FUNCTION__, db_length, res_select->rowsCount());
  delete stmt;
  delete res_select;
  PoolDB::unborrow_from_pool(con);
}

void test_get_setting() {
  std::string name = "storage_root";
  Setting setting = Setting(name);
  Assertion::assert_equals(__FUNCTION__, name, setting.get_name());
  Assertion::assert_equals(__FUNCTION__,
                           Setting::get_default_value(Setting::STORAGE_ROOT),
                           setting.get_value());
}

void test_get_wrong_setting() {
  try {
    std::string name = "wrong_setting";
    Setting setting = Setting(name);
    Assertion::assert_throw(__FUNCTION__, "SettingNotFoundException");
  } catch (SettingNotFoundException &e) {
    return;
  }
}

void test_fetch_tweets() {
  auto con = PoolDB::borrow_from_pool();
  std::list<shared_ptr<File>> tweets =
      Indexer().fetch_from_tag("type", "tweet");
  PoolDB::unborrow_from_pool(con);
  Assertion::assert_equals(__FUNCTION__, TWEET_COUNT, tweets.size());
}

void test_fetch_even_files() {
  auto con = PoolDB::borrow_from_pool();
  std::list<shared_ptr<File>> tweets = Indexer().fetch_from_tag("is_even", "1");
  PoolDB::unborrow_from_pool(con);
  Assertion::assert_equals(__FUNCTION__, EVEN_FILES, tweets.size());
}

void test_fetch_by_name() {
  auto con = PoolDB::borrow_from_pool();
  std::list<shared_ptr<File>> tweets =
      Indexer().fetch_from_attribute("name", "file12");
  PoolDB::unborrow_from_pool(con);
  Assertion::assert_equals(__FUNCTION__, 1, tweets.size());
  shared_ptr<File> &tweet = *(tweets.begin());
  Assertion::assert_equals(__FUNCTION__, tweet->get_name(), "file12");
}

void test_fetch_by_size() {
  auto con = PoolDB::borrow_from_pool();
  SearchBuilder sb = Indexer().get_search_builder();
  std::list<shared_ptr<File>> tweets =
      sb.add_condition("size", "110", "<=")->build();
  Assertion::assert_equals(__FUNCTION__, "size <= 110", sb.get_filters());
  Assertion::assert_equals(__FUNCTION__, 11, tweets.size());
}

void test_fetch_by_tag_lt() {
  auto con = PoolDB::borrow_from_pool();
  SearchBuilder sb = Indexer().get_search_builder();
  std::list<shared_ptr<File>> tweets =
      sb.add_tag_condition("retweet", "65", "<")->build();
  Assertion::assert_equals(__FUNCTION__, 15, tweets.size());
}

void test_fetch_specific_files() {
  auto con = PoolDB::borrow_from_pool();
  SearchBuilder sb = Indexer().get_search_builder();
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
  auto con = PoolDB::borrow_from_pool();
  SearchBuilder sb = Indexer().get_search_builder();
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
  auto con = PoolDB::borrow_from_pool();
  SearchBuilder sb = Indexer().get_search_builder();
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
  auto con = PoolDB::borrow_from_pool();
  sql::Statement *stmt = con->createStatement();
  stmt->execute("DELETE FROM CorpusFiles");
  stmt->execute("DELETE FROM Corpus");

  SearchBuilder sb = Indexer().get_search_builder();
  std::list<shared_ptr<File>> files = sb.add_condition("name", "file6", "=")
                                          ->logical_or()
                                          ->add_condition("name", "file8", "=")
                                          ->logical_or()
                                          ->add_condition("name", "file3", "=")
                                          ->build();
  Corpus corpus("file_6-8-3", files, sb.get_filters());
  Corpus corpus2("empty");
  Indexer().save_corpus(corpus);
  Indexer().save_corpus(corpus2);
  sql::ResultSet *res_files;
  std::list<shared_ptr<Corpus>> corpuses =
      Corpus::get_all_corpuses(Corpus::ordering_method::NONE);
  Assertion::assert_equals(__FUNCTION__, 2, corpuses.size());
  shared_ptr<Corpus> first_element = *corpuses.begin();
  Assertion::assert_equals(__FUNCTION__, "file_6-8-3",
                           first_element->get_title());
  res_files = stmt->executeQuery(GET_ALL_CORPUS_FILES);
  Assertion::assert_equals(__FUNCTION__, 3, res_files->rowsCount());

  delete stmt;
  delete res_files;
  PoolDB::unborrow_from_pool(con);
}

void test_fetch_corpuses() {

  auto con = PoolDB::borrow_from_pool();
  sql::Statement *stmt = con->createStatement();
  stmt->execute("DELETE FROM CorpusFiles");
  stmt->execute("DELETE FROM Corpus");
  delete stmt;
  PoolDB::unborrow_from_pool(con);

  std::list<shared_ptr<Corpus>> corpuses;
  std::vector<shared_ptr<Corpus>> corpuses_vec;

  try {
    Corpus::get_corpus_from_id(0);
    Assertion::assert_throw(__FUNCTION__, "db_id_not_found");
  } catch (const db_id_not_found &e) {
  }

  Corpus c1("Jurassic Park");
  Corpus c2("Zootopia");
  Corpus c3("Avengers");
  c1.insert();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  c2.insert();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  c3.insert();

  Corpus::get_corpus_from_id(c1.get_id());

  corpuses = Corpus::get_all_corpuses(Corpus::ordering_method::DATE_ASC);
  corpuses_vec = vector<shared_ptr<Corpus>>{corpuses.begin(), corpuses.end()};
  Assertion::assert_equals(__FUNCTION__, 3, corpuses_vec.size());
  Assertion::assert_equals(__FUNCTION__, "Jurassic Park",
                           corpuses_vec.at(0)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Zootopia",
                           corpuses_vec.at(1)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Avengers",
                           corpuses_vec.at(2)->get_title());

  corpuses = Corpus::get_all_corpuses(Corpus::ordering_method::DATE_DESC);
  corpuses_vec = vector<shared_ptr<Corpus>>{corpuses.begin(), corpuses.end()};
  Assertion::assert_equals(__FUNCTION__, 3, corpuses_vec.size());
  Assertion::assert_equals(__FUNCTION__, "Jurassic Park",
                           corpuses_vec.at(2)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Zootopia",
                           corpuses_vec.at(1)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Avengers",
                           corpuses_vec.at(0)->get_title());

  corpuses = Corpus::get_all_corpuses(Corpus::ordering_method::NAME_ASC);
  corpuses_vec = vector<shared_ptr<Corpus>>{corpuses.begin(), corpuses.end()};
  Assertion::assert_equals(__FUNCTION__, 3, corpuses_vec.size());
  Assertion::assert_equals(__FUNCTION__, "Jurassic Park",
                           corpuses_vec.at(1)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Zootopia",
                           corpuses_vec.at(2)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Avengers",
                           corpuses_vec.at(0)->get_title());

  corpuses = Corpus::get_all_corpuses(Corpus::ordering_method::NAME_DESC);
  corpuses_vec = vector<shared_ptr<Corpus>>{corpuses.begin(), corpuses.end()};
  Assertion::assert_equals(__FUNCTION__, 3, corpuses_vec.size());
  Assertion::assert_equals(__FUNCTION__, "Jurassic Park",
                           corpuses_vec.at(1)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Zootopia",
                           corpuses_vec.at(0)->get_title());
  Assertion::assert_equals(__FUNCTION__, "Avengers",
                           corpuses_vec.at(2)->get_title());
}

void test_delete_corpuss() {
  Corpus c = Corpus("Imacorpus");
  c.insert();
  c.delete_();
  try {
    Corpus::get_corpus_from_id(c.get_id());
    Assertion::assert_throw(__FUNCTION__, "db_id_not_found");
  } catch (db_id_not_found &e) {
    return;
  }
}

void test_wrong_search() {
  try {
    auto con = PoolDB::borrow_from_pool();
    SearchBuilder sb = Indexer().get_search_builder();
    sb.add_condition("name", "file6", "=")->logical_or()->logical_or()->build();
    Assertion::assert_throw(__FUNCTION__, "SQLException");
  } catch (sql::SQLException &e) {
    return;
  }
}

void test_wrong_search2() {
  try {
    auto con = PoolDB::borrow_from_pool();
    SearchBuilder sb = Indexer().get_search_builder();
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
  setting.update();
  Setting updated_setting(name);
  Assertion::assert_equals(__FUNCTION__, new_value,
                           updated_setting.get_value());
}

void test_insert_setting() {
  std::string name = "new_setting";
  std::string value = "value";
  Setting setting = Setting(name, value);
  bool inserted = setting.insert();
  Setting inserted_setting(name);
  Assertion::assert_true(__FUNCTION__, inserted);
  Assertion::assert_equals(__FUNCTION__, name, inserted_setting.get_name());
  Assertion::assert_equals(__FUNCTION__, value, inserted_setting.get_value());
}

void test_insert_existing_setting() {
  std::string name = "storage_root";
  Setting existing_setting(name);
  std::string value = "value";
  Setting setting = Setting(name, value);
  bool inserted = setting.insert();
  Assertion::assert_false(__FUNCTION__, inserted);
  Assertion::assert_not_equals(__FUNCTION__, value,
                               existing_setting.get_value());
}

void test_get_statistics() {
  db_statistics stats = Indexer::get_statistics();
  Assertion::assert_equals(__FUNCTION__, 51, stats.file_count);
  Assertion::assert_equals(__FUNCTION__, 3, stats.corpus_count);
  Assertion::assert_equals(__FUNCTION__, 50, stats.text_count);
  Assertion::assert_equals(__FUNCTION__, 1, stats.image_count);
  Assertion::assert_equals(__FUNCTION__, 6375, stats.total_size);
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
  Assertion::test(test_fetch_corpuses, "test_fetch_corpuses");
  Assertion::test(test_delete_corpuss, "test_delete_corpuss");
  Assertion::test(test_wrong_search, "test_wrong_search");
  Assertion::test(test_wrong_search2, "test_wrong_search2");
  Assertion::test(test_update_setting, "test_update_setting");
  Assertion::test(test_insert_setting, "test_insert_setting");
  Assertion::test(test_insert_existing_setting, "test_insert_existing_setting");
  Assertion::test(test_get_statistics, "test_get_statistics");
}
