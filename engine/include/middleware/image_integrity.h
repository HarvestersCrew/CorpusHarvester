#ifndef IMAGE_INTEGRITY_H
#define IMAGE_INTEGRITY_H

#include "indexation/file.h"
#include "middleware/file_middleware.h"
#include <memory>

using std::shared_ptr;

/**
 * ImageIntegrity class is a middleware in charge of checking an image file
 * integrity
 */
class ImageIntegrity : public FileMiddleware {

public:
  bool exec_on_file(shared_ptr<File> file);
};

#endif
