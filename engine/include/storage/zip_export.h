#ifndef ZIP_EXPORT_H
#define ZIP_EXPORT_H

#include <iostream>
#include <list>
#include <memory>

#include "export_method.h"
#include "indexation/file.h"
#include "storage/storage.h"

using std::shared_ptr;
using std::string;

/**
 * ZipExport class is used to export a corpus as a zip file
 */
class ZipExport : public ExportMethod {

public:
  ZipExport(sql::Connection *db) : ExportMethod(db) {}

  string compressed_export(std::list<shared_ptr<File>> files,
                           string archive_name);
};

#endif
