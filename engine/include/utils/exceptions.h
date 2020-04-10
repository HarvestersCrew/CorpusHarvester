#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::to_string;

class TestFailedException {
  std::string _functionName;
  std::string _expected;
  std::string _found;
  int _line;

public:
  TestFailedException(std::string functionName, std::string expected,
                      std::string found, int line = -1)
      : _functionName(functionName), _expected(expected), _found(found),
        _line(line) {}

  std::string what() const throw() {
    std::string what = " {{ ERROR }} Test of function " + _functionName +
                       "() failed : expected [" + _expected + "], got [" +
                       _found + "]";
    if (_line != -1) {
      what += " at line " + to_string(_line);
    }
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

class api_no_setting_exception : public std::runtime_error {
public:
  api_no_setting_exception(const std::string &key)
      : std::runtime_error("Given setting key not existing in API: " + key) {}
};

class api_missing_settings_exception : public std::runtime_error {
public:
  api_missing_settings_exception(const std::string &setting)
      : std::runtime_error("Missing setting to call an API: " + setting) {}
};

class api_filetype_incompatible : public std::runtime_error {
public:
  api_filetype_incompatible(const std::string &type)
      : std::runtime_error("Can't manage filetype: " + type) {}
};

class api_unrecognized_settings_exception : public std::runtime_error {
public:
  api_unrecognized_settings_exception()
      : std::runtime_error("An API setting was unrecognized") {}
  api_unrecognized_settings_exception(const std::string &key,
                                      const std::string &val)
      : std::runtime_error("An API setting value was unrecognized: " + key +
                           " = " + val) {}
};

class download_no_200_exception : public std::runtime_error {
public:
  download_no_200_exception()
      : std::runtime_error("HTTP call didn't succeed") {}
  download_no_200_exception(int http_code)
      : std::runtime_error("HTTP call didn't succeed: " +
                           to_string(http_code)) {}
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

class api_request_builder_invalid_type : public std::runtime_error {
public:
  api_request_builder_invalid_type(int val)
      : std::runtime_error("Invalid API type: " + to_string(val)) {}
};

class api_parameter_incompatible_value : public std::runtime_error {
public:
  api_parameter_incompatible_value(const std::string &expected_type,
                                   const std::string &param_name)
      : std::runtime_error("Incompatible type when setting value of '" +
                           param_name + "', expected " + expected_type) {}
};

class api_builder_incompatible_operator : public std::runtime_error {
public:
  api_builder_incompatible_operator(const std::string &op,
                                    const std::string &builder_type)
      : std::runtime_error("Incompatible operator '" + op + "' given to '" +
                           builder_type + "' builder") {}
};

class api_builder_request_not_found : public std::runtime_error {
public:
  api_builder_request_not_found(long unsigned int id)
      : std::runtime_error("Request ID " + to_string(id) +
                           " not found in builder") {}
};

#endif
