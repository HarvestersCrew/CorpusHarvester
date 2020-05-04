#include "test/storage_test.h"

shared_ptr<File> file = std::make_shared<File>(
    File("null", "storage", 100, "twitter", ".txt", "text"));

void test_file_destination() {

  Storage storage;
  std::string STORED_PATH = storage.get_root_folder_name();
  std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";

  std::string fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH + "download/twitter/dd/ec/ebdea58b5f264d27f1f7909bab74.txt",
      fileDest);
  file->set_source("wikicommons");
  file->set_name("harvester");
  file->set_format(".jpg");
  fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH +
          "download/wikicommons/9f/8f/961b5607b1003cf830f44c67efc9.jpg",
      fileDest);
}

void test_empty_file_name() {

  Storage storage;
  std::string STORED_PATH = storage.get_root_folder_name();
  std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";

  file->set_name("");
  std::string fileDest = storage.file_destination(file);
  Assertion::assert_not_equals(__FUNCTION__, "", fileDest);
}

void test_store_one_file() {

  Storage storage;
  std::string STORED_PATH = storage.get_root_folder_name();
  std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";

  file->set_source("tmdb");
  file->set_name("test");
  file->set_format(".png");
  std::string content = "Bien le bonjour je suis un fichier de test";
  file->set_content(content);
  std::string file_dest = storage.store_file(file);
  std::string expected_path = "download/tmdb/09/8f/";
  std::string expected_name = "6bcd4621d373cade4e832627b4f6";
  std::string expected_destination =
      STORED_PATH + expected_path + expected_name + ".png";
  Assertion::assert_equals(__FUNCTION__, expected_destination, file_dest);
  Assertion::assert_equals(__FUNCTION__, expected_path, file->get_path());
  Assertion::assert_equals(__FUNCTION__, expected_name, file->get_name());
  std::stringstream ss;
  ss << std::ifstream(expected_destination).rdbuf();
  Assertion::assert_equals(__FUNCTION__, content + "\n", ss.str());
}

void test_export_methods() {
  std::list<shared_ptr<File>> files;
  files.push_back(file);
  string path = ExportMethod::compressed_export(files, "test_indirect",
                                                ExportMethod::methods::ZIP);
  Assertion::assert_true(__FUNCTION__, std::filesystem::exists(path));

  shared_ptr<ZipExport> z = std::make_shared<ZipExport>();
  path = z->compressed_export(files, "test_direct");
  Assertion::assert_true(__FUNCTION__, std::filesystem::exists(path));
}

void test_export_corpus_zip() {

  Storage storage;
  std::string STORED_PATH = storage.get_root_folder_name();
  std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";

  std::list<shared_ptr<File>> files;
  files.push_back(file);
  shared_ptr<File> file2 = std::make_shared<File>(
      File("null", "test2", 100, "twitter", ".txt", "text"));
  storage.store_file(file2);
  files.push_back(file2);
  logger::set_level(logger::DEBUG);
  auto con = PoolDB::borrow_from_pool();
  for (auto &file : files) {
    file->insert();
  }
  Corpus corpus = Corpus("corpus_test", files, "something");
  corpus.insert();
  corpus.export_(ExportMethod::methods::ZIP);
  std::string new_extraction_path =
      storage.get_corpus_path() + std::to_string(corpus.get_id()) + ".zip";
  Assertion::assert_equals(__FUNCTION__, new_extraction_path,
                           corpus.get_extraction_path().value_or(""));
  Assertion::assert_true(
      __FUNCTION__,
      std::filesystem::exists(corpus.get_extraction_path().value_or("")));

  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  prep_stmt = con->prepareStatement(GET_CORPUS_FROM_ID);
  prep_stmt->setInt(1, corpus.get_id());
  res = prep_stmt->executeQuery();
  res->next();
  Assertion::assert_equals(__FUNCTION__, new_extraction_path,
                           res->getString("extraction_path"));
  PoolDB::unborrow_from_pool(con);
}

void test_migration_not_exists() {

  Storage storage;

  try {
    storage.migrate("/tmp/je/suis/non/existant");
    Assertion::assert_throw(__FUNCTION__, "StorageMigrationException");
  } catch (StorageMigrationException &e) {
    return;
  }
}

void test_migration_already_exists() {

  Storage storage;

  try {
    storage.migrate("/tmp");
    Assertion::assert_throw(__FUNCTION__, "StorageMigrationException");
  } catch (StorageMigrationException &e) {
    return;
  }
}

void test_migration() {

  Storage storage;
  string last_storage_path = storage.get_root_folder_name();

  std::filesystem::remove_all("/tmp/harvester");
  storage.migrate("/tmp/harvester");
  Assertion::assert_false(__FUNCTION__,
                          std::filesystem::exists(last_storage_path));
  Assertion::assert_equals(__FUNCTION__, "/tmp/harvester/",
                           storage.get_root_folder_name());
  Assertion::assert_true(
      __FUNCTION__, std::filesystem::exists(storage.get_root_folder_name()));

  // storage.migrate(STORED_PATH);
}

void storage_test() {
  std::cout << std::endl << "Storage tests : " << std::endl;
  Assertion::test(test_file_destination, "test_file_destination");
  Assertion::test(test_empty_file_name, "test_empty_file_name");
  Assertion::test(test_store_one_file, "test_store_one_file");
  Assertion::test(test_export_methods, "test_export_methods");
  Assertion::test(test_export_corpus_zip, "test_export_corpus_zip");
  Assertion::test(test_migration_not_exists, "test_migration_not_exists");
  Assertion::test(test_migration_already_exists,
                  "test_migration_already_exists");
  Assertion::test(test_migration, "test_migration");
}
