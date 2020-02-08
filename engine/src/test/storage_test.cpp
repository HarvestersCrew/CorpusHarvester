#include "test/storage_test.h"

string STORAGE_ROOT = "/tmp/";
string STORED_PATH = STORAGE_ROOT + "stored/";
string TEMP_FILES_PATH = STORAGE_ROOT + "storage_data/";

Storage storage(STORED_PATH);

void test_file_destination() {
  string fileDest = storage.file_destination("imafilefrom", "tweeter");
  Assertion::assertEquals(__FUNCTION__, STORED_PATH + "tweeter/i/imafilefrom",
                          fileDest);
  fileDest = storage.file_destination("andiamanotherfilefrom", "wikicommons");
  Assertion::assertEquals(__FUNCTION__,
                          STORED_PATH + "wikicommons/a/andiamanotherfilefrom",
                          fileDest);
}

void testMoveOneFile() {
  system("/project/script/init_storage_data 100");
  File *file = new File(TEMP_FILES_PATH + "tweet3", "tweet3", 200, "tweeter");
  storage.move_file(file);
  string expectedDestination = STORED_PATH + "tweeter/t/tweet3";
  Assertion::assertEquals(__FUNCTION__, expectedDestination, file->getPath());
  string ls = exec("ls " + expectedDestination);
  Assertion::assertEquals(__FUNCTION__, expectedDestination + "\n", ls);
}

void testStoreOneFile() {
  File *file = new File("", "jesuisunfichierquivientde", 200, "tmdb");
  string content = "Bien le bonjour je suis un fichier de test";
  file->setContent(content);
  storage.store_file(file);
  string expectedDestination = STORED_PATH + "tmdb/j/jesuisunfichierquivientde";
  Assertion::assertEquals(__FUNCTION__, expectedDestination, file->getPath());
  string ls = exec("cat " + expectedDestination);
  Assertion::assertEquals(__FUNCTION__, content + "\n", ls);
}

// // TODO :: Need to uncomment in order to start the test !
// // TODO :: Change the way we start the test
void storage_test() {
  cout << endl << "Storage tests : " << endl;
  try {
    Assertion::test(test_file_destination, "test_file_destination");
    Assertion::test(testMoveOneFile, "testMoveOneFile");
    Assertion::test(testStoreOneFile, "testStoreOneFile");
  } catch (const TestFailedException &e) {
    cerr << e.what() << endl;
  } catch (const CommandException &e) {
    cerr << e.what() << endl;
  }
}
