#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class TestFailedException {
  std::string _functionName;
  std::string _expected;
  std::string _found;

public:
  TestFailedException(std::string functionName, std::string expected,
                      std::string found)
      : _functionName(functionName), _expected(expected), _found(found) {}

  std::string what() const throw() {
    std::string what = "[ERROR] Test of function " + _functionName +
                       "() failed : expected [" + _expected + "], got [" +
                       _found + "]";
    return what;
  }
};

class CommandException : public std::runtime_error {

public:
  CommandException(std::string &error_message)
      : std::runtime_error(error_message) {}
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

class api_unrecognized_settings_exception : public std::exception {
private:
  std::string _msg;

public:
  api_unrecognized_settings_exception()
      : _msg("An API setting was unrecognized.") {}
  api_unrecognized_settings_exception(const std::string &key,
                                      const std::string &val)
      : _msg("An API setting value was unrecognized: " + key + " = " + val) {}
  const char *what() const throw();
};

class download_no_200_exception : public std::exception {
private:
  std::string _msg;

public:
  download_no_200_exception() : _msg("HTTP call didn't succeed.") {}
  download_no_200_exception(int http_code)
      : _msg("HTTP call didn't succeed: " + http_code) {}
  const char *what() const throw();
};

#endif
