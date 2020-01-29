#ifndef CREATING_FOLDER_EXCEPTION_H
#define CREATING_FOLDER_EXCEPTION_H

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

class CommandException : public runtime_error {

public:
  CommandException(string &error_message) : runtime_error(error_message) {}
};

#endif