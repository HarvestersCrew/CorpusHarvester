#ifndef SETTING_H
#define SETTING_H

#include "database/database_item.h"
#include "database/pool_db.h"
#include "utils/exceptions.h"
#include "utils/utils.h"
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <sstream>

#define SETTING_CREATE_STATEMENT                                               \
  "CREATE TABLE IF NOT EXISTS Setting(id INTEGER NOT NULL "                    \
  "AUTO_INCREMENT,name TEXT NOT NULL, value TEXT, "                            \
  "PRIMARY KEY (id));"
#define GET_SETTING_FROM_KEY "SELECT s.* FROM Setting s WHERE s.name = ?;"
#define GET_SETTINGS "SELECT * FROM Setting;"
#define INSERT_SETTING_STATEMENT                                               \
  "INSERT INTO Setting (name, value) VALUES(?, ?);"
#define UPDATE_SETTING_STATEMENT "UPDATE Setting SET value = ? WHERE name = ?"
#define DROP_SETTING_STATEMENT "DROP TABLE IF EXISTS Setting;"

using std::map;
using std::shared_ptr;
using std::string;

/**
 * Setting class describes a Setting table in the database
 */
class Setting : public DatabaseItem {

  /** A map describing the default settings */
  static map<string, string> _default_settings;

  /** The path of the setting */
  string _name;

  /** The name of the setting */
  string _value;

public:
  static string STORAGE_NAME;
  /* Settings name */
  static string STORAGE_ROOT;

  static string LOGGER_LEVEL;
  static string LOGGER_OUTPUT;
  static string LOGGER_OUTPUT_PATH;

  /**
   * Default constructor
   */
  Setting();

  /**
   * Creates a Setting object
   * @param name the name of the setting
   * @param value the value of the setting
   */
  Setting(string name, string value);

  /**
   * Creates a Setting object from name fetching value in database
   * @param name the name of the setting
   */
  Setting(string name);

  string to_string() const;

  /**
   * Inserts the current setting in the DB
   */
  bool insert();

  /**
   * Updates the value and the link in the database
   */
  void update();

  /**
   * Fills this setting from a DB resultset
   * @param res result from the DB for a setting
   */
  void fill_from_statement(sql::ResultSet *res);

  string get_name() const { return _name; }
  string get_value() const { return _value; }

  static int get_init_settings_count() { return _default_settings.size(); }
  /**
   * Get the default value of a given setting
   * @param name setting name to get default value
   */
  static string get_default_value(string name) {
    return _default_settings[name];
  }
  /**
   * Get the default of the current setting
   */
  string get_default_value() { return _default_settings.at(this->_name); }

  void set_name(string name) { _name = name; }
  void set_value(string value) { _value = value; }

  /**
   * Edit (for the current execution) the default value of a given setting
   * @param name setting to edit
   * @param value to use
   */
  static void set_default_value(const string &name, const string &value);

  /**
   * Initializes the default settings in the database
   * @param db the database
   */
  static void init_settings(sql::Connection *db);
  static void init_settings(shared_ptr<sql::Connection> db);
};

#endif
