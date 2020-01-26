#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <iostream>

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

class ApiRequiredSettingsNotFilledException : public std::exception {
  const char *what() const throw() {
    return "Required settings of API not filled.";
  }
};

class ApiNoSettingCalledLikeThisException : public std::exception {
  const char *what() const throw() { return "Given setting key not existing."; }
};

#endif
