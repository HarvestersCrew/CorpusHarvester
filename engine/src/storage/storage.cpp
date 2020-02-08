#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "indexation/file.h"
#include "storage/storage.h"
#include "utils/exceptions.h"

using std::cout;
using std::endl;

Storage::Storage(const string root_folder_name)
    : _root_folder_name((root_folder_name[root_folder_name.length() - 1] == '/')
                            ? root_folder_name
                            : root_folder_name + "/") {}

int Storage::create_folders_in_root(string &folder_path) const {
  string dest_folder_name = _root_folder_name + folder_path;
  string mkpath_cmd = "mkdir -p " + dest_folder_name;
  if (system(mkpath_cmd.c_str()) == -1) {
    return 0;
  } else {
    return 1;
  }
}

bool Storage::folder_exists_in_root(string &folder_path) const {
  struct stat buffer;
  string dest = _root_folder_name + folder_path;
  return (stat(dest.c_str(), &buffer) == 0);
}

string Storage::file_destination(string file_name, string api_name) const {
  string dest_folder_name = api_name + "/" + file_name[0] + "/";
  string dest_folder_path = _root_folder_name + dest_folder_name;
  bool dest_folder_exists = folder_exists_in_root(dest_folder_name);
  if (!dest_folder_exists) {
    int correctly_created = create_folders_in_root(dest_folder_name);
    if (!correctly_created) {
      string error_message = "Error creating : " + dest_folder_path;
      throw CommandException(error_message);
    }
  }
  return dest_folder_path + file_name;
}

void Storage::move_file(File &file) const {
  string file_dest = file_destination(file.getName(), file.getSource());
  string move_file_cmd = "mv " + file.getPath() + " " + file_dest;
  if (system(move_file_cmd.c_str()) == -1) {
    string error_message =
        "Error moving : " + file.getPath() + " to " + file_dest;
    throw CommandException(error_message);
  }
  file.setPath(file_dest);
}

void Storage::store_file(string fileContent, File &file) const {
  string file_dest = file_destination(file.getName(), file.getSource());
  string create_file_cmd = "touch " + file_dest;
  if (system(create_file_cmd.c_str()) == -1) {
    string error_message = "Error creating : " + file_dest;
    throw CommandException(error_message);
  }
  file.setPath(file_dest);
}
