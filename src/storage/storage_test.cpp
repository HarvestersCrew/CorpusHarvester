#include <iostream>
#include <sys/stat.h>

#include "storage/command_exception.h"
#include "storage/storage.h"

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char const *argv[]) {
  system("/project/init_storage_data 100");
  string storageRoot = "/tmp/";
  Storage storage(storageRoot + "stored");
  try {
    cout << storage.fileDestination("tweet2", "tweeter") << endl;
    storage.storeFile(storageRoot + "storage_data/tweet3", "tweet3", "tweeter");
  } catch (const CommandException &e) {
    cerr << e.what() << endl;
  }
  return 0;
}
