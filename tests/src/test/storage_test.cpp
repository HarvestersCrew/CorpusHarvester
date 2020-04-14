#include "test/storage_test.h"

Storage storage;
std::string STORED_PATH = storage.get_root_folder_name();
std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";
shared_ptr<File> file =
    std::make_shared<File>(File("null", "storage", 100, "twitter", ".txt"));

void test_file_destination() {
  std::string fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH + "download/twitter/ddecebde/a58b5f26/4d27f1f7/909bab74.txt",
      fileDest);
  file->set_source("wikicommons");
  file->set_name("harvester");
  file->set_format(".jpg");
  fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH +
          "download/wikicommons/9f8f961b/5607b100/3cf830f4/4c67efc9.jpg",
      fileDest);
}

void test_empty_file_name() {
  file->set_name("");
  std::string fileDest = storage.file_destination(file);
  Assertion::assert_not_equals(__FUNCTION__, "", fileDest);
}

void test_store_one_file() {
  file->set_source("tmdb");
  file->set_name("test");
  file->set_format(".png");
  std::string content = "Bien le bonjour je suis un fichier de test";
  file->set_content(content);
  std::string file_dest = storage.store_file(file);
  std::string expected_destination =
      STORED_PATH + "download/tmdb/098f6bcd/4621d373/cade4e83/2627b4f6.png";
  std::string expected_path = "download/tmdb/098f6bcd/4621d373/cade4e83/";
  std::string expected_name = "2627b4f6";
  Assertion::assert_equals(__FUNCTION__, expected_destination, file_dest);
  Assertion::assert_equals(__FUNCTION__, expected_path, file->get_path());
  Assertion::assert_equals(__FUNCTION__, expected_name, file->get_name());
  std::string cat = exec("cat " + expected_destination);
  Assertion::assert_equals(__FUNCTION__, content + "\n", cat);
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
  std::list<shared_ptr<File>> files;
  files.push_back(file);
  shared_ptr<File> file2 =
      std::make_shared<File>(File("null", "test2", 100, "twitter", ".txt"));
  storage.store_file(file2);
  files.push_back(file2);
  logger::set_level(logger::DEBUG);
  for (auto &file : files) {
    file->insert(HarvesterDatabase::init());
  }
  Corpus corpus = Corpus("corpus_test", files, "something");
  corpus.insert(HarvesterDatabase::init());
  corpus.export_(ExportMethod::methods::ZIP);
  std::string new_extraction_path =
      storage.get_corpus_path() + std::to_string(corpus.get_id()) + ".zip";
  Assertion::assert_equals(__FUNCTION__, new_extraction_path,
                           corpus.get_extraction_path());
  Assertion::assert_true(__FUNCTION__,
                         std::filesystem::exists(corpus.get_extraction_path()));

  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  prep_stmt = HarvesterDatabase::init()->prepareStatement(GET_CORPUS_FROM_ID);
  prep_stmt->setInt(1, corpus.get_id());
  res = prep_stmt->executeQuery();
  res->next();
  Assertion::assert_equals(__FUNCTION__, new_extraction_path,
                           res->getString("extraction_path"));
}

void storage_test() {
  std::cout << std::endl << "Storage tests : " << std::endl;
  Assertion::test(test_file_destination, "test_file_destination");
  Assertion::test(test_empty_file_name, "test_empty_file_name");
  Assertion::test(test_store_one_file, "test_store_one_file");
  Assertion::test(test_export_methods, "test_export_methods");
  Assertion::test(test_export_corpus_zip, "test_export_corpus_zip");
  HarvesterDatabase::close();
}
