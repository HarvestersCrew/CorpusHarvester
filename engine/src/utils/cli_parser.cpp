#include "utils/cli_parser.h"

cli_command::cli_command(const string &initial_call, const string &description,
                         bool is_option, bool is_option_bool)
    : _initial_call(initial_call), _description(description),
      _is_option(is_option), _is_option_bool(is_option_bool) {}

cli_command::cli_command(const string &initial_call, const string &description)
    : cli_command(initial_call, description, false, false) {}

cli_command &cli_command::add_command(const string &name,
                                      const string &description) {
  if (this->_is_option) {
    throw cli_parser_exception("Can't add a subcommand to an option");
  }
  if (this->_options.size() > 0) {
    throw cli_parser_exception(
        "Can't add a subcommand as it already has options");
  }
  if (this->_commands.find(name) != this->_commands.end()) {
    throw cli_parser_exception("Can't add subcommand '" + name +
                               "' because it already exists");
  }
  this->_commands.try_emplace(name, this->_initial_call + " " + name,
                              description, false, false);
  return this->_commands.find(name)->second;
}

void cli_command::add_option(const string &name, const string &description,
                             bool is_option_bool) {
  if (this->_is_option) {
    throw cli_parser_exception("Can't add an option to an option");
  }
  // if (this->_commands.size() > 0) {
  //   throw cli_parser_exception(
  //       "Can't add an option as it already has commands");
  // }
  if (this->_options.find(name) != this->_options.end()) {
    throw cli_parser_exception("Can't add option '" + name +
                               "' because it already exists");
  }
  this->_options.try_emplace(name, "", description, true, is_option_bool);
}

cli_command &cli_command::get_command(const string &name) {
  if (this->_commands.find(name) == this->_commands.end()) {
    throw cli_parser_exception("Can't find subcommand '" + name + "'");
  }
  return this->_commands.find(name)->second;
}

cli_command &cli_command::get_option(const string &name) {
  if (this->_options.find(name) == this->_options.end()) {
    throw cli_parser_exception("Can't find option '" + name + "'");
  }
  return this->_options.find(name)->second;
}

const cli_command &cli_command::get_command(const string &name) const {
  if (this->_commands.find(name) == this->_commands.end()) {
    throw cli_parser_exception("Can't find subcommand '" + name + "'");
  }
  return this->_commands.find(name)->second;
}

const cli_command &cli_command::get_option(const string &name) const {
  if (this->_options.find(name) == this->_options.end()) {
    throw cli_parser_exception("Can't find option '" + name + "'");
  }
  return this->_options.find(name)->second;
}

const map<string, cli_command> &cli_command::get_commands() const {
  return this->_commands;
}
const map<string, cli_command> &cli_command::get_options() const {
  return this->_options;
}

const string &cli_command::get_description() const {
  return this->_description;
}

bool cli_command::is_option() const { return this->_is_option; }

bool cli_command::is_option_bool() const {
  if (!this->_is_option) {
    throw cli_parser_exception("Can't have an option state in a subcommand");
  }
  return this->_is_option_bool;
}

bool cli_command::is_terminal() const { return this->_commands.size() == 0; }

string cli_command::get_help() const {
  stringstream ss;

  ss << "Current call: " << _initial_call << endl;
  ss << "Description: " << _description << endl;
  ss << "-h: displays this help";

  if (_options.size() > 0) {
    ss << endl << endl << _initial_call << " [option(s)]...";
    for (const auto &option : _options) {
      ss << endl << "--" << option.first;
      if (!option.second.is_option_bool()) {
        ss << " \"string\"";
      }
      ss << ": " << option.second.get_description();
    }
  }

  if (_commands.size() > 0) {
    ss << endl << endl << _initial_call << " [subcommand]";
    for (const auto &sub : _commands) {
      ss << endl << sub.first << ": " << sub.second.get_description();
    }
  }

  return ss.str();
}

tuple<vector<string>, map<string, string>, map<string, bool>>
cli_parser::parse(const cli_command &root, vector<string> cli_args) {

  if (cli_args.size() > 0 && cli_args.at(0) == "-h") {
    cout << root.get_help() << endl;
    throw cli_parser_help_asked_exception();
  }

  try {

    if (root.is_terminal()) {
      auto res = cli_parser::parse_terminal(root, cli_args);
      return make_tuple(vector<string>(), res.first, res.second);
    } else {

      const string first_arg = cli_args.at(0);
      cli_args.erase(cli_args.begin());
      const cli_command &next_command = root.get_command(first_arg);
      auto res = cli_parser::parse(next_command, cli_args);
      std::get<0>(res).insert(std::get<0>(res).begin(), first_arg);
      return res;
    }

  } catch (const cli_parser_exception &e) {
    cout << "Wrong command line, displaying help." << endl;
    cout << root.get_help() << endl;
    throw cli_parser_bad_parse_exception(e.what());
  } catch (const std::out_of_range &e) {
    cout << "Argument missing, displaying help." << endl;
    cout << root.get_help() << endl;
    throw cli_parser_bad_parse_exception(e.what());
  }
}

pair<map<string, string>, map<string, bool>>
cli_parser::parse_terminal(const cli_command &root,
                           const vector<string> &cli_args) {
  map<string, string> string_args;
  map<string, bool> bool_args;
  const auto &available_options = root.get_options();

  for (const auto &el : available_options) {
    if (el.second.is_option_bool()) {
      bool_args.insert_or_assign(el.first, false);
    }
  }

  for (long unsigned int i = 0; i < cli_args.size(); ++i) {

    string first_two = cli_args.at(i).substr(0, 2);
    if (first_two != "--") {
      throw cli_parser_exception("Unexpected parameter not preceded by --");
    }

    string option_name = cli_args.at(i).substr(2);
    const cli_command &option = root.get_option(option_name);

    if (option.is_option_bool()) {
      bool_args.at(option_name) = true;
    } else {
      string_args.insert_or_assign(option_name, cli_args.at(i + 1));
      ++i;
    }
  }

  return make_pair(string_args, bool_args);
}
