#ifndef LOGGER_H
#define LOGGER_H

#include "utils/exceptions.h"
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <system_error>

#define LOGGER_DEFAULT_OUTPUT_PATH "./"
#define LOGGER_DEFAULT_FILENAME "logs"

using std::cout;
using std::endl;
using std::ofstream;
using std::ostream;
using std::string;
using std::stringstream;

class logger {

public:
  logger();
  enum level { DEBUG, INFO, WARNING, ERROR, NONE };
  enum output { STDOUT, FILE };

  static logger::level get_level();
  static void set_level(logger::level level);

  static logger::output get_output();
  static void set_output(logger::output output);

  static string get_output_path();
  static void set_output_path(string path);

  static void ostream_log(ostream &os, logger::level level, const string &msg);
  static void print_log(logger::level level, const string &msg);

  static void debug(const string &msg);
  static void info(const string &msg);
  static void warning(const string &msg);
  static void error(const string &msg);

private:
  static level _level;
  static output _output;
  static string _output_path;
};

#endif
