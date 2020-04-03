#include "database/setting.h"

/* STATIC VARIABLES */

std::string Setting::STORAGE_ROOT = "storage_root";
std::string Setting::LOGGER_LEVEL = "logger_level";
std::string Setting::LOGGER_OUTPUT = "logger_output";
std::string Setting::LOGGER_OUTPUT_PATH = "logger_output_path";

std::map<std::string, std::string> Setting::_default_settings = {
    {Setting::STORAGE_ROOT, "/tmp/stored/"},
    {Setting::LOGGER_LEVEL, "0"},
    {Setting::LOGGER_OUTPUT, "0"},
    {Setting::LOGGER_OUTPUT_PATH, "/tmp/"}};

/* METHODS */

Setting::Setting(std::string name, std::string value)
    : DatabaseItem(-1), _name(name), _value(value) {}

Setting::Setting(std::string name, sql::Connection *db) : DatabaseItem(-1) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  prep_stmt = db->prepareStatement(GET_SETTING_FROM_KEY);
  prep_stmt->setString(1, name);
  res = prep_stmt->executeQuery();
  if (!res->next()) {
    throw SettingNotFoundException(name);
  }
  fill_from_statement(db, res);
}

Setting::Setting() : DatabaseItem(0), _name(""), _value("") {}

std::string Setting::to_string() const {
  std::ostringstream out;
  out << "Setting{_id=" << _id << ", _name=" << _name << ", _value=" << _value
      << "}";
  return out.str();
}

bool Setting::insert(sql::Connection *db) {

  try {
    sql::PreparedStatement *prep_stmt;
    sql::ResultSet *res;
    prep_stmt = db->prepareStatement(GET_SETTING_FROM_KEY);
    prep_stmt->setString(1, _name);
    res = prep_stmt->executeQuery();
    if (res->next()) {
      logger::error("Can't insert " + _name + " setting : already exists");
      return false;
    }

    prep_stmt = db->prepareStatement(INSERT_SETTING_STATEMENT);
    prep_stmt->setString(1, _name);
    prep_stmt->setString(2, _value);
    prep_stmt->execute();
    _id = DatabaseItem::get_last_inserted_id(db);
    delete res;
    delete prep_stmt;
  } catch (sql::SQLException &e) {
    log_sql_exception(e);
    return false;
  }

  return true;
}

void Setting::update(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;

  prep_stmt = db->prepareStatement(UPDATE_SETTING_STATEMENT);
  prep_stmt->setString(1, _value);
  prep_stmt->setString(2, _name);
  prep_stmt->execute();
  delete prep_stmt;
}

void Setting::fill_from_statement(sql::Connection *db, sql::ResultSet *res) {
  _id = res->getInt("id");
  _name = res->getString("name");
  _value = res->getString("value");
  db->isClosed();
}

void Setting::init_settings(sql::Connection *db) {
  Setting setting = Setting();
  for (auto &pair : _default_settings) {
    setting.set_name(pair.first);
    setting.set_value(pair.second);
    setting.insert(db);
  }
}
