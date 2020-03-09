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

/**
 * Represents a command for the CLI parser
 * Usage:
 * For a given program, let's call it 'a.out', instantiate one cli_command as
 * is:
 * cli_command my_cli_parser("a.out", "My great program");
 * Then you can add a tree of subcommands like this:
 * cli_command &command1 = my_cli_parser.add_command("command1", "Command 1
 * under root");
 * cli_command &command2 = my_cli_parser.add_command("command2", "Command 2
 * under root");
 * cli_command &command1_1 = command1.add_command("command1.1", "Command 1 of
 * command 1 under root");
 * Finally, subcommands without other subcommands below them are terminal,
 * that's when the parser gives its response.
 * You can add options to a terminal command (--option1, --option2...) as is:
 * command2.add_option("option1", "my option 1", false);
 * command2.add_option("option2", "my option 2", true);
 * The boolean specifies if the option is considered as a value for a boolean
 * (and needs no string after it)
 * Then call the parser with:
 * vector<string> subcommands;
 * map<string, string> string_inputs;
 * map<string, bool> bool_inputs;
 * std::tie(subcommands, string_inputs, bool_inputs =
 * cli_parser::parse(my_cli_parser, vector_of_user_args);
 * 'subcommands' has a list of subcommands
 * 'string_inputs' has options as key and values
 * 'bool_inputs' has options as key and values
 *
 * So with the parser above and this command parameters:
 * command2 --option1 "value" --option2,
 * subcommands = [command2]
 * string_inputs = [<option1, value>],
 * bool_inputs = [<option2, true>] (it'd be false if no --option2 was put in by
 * the user)
 *
 * Also, a '-h' anywhere in the parameters displays the help of the level
 */
class cli_command {

private:
  /** Map of items to precise a command, like "corpus create" are two commands
   * right below the other */
  map<string, cli_command> _commands;
  /** Map of options in a terminal command */
  map<string, cli_command> _options;

  /** List of CLI commands up to now */
  string _initial_call;
  /** Description of the current command */
  string _description;
  /** If the command is an option */
  bool _is_option;
  /** If the command is an option setting its value to true when present */
  bool _is_option_bool;

public:
  /**
   * Instantiates a command
   * @param initial_call the path of the command so it can be displayed in
   * the usage helper (./a.out for a root command, corpus or api for
   * subcommands)
   * @param description a description to describe the command
   */
  cli_command(const string &initial_call, const string &description);

  /**
   * Instantiates a command
   * Not to be used by public users, is public do to emplacing.
   * @param initial_call the path of the command so it can be displayed in
   * the usage helper (./a.out for a root command, corpus or api for
   * subcommands)
   * @param description a description to describe the command
   * @param is_option if the command is an option at the end of a list of
   * commands
   * @param is_option_bool matters only if is_option is true, specifies if the
   * option is a boolean, not requiring a string in the CLI
   */
  cli_command(const string &initial_call, const string &description,
              bool is_option, bool is_option_bool);

  /**
   * Adds a subcommand to the current command and returns a reference to the new
   * one created
   * @param name name of the subcommand
   * @param description description of the subcommand
   * @return cli_command reference to the newly created command
   */
  cli_command &add_command(const string &name, const string &description);

  /**
   * Adds an option to the current command
   * @param name name of the option
   * @param description description of the option
   * @param is_option_bool if the option is a boolean, not requiring a string in
   * the CLI
   */
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

  /**
   * Gets the helper string related to this command
   * @return string helper string to display
   */
  string get_help() const;
};

/**
 * Static methods to parse a command line input
 */
class cli_parser {
public:
  /**
   * Given a root command and a vector of args (0 being the first one), returns
   * a tuple of three values representing the command of the user
   * @param root cli_command at the root of the parsing
   * @param cli_args vector of CLI arguments to parse
   * @return tuple of three values, use 'std::tie(subcommands, string_inputs,
   * bool_inputs) = parse(...);' to fill the three variables (created
   * beforehand)
   * vector<string>: list of subcommands given by the user to define the
   * function he wants
   * map<string, string>: map of string options with the name
   * as key and the value as value
   * map<string, bool>: map of boolean options
   * with the name as key and the value as value
   * @throw cli_parser_help_asked_exception if the user asked for help (maybe
   * hide it when catching it, or logging it)
   * @throw cli_parser_bad_parse_exception if the CLI arguments given can't be
   * matched by the parser
   */
  static tuple<vector<string>, map<string, string>, map<string, bool>>
  parse(const cli_command &root, vector<string> cli_args);

private:
  cli_parser();

  /**
   * Parses a terminal subcommand (one with options)
   * See the definition of parse to get meaning of args
   */
  static pair<map<string, string>, map<string, bool>>
  parse_terminal(const cli_command &root, const vector<string> &cli_args);
};

#endif
