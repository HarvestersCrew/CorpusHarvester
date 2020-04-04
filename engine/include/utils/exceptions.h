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
    std::string what = " {{ ERROR }} Test of function " + _functionName +
                       "() failed : expected [" + _expected + "], got [" +
                       _found + "]";
    return what;
  }
};

class SettingNotFoundException : public std::runtime_error {

public:
  SettingNotFoundException(std::string &name)
      : std::runtime_error("Setting with name '" + name +
                           "' was not found in the database") {}
};

class CommandException : public std::runtime_error {

public:
  CommandException(std::string &error_message)
      : std::runtime_error(error_message) {}
};

class ClosedDatabaseException : public std::runtime_error {

public:
  ClosedDatabaseException() : std::runtime_error("Database is closed") {}
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

class logger_exception : public std::runtime_error {

public:
  logger_exception(std::string error_message)
      : std::runtime_error(error_message) {}
};

class cli_parser_exception : public std::runtime_error {
public:
  cli_parser_exception(std::string error_message)
      : std::runtime_error(error_message) {}
};

class cli_parser_bad_parse_exception : public std::runtime_error {
public:
  cli_parser_bad_parse_exception(std::string error_message)
      : std::runtime_error(error_message) {}
};

class cli_parser_help_asked_exception : public std::runtime_error {
public:
  cli_parser_help_asked_exception()
      : std::runtime_error("User asked for help on CLI") {}
};

class api_default_not_in_schema : public std::runtime_error {
public:
  api_default_not_in_schema(std::string param)
      : std::runtime_error("Default parameter '" + param +
                           "' not found in schema") {}
};

class api_twice_same_name : public std::runtime_error {
public:
  api_twice_same_name(std::string name)
      : std::runtime_error("Another '" + name + "' already exists") {}
};

class api_factory_name_not_found : public std::runtime_error {
public:
  api_factory_name_not_found(std::string name)
      : std::runtime_error("API '" + name + "' not found by API discover") {}
};

class logger_not_started_exception : public std::runtime_error {
public:
  logger_not_started_exception() : std::runtime_error("Logger not started") {}
};

#endif
