#ifndef EXPORT_METHOD_H
#define EXPORT_METHOD_H

#include "indexation/file.h"
#include <cppconn/connection.h>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>
#include <zip.h>

using std::ofstream;
using std::shared_ptr;
using std::string;
using std::unordered_map;

/**
 * ExportMethod interface describes the strategy used to export a corpus
 */
class ExportMethod {

public:
  enum methods { ZIP };

private:
  static unordered_map<methods, shared_ptr<ExportMethod>> available_methods;

public:
  /**
   * Adds a file in the archive
   * @param storage_file_path the path of the file in the file system
   * @param file_path the path of the file in the archive
   */
  virtual void add_file(string storage_file_path, string file_path) = 0;

  /**
   * Exports files to a given path
   * @param files list of files to put into the exported archive
   * @param archive_name name of the archive
   * @return string path to the exported file
   */
  virtual string compressed_export(std::list<shared_ptr<File>> files,
                                   string archive_name) = 0;

  /**
   * Inits the given ExtractionApiTags with the Apis of the exported files
   * @param api_tags the variable to fill
   * @param files the exported files
   */
  void init_api_tags(ExtractionApiTags &api_tags,
                     std::list<shared_ptr<File>> &files);

  /**
   * Gets a CSV line representing the Apis and their tags name contained in the
   * given ExtractionApiTags return the CSV line representing the Apis and their
   * tags name
   */
  string get_api_tags_titles(ExtractionApiTags api_tags);

  /**
   * Gets a CSV line representing the Apis and their tags value contained in the
   * given ExtractionApiTags
   * @param api_tags the apis with their tags
   * @param source the source of the file which filled the tags values
   * return the CSV line representing the Apis and their tags name
   */
  string get_api_tags_values(ExtractionApiTags api_tags, string source);

  /**
   * Clears the api tags values in the given ExtractionApiTags
   * @param api_tags the apis and their tags
   */
  void clear_api_tags(ExtractionApiTags &api_tags);

  /**
   * Gets the content of the metadata file baised on the given file list
   * @param files the file list
   * return the content of the metadata file
   */
  string get_metadata_content(std::list<shared_ptr<File>> files);

  /**
   * Adds a file in the archive describing the metadata of all given files
   * @param files the files
   * @param tmp_path a temporary path to create the metadata file
   */
  void add_metadata(std::list<shared_ptr<File>> files, std::string tmp_path);

  /**
   * Exports to a given type without having to create the pointer of method
   * @param files list of files to put into the exported archive
   * @param archive_name name of the archive
   * @param method method of export to use
   * @return string path to the exported file
   */
  static string compressed_export(std::list<shared_ptr<File>> files,
                                  string archive_name, methods method);
};

/**
 * ZipExport class is used to export a corpus as a zip file
 */
class ZipExport : public ExportMethod {

private:
  zip_t *_corpus;
  zip_source_t *_file_source;

public:
  virtual void add_file(string storage_file_path, string file_path);

  virtual string compressed_export(std::list<shared_ptr<File>> files,
                                   string archive_name);
};

#endif
