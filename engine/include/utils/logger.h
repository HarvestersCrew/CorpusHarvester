#ifndef LOGGER_H
#define LOGGER_H

#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include "utils/utils.h"
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <system_error>

#define LOGGER_DEFAULT_OUTPUT_PATH "/tmp/"
#define LOGGER_DEFAULT_FILENAME "harvester_logs"
#define LOGGER_SETTINGS_PATH "./data/logger_settings.env.json"

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
   * Gives the attributes what we consider default values
   */
  static void set_default_values();

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

  /**
   * Saves settings to the settings file
   */
  static void update_file_settings();

private:
  /** Minimal level to output */
  static level _level;
  /** Where to output logs */
  static output _output;
  /** Directory to use if outputting to files */
  static string _output_path;
  /** If settings were loaded from file */
  static bool _initialized;
  /** Path to the settings file */
  static string _settings_file;

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

  /**
   * Inits if necessary the settings from the file
   */
  static void init_from_file();
};

#endif
