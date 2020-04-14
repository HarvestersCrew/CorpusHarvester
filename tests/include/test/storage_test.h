#ifndef STORAGE_TEST_H
#define STORAGE_TEST_H

#include "indexation/file.h"
#include "indexation/indexer.h"
#include "storage/storage.h"
#include "storage/zip_export.h"
#include "test/assertion.h"
#include "utils/exceptions.h"
#include "utils/utils.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <sys/stat.h>

void test_file_destination();
void test_move_one_file();
void test_store_one_file();
void test_export_corpus_zip();
void storage_test();

#endif
