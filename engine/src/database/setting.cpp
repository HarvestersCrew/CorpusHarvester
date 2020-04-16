#include "database/setting.h"

/* STATIC VARIABLES */

string Setting::STORAGE_NAME = "stored/";
string Setting::STORAGE_ROOT = "storage_root";
string Setting::LOGGER_LEVEL = "logger_level";
string Setting::LOGGER_OUTPUT = "logger_output";
string Setting::LOGGER_OUTPUT_PATH = "logger_output_path";

std::map<string, string> Setting::_default_settings = {
    {Setting::STORAGE_ROOT, "/tmp/" + Setting::STORAGE_NAME},
    {Setting::LOGGER_LEVEL, "0"},
    {Setting::LOGGER_OUTPUT, "0"},
    {Setting::LOGGER_OUTPUT_PATH, "/tmp/harvester_logs"}};

/* METHODS */

Setting::Setting(string name, string value)
    : DatabaseItem(-1), _name(name), _value(value) {}

Setting::Setting(string name) : DatabaseItem(-1) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();
  prep_stmt = con->prepareStatement(GET_SETTING_FROM_KEY);
  prep_stmt->setString(1, name);
  res = prep_stmt->executeQuery();
  PoolDB::unborrow_from_pool(con);
  if (!res->next()) {
    throw SettingNotFoundException(name);
  }
  fill_from_statement(res);
  delete prep_stmt;
  delete res;
}

Setting::Setting() : DatabaseItem(0), _name(""), _value("") {}

string Setting::to_string() const {
  std::ostringstream out;
  out << "Setting{_id=" << _id << ", _name=" << _name << ", _value=" << _value
      << "}";
  return out.str();
}

bool Setting::insert() {

  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();

  try {
    prep_stmt = con->prepareStatement(GET_SETTING_FROM_KEY);
    prep_stmt->setString(1, _name);
    res = prep_stmt->executeQuery();
    if (res->next()) {
      logger::error("Can't insert " + _name + " setting : already exists");
      return false;
    }

    prep_stmt = con->prepareStatement(INSERT_SETTING_STATEMENT);
    prep_stmt->setString(1, _name);
    prep_stmt->setString(2, _value);
    prep_stmt->execute();
    _id = DatabaseItem::get_last_inserted_id(con);
    logger::debug("Setting inserted '" + _name + "' to DB");
  } catch (sql::SQLException &e) {
    log_sql_exception(e);
    return false;
  }

  delete res;
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);
  return true;
}

void Setting::update() {
  sql::PreparedStatement *prep_stmt;
  auto con = PoolDB::borrow_from_pool();
  prep_stmt = con->prepareStatement(UPDATE_SETTING_STATEMENT);
  prep_stmt->setString(1, _value);
  prep_stmt->setString(2, _name);
  prep_stmt->execute();
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);
  logger::debug("Setting '" + _name + "' updated in DB");
}

void Setting::fill_from_statement(sql::ResultSet *res) {
  _id = res->getInt("id");
  _name = res->getString("name");
  _value = res->getString("value");
}

void Setting::set_default_value(const string &name, const string &value) {
  Setting::_default_settings.at(name) = value;
}

void Setting::init_settings() {
  Setting setting = Setting();
  for (auto &pair : _default_settings) {
    setting.set_name(pair.first);
    setting.set_value(pair.second);
    setting.insert();
  }
}
