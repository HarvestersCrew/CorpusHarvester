#include <exception>

class ApiRequiredSettingsNotFilledException : public std::exception {
  const char *what() const throw() {
    return "Required settings of API not filled.";
  }
};

class ApiNoSettingCalledLikeThisException : public std::exception {
  const char *what() const throw() { return "Given setting key not existing."; }
};
