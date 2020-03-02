#include "storage/storage.h"

Storage::Storage(const std::string root_folder_name)
    : _root_folder_name((root_folder_name[root_folder_name.length() - 1] == '/')
                            ? root_folder_name
                            : root_folder_name + "/") {}

int Storage::create_folders_in_root(std::string folder_path) const {
  std::string dest_folder_name = _root_folder_name + folder_path;
  std::string mkpath_cmd = "mkdir -p " + dest_folder_name;
  if (system(mkpath_cmd.c_str()) == -1) {
    return 0;
  } else {
    return 1;
  }
}

bool Storage::folder_exists_in_root(std::string folder_path) const {
  struct stat buffer;
  std::string dest = _root_folder_name + folder_path;
  return (stat(dest.c_str(), &buffer) == 0);
}

std::string Storage::file_destination(std::string file_name,
                                      std::string api_name) const {
  std::string dest_folder_name = api_name + "/" + file_name[0] + "/";
  std::string dest_folder_path = _root_folder_name + dest_folder_name;
  bool dest_folder_exists = folder_exists_in_root(dest_folder_name);
  if (!dest_folder_exists) {
    int correctly_created = create_folders_in_root(dest_folder_name);
    if (!correctly_created) {
      std::string error_message = "Error creating : " + dest_folder_path;
      throw CommandException(error_message);
    }
  }
  return dest_folder_path + file_name;
}

void Storage::move_file(File *file) const {
  std::string file_dest =
      file_destination(file->get_name(), file->get_source());
  std::string move_file_cmd = "mv " + file->get_path() + " " + file_dest;
  if (system(move_file_cmd.c_str()) == -1) {
    std::string error_message =
        "Error moving : " + file->get_path() + " to " + file_dest;
    throw CommandException(error_message);
  }
  file->set_path(file_dest);
}

void Storage::store_file(File *file) const {
  std::string file_dest =
      file_destination(file->get_name(), file->get_source());
  file->store(file_dest);
  file->set_path(file_dest);
}

void Storage::store_files(std::list<File *> files) const {
  for (auto &file : files) {
    store_file(file);
  }
}
