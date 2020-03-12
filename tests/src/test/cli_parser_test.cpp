#include "test/cli_parser_test.h"

void test_cli_parser_construction() {
  cli_command root("initial", "my description");
  Assertion::assert_false(__FUNCTION__, root.is_option());
  Assertion::assert_equals(__FUNCTION__, "my description",
                           root.get_description());
}

void test_cli_parser_is_terminal() {
  cli_command root("initial", "my description");
  Assertion::assert_true(__FUNCTION__, root.is_terminal());
  cli_command &child =
      root.add_command("my_command", "description of subcommand");
  Assertion::assert_true(__FUNCTION__, child.is_terminal());
  Assertion::assert_false(__FUNCTION__, root.is_terminal());
  child.add_option("option", "desc", true);
  Assertion::assert_true(__FUNCTION__, child.is_terminal());
}

void test_cli_parser_add_command() {
  cli_command root("initial", "my description");
  cli_command &child = root.add_command("subcommand", "desc");
  Assertion::assert_equals(__FUNCTION__, &child,
                           &(root.get_command("subcommand")));
  child.add_option("option", "desc", false);
  try {
    child.add_command("wrong_command", "desc");
    Assertion::assert_throw(__FUNCTION__, "cli_parser_exception");
  } catch (const cli_parser_exception &e) {
  }

  cli_command option("init_option", "desc", true, false);
  try {
    option.add_command("wrong_command", "desc");
    Assertion::assert_throw(__FUNCTION__, "cli_parser_exception");
  } catch (const cli_parser_exception &e) {
  }

  try {
    root.add_command("subcommand", "desc");
    Assertion::assert_throw(__FUNCTION__, "cli_parser_exception");
  } catch (const std::exception &e) {
  }
}

void test_cli_parser_add_option() {
  cli_command root("initial", "my description");
  root.add_command("command", "desc");
  try {
    root.add_option("wrong_option", "desc", false);
    Assertion::assert_throw(__FUNCTION__, "cli_parser_exception");
  } catch (const cli_parser_exception &e) {
  }

  cli_command root2("initial", "my description");
  root2.add_option("option", "desc", false);
  try {
    root2.add_option("option", "desc", false);
    Assertion::assert_throw(__FUNCTION__, "cli_parser_exception");
  } catch (const cli_parser_exception &e) {
  }
}

void test_cli_parser_parser() {
  vector<string> subcommands;
  map<string, string> string_args;
  map<string, bool> bool_args;

  cli_command root("initial", "desc");
  std::tie(subcommands, string_args, bool_args) =
      cli_parser::parse(root, vector<string>());
  Assertion::assert_equals(__FUNCTION__, 0, subcommands.size());
  Assertion::assert_equals(__FUNCTION__, 0, string_args.size());
  Assertion::assert_equals(__FUNCTION__, 0, bool_args.size());

  try {
    cli_parser::parse(root, vector<string>({"-h"}));
    Assertion::assert_throw(__FUNCTION__, "cli_parser_help_asked_exception");
  } catch (const cli_parser_help_asked_exception &e) {
  }

  try {
    cli_parser::parse(root, vector<string>({"wrong_subcommand"}));
    Assertion::assert_throw(__FUNCTION__, "cli_parser_bad_parse_exception");
  } catch (const cli_parser_bad_parse_exception &e) {
  }

  root.add_command("subcommand1", "subcommand1 desc");
  cli_command &sub2 = root.add_command("subcommand2", "subcommand2 desc");
  sub2.add_option("option1", "option1 desc", true);
  sub2.add_option("option2", "option2 desc", false);
  sub2.add_option("option3", "option1 desc", true);

  try {
    cli_parser::parse(root, vector<string>({"subcommand2", "-h"}));
    Assertion::assert_throw(__FUNCTION__, "cli_parser_help_asked_exception");
  } catch (const cli_parser_help_asked_exception &e) {
  }

  std::tie(subcommands, string_args, bool_args) = cli_parser::parse(
      root, vector<string>({"subcommand2", "--option2", "value", "--option1"}));
  Assertion::assert_equals(__FUNCTION__, 1, subcommands.size());
  Assertion::assert_equals(__FUNCTION__, 1, string_args.size());
  Assertion::assert_equals(__FUNCTION__, 2, bool_args.size());
  Assertion::assert_equals(__FUNCTION__, "subcommand2", subcommands.at(0));
  Assertion::assert_equals(__FUNCTION__, "value",
                           string_args.find("option2")->second);
  Assertion::assert_equals(__FUNCTION__, true,
                           bool_args.find("option1")->second);
  Assertion::assert_equals(__FUNCTION__, false,
                           bool_args.find("option3")->second);
}

std::pair<int, int> cli_parser_test() {
  int function_count = 5;
  void (*test_functions[])(void) = {
      test_cli_parser_construction, test_cli_parser_is_terminal,
      test_cli_parser_add_command, test_cli_parser_add_option,
      test_cli_parser_parser};
  std::string test_functions_name[] = {
      "test_cli_parser_construction", "test_cli_parser_is_terminal",
      "test_cli_parser_add_command", "test_cli_parser_add_option",
      "test_cli_parser_parser"};

  std::cout << std::endl << "CLI parser tests : " << std::endl;
  return Assertion::test_all(test_functions, test_functions_name,
                             function_count);
}
