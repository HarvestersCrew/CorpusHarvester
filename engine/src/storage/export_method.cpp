#include "storage/export_method.h"

unordered_map<ExportMethod::methods, shared_ptr<ExportMethod>>
    ExportMethod::available_methods = {
        {ExportMethod::methods::ZIP, shared_ptr<ZipExport>(new ZipExport())}};

string ExportMethod::compressed_export(std::list<shared_ptr<File>> files,
                                       string archive_name,
                                       ExportMethod::methods method) {
  return ExportMethod::available_methods.at(method)->compressed_export(
      files, archive_name);
}
