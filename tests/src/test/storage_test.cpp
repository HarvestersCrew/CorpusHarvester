#include "test/storage_test.h"

sql::Connection *storage_db = HarvesterDatabase::init();
Storage storage = Storage(storage_db);
std::string STORED_PATH = storage.get_root_folder_name();
std::string TEMP_FILES_PATH = STORED_PATH + "storage_data/";
shared_ptr<File> file =
    std::make_shared<File>(File("", "storage", 100, "twitter", ".txt"));

void test_file_destination() {
  std::string fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH + "twitter/ddecebde/a58b5f26/4d27f1f7/909bab74.txt",
      fileDest);
  file->set_source("wikicommons");
  file->set_name("harvester");
  file->set_format(".jpg");
  fileDest = storage.file_destination(file);
  Assertion::assert_equals(
      __FUNCTION__,
      STORED_PATH + "wikicommons/9f8f961b/5607b100/3cf830f4/4c67efc9.jpg",
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
      STORED_PATH + "tmdb/098f6bcd/4621d373/cade4e83/2627b4f6.png";
  std::string expected_path = "tmdb/098f6bcd/4621d373/cade4e83/";
  std::string expected_name = "2627b4f6";
  Assertion::assert_equals(__FUNCTION__, expected_destination, file_dest);
  Assertion::assert_equals(__FUNCTION__, expected_path, file->get_path());
  Assertion::assert_equals(__FUNCTION__, expected_name, file->get_name());
  std::string ls = exec("cat " + expected_destination);
  Assertion::assert_equals(__FUNCTION__, content + "\n", ls);
}

std::pair<int, int> storage_test() {
  int function_count = 3;
  void (*test_functions[])(void) = {test_file_destination, test_empty_file_name,
                                    test_store_one_file};
  std::string test_functions_name[] = {
      "test_file_destination", "test_empty_file_name", "test_store_one_file"};

  std::cout << std::endl << "Storage tests : " << std::endl;
  return Assertion::test_all(test_functions, test_functions_name,
                             function_count);
}
