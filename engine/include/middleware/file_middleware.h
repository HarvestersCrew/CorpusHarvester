#ifndef FILE_MIDDLEWARE_H
#define FILE_MIDDLEWARE_H

#include "indexation/file.h"
#include <memory>

using std::shared_ptr;

/**
 * FileMiddleware interface provides a method to execute an action on a file
 */
class FileMiddleware {

public:
  /**
   * Executes a specific action on the given file
   * @file the file
   * @return true if the action was correctly done, false otherwise
   */
  virtual bool exec_on_file(shared_ptr<File> file) = 0;
};

#endif
