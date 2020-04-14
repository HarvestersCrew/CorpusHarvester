#include "test/storage_test.h"

Storage storage;
std::string STORED_PATH = storage.get_root_folder_name();
std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";
shared_ptr<File> file =
    std::make_shared<File>(File("", "storage", 100, "twitter", ".txt"));

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
  file->set_path("");
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
  std::string ls = exec("cat " + expected_destination);
  Assertion::assert_equals(__FUNCTION__, content + "\n", ls);
}

void test_export_corpus_zip() {
  std::list<shared_ptr<File>> files;
  files.push_back(file);
  shared_ptr<File> file2 =
      std::make_shared<File>(File("", "test2", 100, "twitter", ".txt"));
  storage.store_file(file2);
  files.push_back(file2);
  Corpus corpus = Corpus("corpus_test", files, "");
  shared_ptr<ZipExport> zip_export = std::make_shared<ZipExport>();
  string path = corpus.export_(zip_export);
  string ls = exec("ls " + path);
  Assertion::assert_equals(__FUNCTION__, path + "\n", ls);
}

void storage_test() {
  std::cout << std::endl << "Storage tests : " << std::endl;
  Assertion::test(test_file_destination, "test_file_destination");
  Assertion::test(test_empty_file_name, "test_empty_file_name");
  Assertion::test(test_store_one_file, "test_store_one_file");
  Assertion::test(test_export_corpus_zip, "test_export_corpus_zip");
  HarvesterDatabase::close();
}
