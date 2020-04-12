#include "test/logger_test.h"

void test_logger_set_level() {
  logger::set_level(logger::level::INFO);
  Assertion::assert_equals(__FUNCTION__, logger::level::INFO,
                           logger::get_level());
}

void test_logger_set_output() {
  logger::set_output(logger::output::FILE);
  Assertion::assert_equals(__FUNCTION__, logger::output::FILE,
                           logger::get_output());
}

void test_logger_set_output_path() {
  logger::set_output_path("/tmp/");
  Assertion::assert_equals(__FUNCTION__, logger::get_output_path(), "/tmp/");

  logger::set_output_path("/tmp");
  Assertion::assert_equals(__FUNCTION__, logger::get_output_path(), "/tmp/");

  logger::set_output_path("./");
  Assertion::assert_equals(__FUNCTION__, logger::get_output_path(), "./");

  try {
    logger::set_output_path("/random_path_ahzgruthrgke");
    Assertion::assert_throw(__FUNCTION__, "filesystem_error");
  } catch (logger_exception &e) {
    return;
  }
}

void test_logger_get_full_output_path() {
  logger::set_output_path("/tmp/");
  Assertion::assert_equals(__FUNCTION__, logger::get_full_output_path(),
                           "/tmp/" LOGGER_DEFAULT_FILENAME);
}

void test_logger_get_ostream() {
  stringstream ss;
  string to_log;
  logger::set_level(logger::level::DEBUG);

  to_log = "Blabla";
  logger::ostream_log(ss, logger::level::DEBUG, to_log);
  to_log = "[DEBUG]   - " + to_log;
  Assertion::assert_equals(__FUNCTION__, to_log, ss.str());
  ss.clear();

  try {
    logger::ostream_log(ss, logger::level::NONE, to_log);
    Assertion::assert_throw(__FUNCTION__, "logger_error");
  } catch (const logger_exception &e) {
    return;
  }

  logger::set_level(logger::level::ERROR);
  to_log = "Blabla";
  logger::ostream_log(ss, logger::level::DEBUG, to_log);
  Assertion::assert_equals(__FUNCTION__, "", ss.str());
  logger::ostream_log(ss, logger::level::ERROR, to_log);
  to_log = "[ERROR]   - " + to_log;
  Assertion::assert_equals(__FUNCTION__, to_log, ss.str());
  ss.clear();
}

void test_logger_output_log() {
  logger::set_output_path("/tmp/");
  std::filesystem::remove(logger::get_full_output_path());
  logger::set_level(logger::level::DEBUG);
  logger::set_output(logger::output::FILE);
  logger::_initialized = true;

  stringstream msg1, msg2, expected, result;
  msg1 << "TEST MSG";
  msg2 << "TEST MSG2";

  logger::print_log(logger::level::DEBUG, msg1.str());
  logger::ostream_log(expected, logger::level::DEBUG, msg1.str());
  expected << endl;
  logger::print_log(logger::level::ERROR, msg2.str());
  logger::ostream_log(expected, logger::level::ERROR, msg2.str());
  expected << endl;

  ifstream f(logger::get_full_output_path());
  result << f.rdbuf();
  f.close();

  Assertion::assert_equals(__FUNCTION__, expected.str(), result.str());

  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_debug() {
  logger::set_output_path("./");
  std::filesystem::remove(logger::get_full_output_path());
  logger::set_level(logger::level::DEBUG);
  logger::set_output(logger::output::FILE);
  logger::_initialized = true;

  stringstream msg, expected, result;
  msg << "BLABLA";

  logger::debug(msg.str());
  logger::ostream_log(expected, logger::level::DEBUG, msg.str());
  expected << endl;
  logger::set_level(logger::level::INFO);
  logger::ostream_log(expected, logger::level::DEBUG, msg.str());

  ifstream f(logger::get_full_output_path());
  result << f.rdbuf();
  f.close();
  Assertion::assert_equals(__FUNCTION__, expected.str(), result.str());

  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_info() {
  logger::set_output_path("./");
  std::filesystem::remove(logger::get_full_output_path());
  logger::set_level(logger::level::INFO);
  logger::set_output(logger::output::FILE);
  logger::_initialized = true;

  stringstream msg, expected, result;
  msg << "BLABLA";

  logger::info(msg.str());
  logger::ostream_log(expected, logger::level::INFO, msg.str());
  expected << endl;
  logger::set_level(logger::level::WARNING);
  logger::ostream_log(expected, logger::level::INFO, msg.str());

  ifstream f(logger::get_full_output_path());
  result << f.rdbuf();
  f.close();
  Assertion::assert_equals(__FUNCTION__, expected.str(), result.str());

  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_warning() {
  logger::set_output_path("./");
  std::filesystem::remove(logger::get_full_output_path());
  logger::set_level(logger::level::WARNING);
  logger::set_output(logger::output::FILE);
  logger::_initialized = true;

  stringstream msg, expected, result;
  msg << "BLABLA";

  logger::warning(msg.str());
  logger::ostream_log(expected, logger::level::WARNING, msg.str());
  expected << endl;
  logger::set_level(logger::level::ERROR);
  logger::ostream_log(expected, logger::level::WARNING, msg.str());

  ifstream f(logger::get_full_output_path());
  result << f.rdbuf();
  f.close();
  Assertion::assert_equals(__FUNCTION__, expected.str(), result.str());

  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_error() {
  logger::set_output_path("./");
  std::filesystem::remove(logger::get_full_output_path());
  logger::set_level(logger::level::ERROR);
  logger::set_output(logger::output::FILE);
  logger::_initialized = true;

  stringstream msg, expected, result;
  msg << "BLABLA";

  logger::error(msg.str());
  logger::ostream_log(expected, logger::level::ERROR, msg.str());
  expected << endl;
  logger::set_level(logger::level::NONE);
  logger::ostream_log(expected, logger::level::ERROR, msg.str());

  ifstream f(logger::get_full_output_path());
  result << f.rdbuf();
  f.close();
  Assertion::assert_equals(__FUNCTION__, expected.str(), result.str());

  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_start() {
  logger::set_output_path("./");
  logger::set_level(logger::level::ERROR);
  logger::set_output(logger::output::FILE);
  logger::save_to_db();

  logger::_initialized = false;
  std::filesystem::remove(logger::get_full_output_path());

  stringstream msg, expected, result, result2;
  msg << "BLABLA";

  logger::error(msg.str());
  logger::ostream_log(expected, logger::level::ERROR, msg.str());
  expected << endl;

  ifstream f1(logger::get_full_output_path());
  result << f1.rdbuf();
  f1.close();
  Assertion::assert_equals(__FUNCTION__, "", result.str());
  std::filesystem::remove(logger::get_full_output_path());

  // Inits (or not) the DB and starts the logger
  HarvesterDatabase::init();
  ifstream f2(logger::get_full_output_path());
  result2 << f2.rdbuf();
  f2.close();
  Assertion::assert_equals(__FUNCTION__, expected.str(), result2.str());
  std::filesystem::remove(logger::get_full_output_path());
}

void test_logger_save_to_db() {
  logger::set_level(logger::level::DEBUG);
  logger::set_output(logger::output::STDOUT);
  logger::set_output_path("/tmp/");
  logger::save_to_db();

  logger::set_level(logger::level::WARNING);
  logger::set_output(logger::output::FILE);
  logger::set_output_path("./");
  logger::save_to_db();

  Assertion::assert_equals(
      __FUNCTION__, "2",
      Setting(Setting::LOGGER_LEVEL, HarvesterDatabase::init()).get_value());
  Assertion::assert_equals(
      __FUNCTION__, "1",
      Setting(Setting::LOGGER_OUTPUT, HarvesterDatabase::init()).get_value());
  Assertion::assert_equals(
      __FUNCTION__, "./",
      Setting(Setting::LOGGER_OUTPUT_PATH, HarvesterDatabase::init())
          .get_value());
}

void logger_test() {
  std::cout << std::endl << "Logger tests : " << std::endl;
  Assertion::test(test_logger_set_level, "test_logger_set_level");
  Assertion::test(test_logger_set_output, "test_logger_set_output");
  Assertion::test(test_logger_set_output_path, "test_logger_set_output_path");
  Assertion::test(test_logger_get_full_output_path,
                  "test_logger_get_full_output_path");
  Assertion::test(test_logger_get_ostream, "test_logger_get_ostream");
  Assertion::test(test_logger_output_log, "test_logger_output_log");
  Assertion::test(test_logger_debug, "test_logger_debug");
  Assertion::test(test_logger_info, "test_logger_info");
  Assertion::test(test_logger_warning, "test_logger_warning");
  Assertion::test(test_logger_error, "test_logger_error");
  Assertion::test(test_logger_start, "test_logger_start");
  Assertion::test(test_logger_save_to_db, "test_logger_save_to_db");
}
