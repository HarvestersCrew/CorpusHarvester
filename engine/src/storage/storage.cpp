#include "storage/storage.h"

Storage::Storage(sql::Connection *db) { init_root(db); }

Storage::Storage() {}

void Storage::init_root(sql::Connection *db) {
  Setting root_folder_name = Setting(Setting::STORAGE_ROOT, db);
  _root_folder_name = root_folder_name.get_value();
}

bool Storage::create_folders_in_root(std::string folder_path) const {
  std::string dest_folder_name = _root_folder_name + folder_path;
  return std::filesystem::create_directories(dest_folder_name);
}

bool Storage::folder_exists_in_root(std::string folder_path) const {
  std::string dest_folder_name = _root_folder_name + folder_path;
  return std::filesystem::exists(dest_folder_name);
}

std::string Storage::get_folder_path(std::string file_name) const {
  return add_string_every_n_chars(file_name, "/", MD5_SPLIT);
}

std::string Storage::file_destination(shared_ptr<File> file) const {
  std::string current_file_name = file->get_name();
  if (current_file_name == "") {
    current_file_name = random_str(25);
  }
  std::string md5_file_name = md5(current_file_name);
  std::string file_name =
      md5_file_name.substr(md5_file_name.length() - MD5_SPLIT, MD5_SPLIT);
  std::string file_name_for_folder =
      md5_file_name.substr(0, md5_file_name.length() - MD5_SPLIT);
  std::string file_folder = get_folder_path(file_name_for_folder);
  std::string dest_folder_path = file->get_source() + "/" + file_folder;
  file->set_name(file_name);
  file->set_path(dest_folder_path);

  bool dest_folder_exists = folder_exists_in_root(dest_folder_path);
  if (!dest_folder_exists) {
    bool correctly_created = create_folders_in_root(dest_folder_path);
    if (!correctly_created) {
      std::string error_message = "Error creating : " + dest_folder_path;
      throw CommandException(error_message);
    }
  }
  return _root_folder_name + dest_folder_path + file_name + file->get_format();
}

// void Storage::move_file(shared_ptr<File> file) const {
//   std::string file_dest = file_destination(file);
//   try {
//     std::filesystem::rename(file->get_path(), file_dest);
//   } catch (std::filesystem::filesystem_error &e) {
//     std::string error_message =
//         "Error moving : " + file->get_path() + " to " + file_dest;
//     throw CommandException(error_message);
//   }
// }

std::string Storage::store_file(shared_ptr<File> file) const {
  std::string file_dest = file_destination(file);
  file->store(file_dest);
  return file_dest;
}

void Storage::store_files(std::list<shared_ptr<File>> files) const {
  for (auto &file : files) {
    store_file(file);
  }
}
