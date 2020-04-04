#include "utils/logger.h"

logger::logger() {}

logger::level logger::get_level() { return logger::_level; }
void logger::set_level(logger::level level) {
  if (!(level >= logger::level::DEBUG && level <= logger::level::NONE)) {
    throw logger_exception("Unsupported logging level");
  }
  logger::_level = level;
}
void logger::set_level(const string &level) {
  logger::set_level((logger::level)stoi(level));
}

logger::output logger::get_output() { return logger::_output; }
void logger::set_output(logger::output output) {
  if (!(output >= logger::output::STDOUT && output <= logger::output::FILE)) {
    throw logger_exception("Unsupported logging output");
  }
  logger::_output = output;
}
void logger::set_output(const string &output) {
  logger::set_output((logger::output)stoi(output));
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

string logger::get_full_output_path() {
  stringstream ss;
  ss << logger::get_output_path() << LOGGER_DEFAULT_FILENAME;
  return ss.str();
}

void logger::save_to_db() {
  if (!logger::_initialized) {
    throw logger_not_started_exception();
  }
  if ((logger::level)stoi(logger::_setting_level.get_value()) !=
      logger::get_level()) {
    logger::_setting_level.set_value(to_string(logger::get_level()));
    logger::_setting_level.update(HarvesterDatabase::init());
  }
  if ((logger::output)stoi(logger::_setting_output.get_value()) !=
      logger::get_output()) {
    logger::_setting_output.set_value(to_string(logger::get_output()));
    logger::_setting_output.update(HarvesterDatabase::init());
    logger::debug("Saved logger output to DB");
  }
  if (logger::_setting_output_path.get_value() != logger::get_output_path()) {
    logger::_setting_output_path.set_value(logger::get_output_path());
    logger::_setting_output_path.update(HarvesterDatabase::init());
    logger::debug("Saved logger output path to DB");
  }
}

void logger::ostream_log(ostream &os, logger::level level, const string &msg) {
  if (level == logger::level::NONE) {
    throw logger_exception("Can't log level 'NONE'");
  }
  if (level >= logger::get_level()) {
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
  if (!logger::_initialized) {
    logger::_backlog.push_back(make_pair(level, msg));
    return;
  }
  if (logger::get_output() == logger::output::FILE) {
    stringstream ss;
    logger::ostream_log(ss, level, msg);
    if (ss.str() != "") {
      ofstream f(logger::get_output_path() + LOGGER_DEFAULT_FILENAME,
                 std::ofstream::app);
      f << ss.str();
      f.close();
    }
  } else {
    logger::ostream_log(cout, level, msg);
  }
}

void logger::start(Setting level, Setting output, Setting output_path) {
  if (logger::_initialized) {
    return;
  }
  logger::_setting_level = level;
  logger::_setting_output = output;
  logger::_setting_output_path = output_path;
  try {
    logger::set_level(logger::_setting_level.get_value());
    logger::set_output(logger::_setting_output.get_value());
    logger::set_output_path(logger::_setting_output_path.get_value());
  } catch (const logger_exception &e) {
    logger::error("Invalid logger settings from DB, recreating it");
    logger::set_level(Setting::get_default_value(Setting::LOGGER_LEVEL));
    logger::set_output(Setting::get_default_value(Setting::LOGGER_OUTPUT));
    logger::set_output_path(
        Setting::get_default_value(Setting::LOGGER_OUTPUT_PATH));
  }
  logger::_initialized = true;
  for (const auto &el : logger::_backlog) {
    logger::print_log(el.first, el.second);
  }
  logger::_backlog.clear();
  logger::save_to_db();
  logger::debug("Logger successfully initialized");
}

void logger::stop() {
  logger::_backlog.clear();
  logger::_initialized = false;
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

bool logger::_initialized = false;
logger::level logger::_level;
logger::output logger::_output;
string logger::_output_path;
Setting logger::_setting_level;
Setting logger::_setting_output;
Setting logger::_setting_output_path;
vector<pair<logger::level, string>> logger::_backlog;
