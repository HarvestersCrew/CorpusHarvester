#include "test/api_parameter_test.h"

void test_api_parameter_base_construct() {
  json j;

  try {
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::type_error");
  } catch (const json::type_error &e) {
  }

  try {
    j = {{"blabla", "ezrgbtrb"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range");
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range");
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"}, {"relevant", true}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range");
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"},
         {"relevant", true},
         {"type", "fake_type"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "std::runtime_error");
  } catch (const std::runtime_error &e) {
  }

  j = {{"api_name", "test_api_name"}, {"relevant", true}, {"type", "int"}};
  api_parameter_base param(j);
}

void test_api_paramter_base_values() {
  json j = {{"api_name", "test_api_name"}, {"relevant", true}, {"type", "int"}};
  api_parameter_base param1(j);

  Assertion::assert_equals(__FUNCTION__, "test_api_name", param1._api_name);
  Assertion::assert_equals(__FUNCTION__, true, param1._relevant);
  Assertion::assert_equals(__FUNCTION__, api_parameter_base::value_type::INT,
                           param1._value_type);
  Assertion::assert_equals(__FUNCTION__, "test_api_name", param1._name);

  j["name"] = "test_name";
  api_parameter_base param2(j);
  Assertion::assert_equals(__FUNCTION__, "test_name", param2._name);
}

void api_parameter_test() {
  std::cout << std::endl << "API parameters tests : " << std::endl;
  Assertion::test(test_api_parameter_base_construct,
                  "test_api_parameter_base_construct");
  Assertion::test(test_api_paramter_base_values,
                  "test_api_paramter_base_values");
}
