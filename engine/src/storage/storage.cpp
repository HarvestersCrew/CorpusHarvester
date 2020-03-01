#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "storage/storage.h"

using std::cout;
using std::endl;

Storage::Storage(const string root_folder_name)
    : _root_folder_name((root_folder_name[root_folder_name.length() - 1] == '/')
                            ? root_folder_name
                            : root_folder_name + "/") {}

int Storage::create_folders_in_root(string folder_path) const {
  string dest_folder_name = _root_folder_name + folder_path;
  string mkpath_cmd = "mkdir -p " + dest_folder_name;
  if (system(mkpath_cmd.c_str()) == -1) {
    return 0;
  } else {
    return 1;
  }
}

bool Storage::folder_exists_in_root(string folder_path) const {
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

void Storage::move_file(File *file) const {
  string file_dest = file_destination(file->get_name(), file->get_source());
  string move_file_cmd = "mv " + file->get_path() + " " + file_dest;
  if (system(move_file_cmd.c_str()) == -1) {
    string error_message =
        "Error moving : " + file->get_path() + " to " + file_dest;
    throw CommandException(error_message);
  }
  file->set_path(file_dest);
}

void Storage::store_file(File *file) const {
  string file_dest = file_destination(file->get_name(), file->get_source());

  if (!file->get_binary()) {

    std::ofstream outfile(file_dest);
    outfile << file->get_content_str() << std::endl;
    outfile.close();

  } else {

    std::vector<char> vec_content = file->get_content_bin();
    char buf[vec_content.size()];
    for (size_t i = 0; i < vec_content.size(); ++i) {
      buf[i] = vec_content.at(i);
    }
    std::ofstream outfile(file_dest, std::ios::out | std::ios::binary);
    outfile.write(buf, vec_content.size());
    outfile.close();
  }

  file->set_path(file_dest);
}

void Storage::store_files(list<File *> files) const {
  for (auto &file : files) {
    store_file(file);
  }
}
