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
string TEST_FILES_PATH = STORAGE_ROOT + "storage_data/";

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

void testStoreOneFile() {
  system("/project/init_storage_data 100");
  File file(TEST_FILES_PATH + "tweet3", "tweet3", 200, "tweeter");
  storage.storeFile(file);
  string expectedDestination = STORED_PATH + "tweeter/t/tweet3";
  Assertion::assertEquals(__FUNCTION__, expectedDestination, file.getPath());
  string ls = exec("ls " + expectedDestination);
  Assertion::assertEquals(__FUNCTION__, expectedDestination + "\n", ls);
}

int main(int argc, char const *argv[]) {
  cout << endl << "Storage tests : " << endl;
  try {
    Assertion::test(testFileDestination, "testFileDestination");
    Assertion::test(testStoreOneFile, "testStoreFiles");
  } catch (const TestFailedException &e) {
    cerr << e.what() << endl;
  } catch (const CommandException &e) {
    cerr << e.what() << endl;
  }
  return 0;
}
