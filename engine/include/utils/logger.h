#ifndef LOGGER_H
#define LOGGER_H

#include "database/setting.h"
#include "utils/exceptions.h"
#include "utils/utils.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::make_pair;
using std::ofstream;
using std::optional;
using std::ostream;
using std::pair;
using std::shared_ptr;
using std::stoi;
using std::string;
using std::stringstream;
using std::to_string;
using std::unordered_map;
using std::vector;

namespace fs = std::filesystem;

class Setting;

/** Class to be used as a custom additional input for the logger */
class LoggerCustomOutput {
public:
  virtual void output(const std::string &msg) const = 0;
};

/** Class used to manage logs, with levels and different outputs */
class logger {

  friend void test_logger_get_ostream();
  friend void test_logger_output_log();
  friend void test_logger_debug();
  friend void test_logger_info();
  friend void test_logger_warning();
  friend void test_logger_error();
  friend void test_logger_start();

public:
  logger();
  enum level { DEBUG, INFO, WARNING, ERROR, NONE };
  enum output { STDOUT, FILE };

  static unordered_map<level, string> _level_strings;
  static unordered_map<output, string> _output_strings;

  static logger::level get_level();
  static void set_level(logger::level level);
  static void set_level(const string &level);

  static logger::output get_output();
  static void set_output(logger::output output);
  static void set_output(const string &output);

  static string get_output_path();
  static void set_output_path(string path);

  static optional<shared_ptr<LoggerCustomOutput>> custom_output;

  /**
   * Calling this method checks and sets the settings from the DB and
   * starts outputting previously saved and future logs
   * @param db DB to use to load the settings
   */
  static void start();

  /**
   * Stops the logging and starts saving logs in the backlog
   */
  static void stop();

  /**
   * Adds a custom output to use at the same time as the standard outputs
   * @param out LoggerCustomOutput to use as additional output
   */
  static void add_custom_output(shared_ptr<LoggerCustomOutput> out);

  /**
   * Clears the custom output
   */
  static void clear_custom_output();

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
   * Saves the current settings to the DB (if different)
   */
  static void save_to_db();

private:
  /** Minimal level to output */
  static level _level;
  /** Where to output logs */
  static output _output;
  /** Directory to use if outputting to files */
  static string _output_path;
  /** If settings were loaded from DB */
  static bool _initialized;

  /** DB settings of the logger */
  static Setting _setting_level;
  static Setting _setting_output;
  static Setting _setting_output_path;

  /** Backlog of logs sent before the logger was initialized */
  static vector<pair<logger::level, string>> _backlog;

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
};

#endif
