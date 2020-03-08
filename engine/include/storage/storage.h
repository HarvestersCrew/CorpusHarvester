#ifndef STORAGE_H
#define STORAGE_H

#include "indexation/file.h"
#include "indexation/setting.h"
#include "utils/exceptions.h"
#include "utils/md5.h"
#include "utils/utils.h"
#include <cppconn/connection.h>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::shared_ptr;

#define MD5_SPLIT 8

/**
 * Storage class provides methods to store the files in the file system
 */
class Storage {

private:
  /**
   * The name of the root folder where the files are stored
   */
  std::string _root_folder_name;

  /**
   * Creates the missing folder in the given path from the root folder
   * @param folder_path the path to create from the root folder
   * @return 0 if something went wrong, 1 otherwise
   */
  bool create_folders_in_root(std::string folder_path) const;

  /**
   * Checks if the given folder path exists fro mthe root folder
   * @param folder_path the path to create from the root folder
   * @return true if exists, false otherwise
   */
  bool folder_exists_in_root(std::string folder_path) const;

public:
  /**
   * Default constructor
   */
  Storage();

  /**
   * Creates a Storage object fetching settings from database
   * @param db the database
   */
  Storage(sql::Connection *db);

  void init_root(sql::Connection *db);

  std::string get_folder_path(std::string file_name) const;

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

  std::string get_root_folder_name() const { return _root_folder_name; }
  void set_root_folder_name(std::string root_folder_name) {
    _root_folder_name = root_folder_name;
  }
};

#endif
