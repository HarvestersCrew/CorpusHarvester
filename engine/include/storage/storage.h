#ifndef STORAGE_H
#define STORAGE_H

#include <string>

using std::string;

/**
 * Storage class provides methods to store the files in the file system
 */
class Storage {

private:
  /**
   * The name of the root folder where the files are stored
   */
  string _root_folder_name;

  /**
   * Creates the missing folder in the given path from the root folder
   * @param folder_path the path to create from the root folder
   * @return 0 if something went wrong, 1 otherwise
   */
  int create_folders_in_root(string &folder_path) const;

  /**
   * Checks if the given folder path exists fro mthe root folder
   * @param folder_path the path to create from the root folder
   * @return true if exists, false otherwise
   */
  bool folder_exists_in_root(string &folder_path) const;

public:
  /**
   * Creates a Storage object
   * @param root_folder_name the name of the root folder where the files are
   * stored
   */
  Storage(const string root_folder_name);

  /**
   * Given the name of a file and its source (api) name, finds destination of
   * this file in the storage
   * @param file_name the name of the file
   * @param api_name the source where this file was downloaded
   * @return the destination path of the file in the storage
   */
  string file_destination(string file_name, string api_name) const;

  /**
   * Moves the file from its current path to its destination in the storage
   * @param file the file to move
   */
  void move_file(File &file) const;

  /**
   * Creates a file in the storage
   * @param fileContent the content of the file
   * @param file the file metadata
   */
  void store_file(string fileContent, File &file) const;

  string get_root_folder_name() const { return _root_folder_name; }
  void set_root_folder_name(string root_folder_name) {
    _root_folder_name = root_folder_name;
  }
};

#endif