#ifndef EXPORT_METHOD_H
#define EXPORT_METHOD_H

#include "indexation/file.h"
#include <cppconn/connection.h>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>
#include <zip.h>

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
   * Exports files to a given path
   * @param files list of files to put into the exported archive
   * @param archive_name name of the archive
   * @return string path to the exported file
   */
  virtual string compressed_export(std::list<shared_ptr<File>> files,
                                   string archive_name) = 0;

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
public:
  virtual string compressed_export(std::list<shared_ptr<File>> files,
                                   string archive_name);
};

#endif
