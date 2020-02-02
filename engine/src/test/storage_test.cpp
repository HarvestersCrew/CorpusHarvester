#include <iostream>
#include <sys/stat.h>

#include "indexation/file.h"
#include "storage/storage.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

using std::cerr;
using std::cout;
using std::endl;

string STORAGE_ROOT = "/tmp/";
string STORED_PATH = STORAGE_ROOT + "stored/";
string TEMP_FILES_PATH = STORAGE_ROOT + "storage_data/";

Storage storage(STORED_PATH);

void testFileDestination() {
  string fileDest = storage.fileDestination("imafilefrom", "tweeter");
  Assertion::assertEquals(__FUNCTION__, STORED_PATH + "tweeter/i/imafilefrom",
                          fileDest);
  fileDest = storage.fileDestination("andiamanotherfilefrom", "wikicommons");
  Assertion::assertEquals(__FUNCTION__,
                          STORED_PATH + "wikicommons/a/andiamanotherfilefrom",
                          fileDest);
}

void testMoveOneFile() {
  system("/project/init_storage_data 100");
  File file(TEMP_FILES_PATH + "tweet3", "tweet3", 200, "tweeter");
  storage.moveFile(file);
  string expectedDestination = STORED_PATH + "tweeter/t/tweet3";
  Assertion::assertEquals(__FUNCTION__, expectedDestination, file.getPath());
  string ls = exec("ls " + expectedDestination);
  Assertion::assertEquals(__FUNCTION__, expectedDestination + "\n", ls);
}

void testStoreOneFile() {
  File file("", "jesuisunfichierquivientde", 200, "tmdb");
  storage.storeFile("Bien le bonjour je suis un fichier de test", file);
  string expectedDestination = STORED_PATH + "tmdb/j/jesuisunfichierquivientde";
  Assertion::assertEquals(__FUNCTION__, expectedDestination, file.getPath());
  string ls = exec("ls " + expectedDestination);
  Assertion::assertEquals(__FUNCTION__, expectedDestination + "\n", ls);
}

// // TODO :: Need to uncomment in order to start the test !
// // TODO :: Change the way we start the test
// int main(int argc, char const *argv[]) {
//   cout << endl << "Storage tests : " << endl;
//   try {
//     Assertion::test(testFileDestination, "testFileDestination");
//     Assertion::test(testMoveOneFile, "testMoveOneFile");
//     Assertion::test(testStoreOneFile, "testStoreOneFile");
//   } catch (const TestFailedException &e) {
//     cerr << e.what() << endl;
//   } catch (const CommandException &e) {
//     cerr << e.what() << endl;
//   }
//   return 0;
// }
