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
  const auto old_path = get_output_path();
  const auto given_path = fs::path(path);
  if (given_path.is_relative()) {
    throw logger_exception("Output path must be absolute");
  }
  if (fs::is_directory(given_path)) {
    throw logger_exception("Output path must specify filename");
  }
  if (!fs::exists(given_path.parent_path())) {
    throw logger_exception("Output path folder structure must already exist");
  }
  _output_path = path;
  if (fs::exists(old_path)) {
    fs::rename(old_path, given_path);
  }
}

void logger::save_to_db() {
  if (!logger::_initialized) {
    throw logger_not_started_exception();
  }
  if ((logger::level)stoi(logger::_setting_level.get_value()) !=
      logger::get_level()) {
    logger::_setting_level.set_value(to_string(logger::get_level()));
    logger::_setting_level.update();
    logger::info("New logger level saved: " +
                 _level_strings.at(logger::get_level()));
  }
  if ((logger::output)stoi(logger::_setting_output.get_value()) !=
      logger::get_output()) {
    logger::_setting_output.set_value(to_string(logger::get_output()));
    logger::_setting_output.update();
    logger::info("New logger output saved: " +
                 _output_strings.at(logger::get_output()));
  }
  if (logger::_setting_output_path.get_value() != logger::get_output_path()) {
    logger::_setting_output_path.set_value(logger::get_output_path());
    logger::_setting_output_path.update();
    logger::info("New logger output path saved: " + logger::get_output_path());
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
    os << "- " << msg;
  }
}

void logger::print_log(logger::level level, const string &msg) {
  if (!logger::_initialized) {
    logger::_backlog.push_back(make_pair(level, msg));
    return;
  }
  stringstream ss;
  logger::ostream_log(ss, level, msg);

  if (ss.str() != "") {
    if (logger::get_output() == logger::output::FILE) {
      ofstream f(logger::get_output_path(), std::ofstream::app);
      f << ss.str() << endl;
      f.close();
    } else {
      cout << ss.str() << endl;
    }

    if (logger::custom_output.has_value()) {
      logger::custom_output.value()->output(ss.str());
    }
  }
}

void logger::start() {
  if (logger::_initialized) {
    return;
  }
  logger::_initialized = true;
  logger::_setting_level = Setting(Setting::LOGGER_LEVEL);
  logger::_setting_output = Setting(Setting::LOGGER_OUTPUT);
  logger::_setting_output_path = Setting(Setting::LOGGER_OUTPUT_PATH);
  try {
    logger::set_level(logger::_setting_level.get_value());
    logger::set_output(logger::_setting_output.get_value());
    logger::set_output_path(logger::_setting_output_path.get_value());
  } catch (const logger_exception &e) {
    logger::_initialized = false;
    logger::error("Invalid logger settings from DB, recreating it");
    logger::set_level(Setting::get_default_value(Setting::LOGGER_LEVEL));
    logger::set_output(Setting::get_default_value(Setting::LOGGER_OUTPUT));
    logger::set_output_path(
        Setting::get_default_value(Setting::LOGGER_OUTPUT_PATH));
  }
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

void logger::add_custom_output(shared_ptr<LoggerCustomOutput> out) {
  logger::custom_output = out;
}

void logger::clear_custom_output() { logger::custom_output.reset(); }

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

void logger::clear_logfile() {
  if (fs::exists(get_output_path())) {
    fs::remove(get_output_path());
  }
  logger::info("Logfile cleared at " + get_output_path());
}

bool logger::_initialized = false;
logger::level logger::_level;
logger::output logger::_output;
string logger::_output_path;
Setting logger::_setting_level;
Setting logger::_setting_output;
Setting logger::_setting_output_path;
vector<pair<logger::level, string>> logger::_backlog;
optional<shared_ptr<LoggerCustomOutput>> logger::custom_output = std::nullopt;

unordered_map<logger::level, string> logger::_level_strings = {
    {logger::level::DEBUG, "debug"},
    {logger::level::INFO, "info"},
    {logger::level::WARNING, "warning"},
    {logger::level::ERROR, "error"},
    {logger::level::NONE, "none"}};

unordered_map<logger::output, string> logger::_output_strings = {
    {logger::output::STDOUT, "stdout"}, {logger::output::FILE, "file"}};
