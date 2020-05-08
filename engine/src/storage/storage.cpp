#include "storage/storage.h"

Storage::Storage() { init_root(); }

void Storage::init_root() {
  _root_folder_name = Setting(Setting::STORAGE_ROOT);
  if (!folder_exists_in_root(CORPUS_FOLDER)) {
    create_folders_in_root(CORPUS_FOLDER);
  }
}

bool Storage::delete_file_in_root(std::string file_path) const {
  std::string dest_file_name = _root_folder_name.get_value() + file_path;
  return std::filesystem::remove(dest_file_name);
}

bool Storage::create_folders_in_root(std::string folder_path) const {
  std::string dest_folder_name = _root_folder_name.get_value() + folder_path;
  return std::filesystem::create_directories(dest_folder_name);
}

bool Storage::folder_exists_in_root(std::string folder_path) const {
  std::string dest_folder_name = _root_folder_name.get_value() + folder_path;
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
  std::string file_name_for_folders =
      md5_file_name.substr(0, MD5_SPLIT * FOLDER_TREE_HEIGHT);
  std::string file_folders = get_folder_path(file_name_for_folders);
  std::string dest_folder_path =
      DOWNLOAD_FOLDER + file->get_source() + "/" + file_folders;
  std::string file_name = md5_file_name.substr(MD5_SPLIT * FOLDER_TREE_HEIGHT);
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
  return _root_folder_name.get_value() + dest_folder_path + file_name + "." +
         file->get_format();
}

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

void Storage::migrate(std::string new_path) {
  auto path_parsed = std::filesystem::path(new_path);
  if (path_parsed.is_relative()) {
    throw StorageMigrationException("Can't move to a relative path");
  }
  if (!std::filesystem::exists(path_parsed.parent_path())) {
    throw StorageMigrationException(
        "Given parent folder path for storage migration does not exist ");
  }
  if (std::filesystem::exists(path_parsed)) {
    throw StorageMigrationException("Destination folder must not exist");
  }
  string new_storage_path = new_path + "/";
  std::filesystem::rename(_root_folder_name.get_value(), new_storage_path);
  _root_folder_name.set_value(new_storage_path);
  _root_folder_name.update();
  logger::info("Storage root migrated to " + new_storage_path);
}

void Storage::delete_corpus(string extraction_path) {
  if (extraction_path == "") {
    throw ExtractionPathMissingException();
  }
  if (!delete_file_in_root(CORPUS_FOLDER + extraction_path)) {
    throw StorageFileDeletionException("Corpus deletion failed with path : " +
                                       get_corpus_path() + extraction_path);
  }
}
