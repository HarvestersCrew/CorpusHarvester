#include "test/storage_test.h"

string STORAGE_ROOT = "/tmp/";
string STORED_PATH = STORAGE_ROOT + "stored/";
string TEMP_FILES_PATH = STORAGE_ROOT + "storage_data/";

Storage storage(STORED_PATH);

void test_file_destination() {
  string fileDest = storage.file_destination("imafilefrom", "tweeter");
  Assertion::assert_equals(__FUNCTION__, STORED_PATH + "tweeter/i/imafilefrom",
                           fileDest);
  fileDest = storage.file_destination("andiamanotherfilefrom", "wikicommons");
  Assertion::assert_equals(__FUNCTION__,
                           STORED_PATH + "wikicommons/a/andiamanotherfilefrom",
                           fileDest);
}

void test_move_one_file() {
  system(INIT_DATA);
  File *file = new File(TEMP_FILES_PATH + "tweet3", "tweet3", 200, "tweeter");
  storage.move_file(file);
  string expectedDestination = STORED_PATH + "tweeter/t/tweet3";
  Assertion::assert_equals(__FUNCTION__, expectedDestination, file->get_path());
  string ls = exec("ls " + expectedDestination);
  Assertion::assert_equals(__FUNCTION__, expectedDestination + "\n", ls);
}

void test_store_one_file() {
  File *file = new File("", "jesuisunfichierquivientde", 200, "tmdb");
  string content = "Bien le bonjour je suis un fichier de test";
  file->set_content(content);
  storage.store_file(file);
  string expectedDestination = STORED_PATH + "tmdb/j/jesuisunfichierquivientde";
  Assertion::assert_equals(__FUNCTION__, expectedDestination, file->get_path());
  string ls = exec("cat " + expectedDestination);
  Assertion::assert_equals(__FUNCTION__, content + "\n", ls);
}

void storage_test() {
  cout << endl << "Storage tests : " << endl;
  try {
    Assertion::test(test_file_destination, "test_file_destination");
    Assertion::test(test_move_one_file, "test_move_one_file");
    Assertion::test(test_store_one_file, "test_store_one_file");
  } catch (const TestFailedException &e) {
    cerr << e.what() << endl;
  } catch (const CommandException &e) {
    cerr << e.what() << endl;
  }
}
