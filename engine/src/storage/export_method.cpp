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

void ExportMethod::add_metadata(std::list<shared_ptr<File>> files,
                                std::string tmp_path) {
  string metadata_name = "metadata.txt";
  string metadata_path = tmp_path + metadata_name;
  ofstream metadata(metadata_path);
  if (!metadata.is_open()) {
    return;
  }
  for (auto &file : files) {
    metadata << file->get_extraction_metadata() << std::endl;
  }
  metadata.close();
  add_file(metadata_path, metadata_name);
}
