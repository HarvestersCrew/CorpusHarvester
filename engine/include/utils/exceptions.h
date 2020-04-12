#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::string;
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

class ExceptionWrapper : public std::runtime_error {
protected:
  string _exception_name;

public:
  ExceptionWrapper(const string &msg, const string &exception_name)
      : std::runtime_error(msg), _exception_name(exception_name) {}
  virtual const string exception_name() const { return _exception_name; }
};

class SettingNotFoundException : public ExceptionWrapper {

public:
  SettingNotFoundException(std::string &name)
      : ExceptionWrapper("Setting with name '" + name +
                             "' was not found in the database",
                         "SettingNotFoundException") {}
};

class CommandException : public ExceptionWrapper {

public:
  CommandException(std::string &error_message)
      : ExceptionWrapper(error_message, "CommandException") {}
};

class ClosedDatabaseException : public ExceptionWrapper {

public:
  ClosedDatabaseException()
      : ExceptionWrapper("Database is closed", "ClosedDatabaseException") {}
};

class api_no_setting_exception : public ExceptionWrapper {
public:
  api_no_setting_exception(const std::string &key)
      : ExceptionWrapper("Given setting key not existing in API: " + key,
                         "api_no_setting_exception") {}
};

class api_missing_settings_exception : public ExceptionWrapper {
public:
  api_missing_settings_exception(const std::string &setting)
      : ExceptionWrapper("Missing setting to call an API: " + setting,
                         "api_missing_settings_exception") {}
};

class api_filetype_incompatible : public ExceptionWrapper {
public:
  api_filetype_incompatible(const std::string &type)
      : ExceptionWrapper("Can't manage filetype: " + type,
                         "api_filetype_incompatible") {}
};

class api_unrecognized_settings_exception : public ExceptionWrapper {
public:
  api_unrecognized_settings_exception()
      : ExceptionWrapper("An API setting was unrecognized",
                         "api_unrecognized_settings_exception") {}
  api_unrecognized_settings_exception(const std::string &key,
                                      const std::string &val)
      : ExceptionWrapper("An API setting value was unrecognized: " + key +
                             " = " + val,
                         "api_unrecognized_settings_exception") {}
};

class download_no_200_exception : public ExceptionWrapper {
public:
  download_no_200_exception()
      : ExceptionWrapper("HTTP call didn't succeed",
                         "download_no_200_exception") {}
  download_no_200_exception(int http_code)
      : ExceptionWrapper("HTTP call didn't succeed: " + to_string(http_code),
                         "download_no_200_exception") {}
};

class logger_exception : public ExceptionWrapper {
public:
  logger_exception(std::string error_message)
      : ExceptionWrapper(error_message, "logger_exception") {}
};

class cli_parser_exception : public ExceptionWrapper {
public:
  cli_parser_exception(std::string error_message)
      : ExceptionWrapper(error_message, "cli_parser_exception") {}
};

class cli_parser_bad_parse_exception : public ExceptionWrapper {
public:
  cli_parser_bad_parse_exception(std::string error_message)
      : ExceptionWrapper(error_message, "cli_parser_bad_parse_exception") {}
};

class cli_parser_help_asked_exception : public ExceptionWrapper {
public:
  cli_parser_help_asked_exception()
      : ExceptionWrapper("User asked for help on CLI",
                         "cli_parser_help_asked_exception") {}
};

class api_default_not_in_schema : public ExceptionWrapper {
public:
  api_default_not_in_schema(std::string param)
      : ExceptionWrapper("Default parameter '" + param +
                             "' not found in schema",
                         "api_default_not_in_schema") {}
};

class api_twice_same_name : public ExceptionWrapper {
public:
  api_twice_same_name(std::string name)
      : ExceptionWrapper("Another '" + name + "' already exists",
                         "api_twice_same_name") {}
};

class api_factory_name_not_found : public ExceptionWrapper {
public:
  api_factory_name_not_found(std::string name)
      : ExceptionWrapper("API '" + name + "' not found by API discover",
                         "api_factory_name_not_found") {}
};

class logger_not_started_exception : public ExceptionWrapper {
public:
  logger_not_started_exception()
      : ExceptionWrapper("Logger not started", "logger_not_started_exception") {
  }
};

class api_request_builder_invalid_type : public ExceptionWrapper {
public:
  api_request_builder_invalid_type(int val)
      : ExceptionWrapper("Invalid API type: " + to_string(val),
                         "api_request_builder_invalid_type") {}
};

class api_parameter_incompatible_value : public ExceptionWrapper {
public:
  api_parameter_incompatible_value(const std::string &expected_type,
                                   const std::string &param_name)
      : ExceptionWrapper("Incompatible type when setting value of '" +
                             param_name + "', expected " + expected_type,
                         "api_parameter_incompatible_value") {}
};

class api_builder_incompatible_operator : public ExceptionWrapper {
public:
  api_builder_incompatible_operator(const std::string &op,
                                    const std::string &builder_type)
      : ExceptionWrapper("Incompatible operator '" + op + "' given to '" +
                             builder_type + "' builder",
                         "api_builder_incompatible_operator") {}
};

class api_builder_request_not_found : public ExceptionWrapper {
public:
  api_builder_request_not_found(long unsigned int id)
      : ExceptionWrapper("Request ID " + to_string(id) +
                             " not found in builder",
                         "api_builder_request_not_found") {}
};

class wss_cant_find_handler : public ExceptionWrapper {
public:
  wss_cant_find_handler()
      : ExceptionWrapper("Given handled not found in server app",
                         "wss_cant_find_handler") {}
};

#endif
