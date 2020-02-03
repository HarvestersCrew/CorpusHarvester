#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

class TestFailedException {
  string _functionName;
  string _expected;
  string _found;

public:
  TestFailedException(string functionName, string expected, string found)
      : _functionName(functionName), _expected(expected), _found(found) {}

  string what() const throw() {
    string what = "[ERROR] Test of function " + _functionName +
                  "() failed : expected [" + _expected + "], got [" + _found +
                  "]";
    return what;
  }
};

class CommandException : public runtime_error {

public:
  CommandException(string &error_message) : runtime_error(error_message) {}
};

class api_no_setting_exception : public std::exception {
public:
  api_no_setting_exception(std::string key);
  const char *what() const throw();

private:
  std::string _msg;
};

class api_missing_settings_exception : public std::exception {
public:
  const char *what() const throw();
};

#endif
