#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

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
