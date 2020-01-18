#include <exception>
#include <iostream>
#include <sstream>
#include <string>

class ApiNoSettingCalledLikeThisException : public std::exception {
public:
  ApiNoSettingCalledLikeThisException(std::string key);
  const char *what() const throw();

private:
  std::string _msg;
};
