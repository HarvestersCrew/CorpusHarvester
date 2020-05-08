#ifndef TEXT_INTEGRITY_H
#define TEXT_INTEGRITY_H

#include "indexation/file.h"
#include "middleware/file_middleware.h"
#include <memory>

using std::shared_ptr;

/**
 * TextIntegrity class is a middleware in charge of checking a text file
 * integrity
 */
class TextIntegrity : public FileMiddleware {

public:
  bool exec_on_file(shared_ptr<File> file);
};

#endif
