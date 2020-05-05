#ifndef STORAGE_H
#define STORAGE_H

#include "database/setting.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include "utils/md5.h"
#include "utils/utils.h"
#include <cppconn/connection.h>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::shared_ptr;

#define DOWNLOAD_FOLDER "download/"
#define CORPUS_FOLDER "corpus/"
#define FOLDER_TREE_HEIGHT 2
#define MD5_SPLIT 2
// #define MD5_SPLIT 8
// #define MD5_SPLIT 3

/**
 * Storage class provides methods to store the files in the file system
 */
class Storage {

private:
  /**
   * The name of the root folder where the files are stored
   */
  Setting _root_folder_name;

public:
  /**
   * Creates a Storage object fetching settings from database
   */
  Storage();

  void init_root();

  std::string get_folder_path(std::string file_name) const;

  /**
   * Deletes a file given its path
   * @param file_path the path of the file
   * @return true if the file was deleted, false otherwise
   */
  bool delete_file_in_root(std::string file_path) const;

  /**
   * Creates the missing folder in the given path from the root folder
   * @param folder_path the path to create from the root folder
   * @return true if the folders were deleted, false otherwise
   */
  bool create_folders_in_root(std::string folder_path) const;

  /**
   * Checks if the given folder path exists fro mthe root folder
   * @param folder_path the path to create from the root folder
   * @return true if exists, false otherwise
   */
  bool folder_exists_in_root(std::string folder_path) const;

  /**
   * Given the name of a file and its source (api) name, finds destination of
   * this file in the storage
   * @param file_name the name of the file
   * @param api_name the source where this file was downloaded
   * @return the destination path of the file in the storage
   */
  std::string file_destination(shared_ptr<File> file) const;

  // /**
  //  * Moves the file from its current path to its destination in the storage
  //  * @param file the file to move
  //  */
  // void move_file(shared_ptr<File> file) const;

  /**
   * Creates a file in the storage
   * @param file the file to store
   * @return the destination of the file
   */
  std::string store_file(shared_ptr<File> file) const;

  /**
   * Creates the files in the storage
   * @param files the files to store
   */
  void store_files(std::list<shared_ptr<File>> files) const;

  /**
   * Moves the root folder of the storage in another folder
   * @param new_path the new folder of the storage
   * @throw StorageMigrationException if new path can't be used
   */
  void migrate(std::string new_path);

  /**
   * Deletes the archive corresponding to the given corpus in the storage
   * @param extraction_path the extraction_path of the corpus to delete
   * @throw StorageFileDeletionException if something went wrong during deletion
   */
  void delete_corpus(string extraction_path);

  std::string get_corpus_path() const {
    return _root_folder_name.get_value() + CORPUS_FOLDER;
  }
  std::string get_root_folder_name() const {
    return _root_folder_name.get_value();
  }
};

#endif
