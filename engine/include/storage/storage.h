#ifndef STORAGE_H
#define STORAGE_H

#include <string>

using std::string;

class Storage {

private:
  string _root_folder_name;

  int createFoldersInRoot(string &folder_path) const;

  bool folderExistsInRoot(string &folder_path) const;

public:
  Storage(const string root_folder_name);

  string fileDestination(string file_name, string api_name) const;

  void moveFile(File &file) const;

  void storeFile(string fileContent, File &file) const;

  string getRootFolderName() const { return _root_folder_name; }
  void setRootFolderName(string root_folder_name) {
    _root_folder_name = root_folder_name;
  }
};

#endif