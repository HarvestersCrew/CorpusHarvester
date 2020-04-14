#include "storage/zip_export.h"

string ZipExport::compressed_export(std::list<shared_ptr<File>> files,
                                    string archive_name) {
  Storage storage;
  int error;
  string archive_path =
      storage.get_corpus_path() + archive_name + "-" + random_str(6) + ".zip";
  zip_t *corpus = zip_open(archive_path.c_str(), ZIP_EXCL | ZIP_CREATE, &error);
  if (error == ZIP_ER_EXISTS) {
    return archive_path;
  }
  zip_source_t *file_source;
  string storage_file_path;
  string file_path;
  for (auto &file : files) {
    file_path = file->get_full_path();
    storage_file_path = storage.get_root_folder_name() + file_path;
    file_source = zip_source_file(corpus, storage_file_path.c_str(), 0, -1);
    if (file_source != NULL) {
      if (zip_file_add(corpus, file_path.c_str(), file_source, 0) < 0) {
        zip_source_free(file_source);
      }
    }
  }
  zip_close(corpus);
  return archive_path;
}
