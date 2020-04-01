#include "utils/logger.h"

logger::logger() {}

logger::level logger::get_level() {
  logger::init_from_file();
  return logger::_level;
}
void logger::set_level(logger::level level) {
  if (!(level >= logger::level::DEBUG && level <= logger::level::NONE)) {
    throw logger_exception("Unsupported logging level");
  }
  logger::_level = level;
}
void logger::set_level(const string &level) {
  logger::set_level((logger::level)stoi(level));
}

logger::output logger::get_output() {
  logger::init_from_file();
  return logger::_output;
}
void logger::set_output(logger::output output) {
  if (!(output >= logger::output::STDOUT && output <= logger::output::FILE)) {
    throw logger_exception("Unsupported logging output");
  }
  logger::_output = output;
  if (output == logger::output::FILE) {
    logger::set_output_path(LOGGER_DEFAULT_OUTPUT_PATH);
  }
}
void logger::set_output(const string &output) {
  logger::set_output((logger::output)stoi(output));
}

string logger::get_output_path() {
  logger::init_from_file();
  return logger::_output_path;
}
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

void logger::init_from_file() {
  if (!logger::_initialized) {
    logger::set_default_values();
    logger::_initialized = true;
    bool created = false;
    try {
      nlohmann::json settings = json_from_file(logger::_settings_file);
      logger::set_level(settings.at("level").get<logger::level>());
      logger::set_output(settings.at("output").get<logger::output>());
      logger::set_output_path(settings.at("output_path").get<string>());
      created = true;
    } catch (const logger_exception &e) {
      logger::error("Logger settings file content invalid, recreating it");
    } catch (const nlohmann::detail::type_error &e) {
      logger::error("Logger settings file content invalid, recreating it");
    } catch (const std::runtime_error &e) {
      logger::warning("Logger settings file not found, creating it");
    } catch (const nlohmann::detail::parse_error &e) {
      logger::error("Logger settings file content invalid, recreating it");
    }

    if (!created) {
      logger::update_file_settings();
    }

    logger::info("Logger successfully initialized");
  }
}

void logger::update_file_settings() {
  nlohmann::json json;
  json["output"] = logger::get_output();
  json["output_path"] = logger::get_output_path();
  json["level"] = logger::get_level();
  ofstream file(logger::_settings_file);
  file << json;
  file.close();
}

void logger::set_default_values() {
  logger::set_level(logger::level::DEBUG);
  logger::set_output(logger::output::STDOUT);
  logger::set_output_path(LOGGER_DEFAULT_OUTPUT_PATH);
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
string logger::_settings_file = LOGGER_SETTINGS_PATH;
