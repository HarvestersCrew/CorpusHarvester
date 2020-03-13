#include "utils/logger.h"

logger::logger() {}

logger::level logger::get_level() { return logger::_level; }
void logger::set_level(logger::level level) { logger::_level = level; }

logger::output logger::get_output() { return logger::_output; }
void logger::set_output(logger::output output) {
  logger::_output = output;
  if (output == logger::output::FILE) {
    logger::set_output_path(LOGGER_DEFAULT_OUTPUT_PATH);
  }
}

string logger::get_output_path() { return logger::_output_path; }
void logger::set_output_path(string path) {
  if (path.back() != '/') {
    path += '/';
  }
  if (!std::filesystem::exists(path)) {
    string msg = "Logging path not found: " + path;
    throw logger_exception(msg);
  }
  logger::_output_path = path;
}

void logger::ostream_log(ostream &os, logger::level level, const string &msg) {
  if (level == logger::level::NONE) {
    throw logger_exception("Can't log level 'NONE'");
  }
  if (level >= logger::_level) {
    string tag = "";
    switch (level) {
    case logger::level::DEBUG:
      tag = "DEBUG";
      break;
    case logger::level::INFO:
      tag = "INFO";
      break;
    case logger::level::WARNING:
      tag = "WARNING";
      break;
    case logger::level::ERROR:
      tag = "ERROR";
      break;
    default:
      throw logger_exception("Unexpected logger lever");
      break;
    }
    os << "[" << tag << "] ";
    long unsigned int warning_length = 7;
    for (long unsigned int i = 0; i < warning_length - tag.size(); i++) {
      os << " ";
    }
    os << "- " << msg << endl;
  }
}

void logger::print_log(logger::level level, const string &msg) {
  if (logger::_output == logger::output::FILE) {
    stringstream ss;
    logger::ostream_log(ss, level, msg);
    if (ss.str() != "") {
      ofstream f(logger::_output_path + LOGGER_DEFAULT_FILENAME,
                 std::ofstream::app);
      f << ss.str();
      f.close();
    }
  } else {
    logger::ostream_log(cout, level, msg);
  }
}

void logger::debug(const string &msg) {
  logger::print_log(logger::level::DEBUG, msg);
}
void logger::info(const string &msg) {
  logger::print_log(logger::level::INFO, msg);
}
void logger::warning(const string &msg) {
  logger::print_log(logger::level::WARNING, msg);
}
void logger::error(const string &msg) {
  logger::print_log(logger::level::ERROR, msg);
}

logger::level logger::_level = logger::level::DEBUG;
logger::output logger::_output = logger::output::STDOUT;
string logger::_output_path = LOGGER_DEFAULT_OUTPUT_PATH;
