#ifndef SETTING_H
#define SETTING_H

#include "database/database_item.h"
#include "utils/exceptions.h"
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>

#define SETTING_CREATE_STATEMENT                                               \
  "CREATE TABLE IF NOT EXISTS Setting(id INTEGER NOT NULL "                    \
  "AUTO_INCREMENT,name TEXT NOT NULL, value TEXT, "                            \
  "PRIMARY KEY (id));"
#define GET_SETTING_FROM_KEY "SELECT s.* FROM Setting s WHERE s.name = ?;"
#define GET_SETTINGS "SELECT * FROM Setting;"
#define INSERT_SETTING_STATEMENT                                               \
  "INSERT INTO Setting (name, value) VALUES(?, ?)"
#define UPDATE_SETTING_STATEMENT "UPDATE Setting SET value = ? WHERE key = ?"
#define DROP_SETTING_STATEMENT "DROP TABLE IF EXISTS Setting;"

/**
 * Setting class describes a Setting table in the database
 */
class Setting : public DatabaseItem {

  /** A map describing the default settings */
  static std::map<std::string, std::string> _default_settings;

  /** The path of the setting */
  std::string _name;

  /** The name of the setting */
  std::string _value;

public:
  /* Settings name */
  static std::string STORAGE_ROOT;

  /**
   * Default constructor
   */
  Setting();

  /**
   * Creates a Setting object
   * @param name the name of the setting
   * @param value the value of the setting
   */
  Setting(std::string name, std::string value);

  /**
   * Creates a Setting object from name fetching value in database
   * @param name the name of the setting
   */
  Setting(std::string name, sql::Connection *db);

  std::string to_string() const;

  bool insert(sql::Connection *db);

  /**
   * Updates the Setting object in the database
   * @param db the database
   */
  void update(sql::Connection *db);

  void fill_from_statement(sql::Connection *db, sql::ResultSet *res);

  std::string get_name() const { return _name; }
  std::string get_value() const { return _value; }
  static int get_init_settings_count() { return _default_settings.size(); }
  static std::string get_default_value(std::string name) {
    return _default_settings[name];
  }

  void set_name(std::string name) { _name = name; }
  void set_value(std::string value) { _value = value; }

  /**
   * Initializes the default settings in the database
   * @param db the database
   */
  static void init_settings(sql::Connection *db);
};

#endif
