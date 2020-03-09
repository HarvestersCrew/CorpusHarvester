#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include "utils/exceptions.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using std::cout;
using std::endl;
using std::make_pair;
using std::make_tuple;
using std::map;
using std::pair;
using std::string;
using std::stringstream;
using std::tuple;
using std::vector;

class cli_command {
private:
  /** Map of items to precise a command, like "corpus create" are two commands
   * right below the other */
  map<string, cli_command> _commands;
  /** Map of options in a terminal command */
  map<string, cli_command> _options;

  string _initial_call;
  string _description;
  bool _is_option;
  bool _is_option_bool;

public:
  cli_command(const string &initial_call, const string &description,
              bool is_option, bool is_option_bool);
  cli_command &add_command(const string &name, const string &description);
  void add_option(const string &name, const string &description,
                  bool is_option_bool);

  cli_command &get_command(const string &name);
  cli_command &get_option(const string &name);
  const cli_command &get_command(const string &name) const;
  const cli_command &get_option(const string &name) const;
  const map<string, cli_command> &get_commands() const;
  const map<string, cli_command> &get_options() const;
  const string &get_description() const;
  bool is_option() const;
  bool is_option_bool() const;
  bool is_terminal() const;

  string get_help() const;
};

class cli_parser {
public:
  static tuple<vector<string>, map<string, string>, map<string, bool>>
  parse(const cli_command &root, vector<string> cli_args);

private:
  cli_parser();
  static pair<map<string, string>, map<string, bool>>
  parse_terminal(const cli_command &root, const vector<string> &cli_args);
};

#endif
