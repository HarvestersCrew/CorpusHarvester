#ifndef EXPORT_METHOD_H
#define EXPORT_METHOD_H

#include "indexation/file.h"
#include <cppconn/connection.h>
#include <iostream>
#include <list>
#include <memory>
#include <zip.h>

using std::shared_ptr;
using std::string;

/**
 * ExportMethod interface describes the strategy used to export a corpus
 */
class ExportMethod {

public:
  virtual string compressed_export(std::list<shared_ptr<File>> files,
                                   string archive_name) = 0;
};

#endif
