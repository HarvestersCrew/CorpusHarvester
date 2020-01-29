#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "storage/command_exception.h"
#include "storage/storage.h"

Storage::Storage(const string root_folder_name)
    : _root_folder_name((root_folder_name[root_folder_name.length()] == '/')
                            ? root_folder_name
                            : root_folder_name + "/") {}

int Storage::createFoldersInRoot(string &folder_path) const {
  string dest_folder_name = _root_folder_name + folder_path;
  string mkpath_cmd = "mkdir -p " + dest_folder_name;
  if (system(mkpath_cmd.c_str()) == -1) {
    return 0;
  } else {
    return 1;
  }
}

bool Storage::folderExistsInRoot(string &folder_path) const {
  struct stat buffer;
  string dest = _root_folder_name + folder_path;
  return (stat(dest.c_str(), &buffer) == 0);
}

string Storage::fileDestination(string file_name, string api_name) const {
  string dest_folder_name = api_name + "/" + file_name[0] + "/";
  string dest_folder_path = _root_folder_name + dest_folder_name;
  bool dest_folder_exists = folderExistsInRoot(dest_folder_name);
  if (!dest_folder_exists) {
    int correctly_created = createFoldersInRoot(dest_folder_name);
    if (!correctly_created) {
      string error_message = "Error creating : " + dest_folder_path;
      throw CommandException(error_message);
    }
  }
  return dest_folder_path + file_name;
}

string Storage::storeFile(string file_path, string file_name,
                          string api_name) const {
  string file_destination = fileDestination(file_name, api_name);
  string move_file_cmd = "mv " + file_path + " " + file_destination;
  if (system(move_file_cmd.c_str()) == -1) {
    string error_message =
        "Error moving : " + file_path + " to " + file_destination;
    throw CommandException(error_message);
  }
  return file_destination;
}
