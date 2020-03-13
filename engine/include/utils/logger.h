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
using std::stoi;
using std::string;
using std::stringstream;

class Setting;

/** Class used to manage logs, with levels and different outputs */
class logger {

  friend void test_logger_get_ostream();
  friend void test_logger_output_log();
  friend void test_logger_debug();
  friend void test_logger_info();
  friend void test_logger_warning();
  friend void test_logger_error();

public:
  logger();
  enum level { DEBUG, INFO, WARNING, ERROR, NONE };
  enum output { STDOUT, FILE };

  static logger::level get_level();
  static void set_level(logger::level level);
  static void set_level(const string &level);

  static logger::output get_output();
  static void set_output(logger::output output);
  static void set_output(const string &output);

  static string get_output_path();
  static void set_output_path(string path);

  /**
   * Logs a debug message
   * @param msg message to log
   */
  static void debug(const string &msg);
  /**
   * Logs an info message
   * @param msg message to log
   */
  static void info(const string &msg);
  /**
   * Logs a warning message
   * @param msg message to log
   */
  static void warning(const string &msg);
  /**
   * Logs an error message
   * @param msg message to log
   */
  static void error(const string &msg);

private:
  /** Minimal level to output */
  static level _level;
  /** Where to output logs */
  static output _output;
  /** Directory to use if outputting to files */
  static string _output_path;

  static bool _initialized;

  /**
   * Used to updated an ostream based on a given level and message.
   * @param os ostream to update
   * @param level level of importance of the message
   * @param msg message to log
   */
  static void ostream_log(ostream &os, logger::level level, const string &msg);
  /**
   * Prints the given message to the logger output
   * @param level level of importance of the message
   * @param msg message to log
   */
  static void print_log(logger::level level, const string &msg);

  static void init_from_file();
};

#endif
