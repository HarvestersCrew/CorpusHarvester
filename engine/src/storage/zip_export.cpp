#include "storage/export_method.h"
#include "storage/storage.h"

void ZipExport::add_file(string storage_file_path, string file_path) {
  _file_source = zip_source_file(_corpus, storage_file_path.c_str(), 0, -1);
  if (_file_source != NULL) {
    if (zip_file_add(_corpus, file_path.c_str(), _file_source, 0) < 0) {
      zip_source_free(_file_source);
    }
  }
}

string ZipExport::compressed_export(std::list<shared_ptr<File>> files,
                                    string archive_name) {
  Storage storage;
  string corpus_path = storage.get_corpus_path();
  int error;
  string archive_path = corpus_path + archive_name + ".zip";
  _corpus = zip_open(archive_path.c_str(), ZIP_EXCL | ZIP_CREATE, &error);
  if (error == ZIP_ER_EXISTS) {
    return archive_name + ".zip";
  }
  add_metadata(files, corpus_path);
  string storage_file_path;
  string file_path;
  for (auto &file : files) {
    file_path = file->get_full_path();
    storage_file_path = storage.get_root_folder_name() + file_path;
    add_file(storage_file_path, file_path);
  }
  zip_close(_corpus);
  return archive_name + ".zip";
}
