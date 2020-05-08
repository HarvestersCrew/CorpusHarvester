#include "test/api_parameter_test.h"

void test_api_parameter_base_construct() {
  json j;

  try {
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::type_error", __LINE__);
  } catch (const json::type_error &e) {
  }

  try {
    j = {{"blabla", "ezrgbtrb"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range", __LINE__);
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range", __LINE__);
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"}, {"relevant", true}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range", __LINE__);
  } catch (const json::out_of_range &e) {
  }

  try {
    j = {{"api_name", "test_api_name"},
         {"relevant", true},
         {"type", "fake_type"}};
    api_parameter_base param(j);
    Assertion::assert_throw(__FUNCTION__, "std::runtime_error", __LINE__);
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

void test_api_parameter_request_construct() {
  json j = {{"api_name", "test_api_name"}, {"relevant", true}, {"type", "int"}};

  try {
    api_parameter_request param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range", __LINE__);
  } catch (const json::out_of_range &e) {
  }

  j["position"] = "header";
  try {
    api_parameter_request param(j);
    Assertion::assert_throw(__FUNCTION__, "json::out_of_range", __LINE__);
  } catch (const json::out_of_range &e) {
  }

  j["required"] = true;
  api_parameter_request param1(j);

  Assertion::assert_equals(__FUNCTION__, "header", param1._position);
  Assertion::assert_equals(__FUNCTION__, true, param1.get_required());
  Assertion::assert_equals(__FUNCTION__, false,
                           param1.get_default_value().has_value());
  Assertion::assert_equals(__FUNCTION__, 0, param1.get_values().size());

  j["default_value"] = "test_default_value";

  try {
    api_parameter_request param(j);
    Assertion::assert_throw(__FUNCTION__, "api_parameter_incompatible_value",
                            __LINE__);
  } catch (const api_parameter_incompatible_value &e) {
  }

  j["default_value"] = "5";
  j["values"] = {"1", "42"};

  try {
    api_parameter_request param(j);
    Assertion::assert_throw(__FUNCTION__, "api_parameter_incompatible_value",
                            __LINE__);
  } catch (const api_parameter_incompatible_value &e) {
  }

  j["default_value"] = "42";
  api_parameter_request param2(j);

  Assertion::assert_equals(__FUNCTION__, true,
                           param2.get_default_value().has_value());
  Assertion::assert_equals(__FUNCTION__, "42",
                           param2.get_default_value().value());
}

void test_api_parameter_response_construct() {
  json j = {{"api_name", "test_api_name"}, {"relevant", true}, {"type", "int"}};

  api_parameter_response param1(j);
  Assertion::assert_equals(__FUNCTION__, 0, param1._string_appends.size());
  Assertion::assert_equals(__FUNCTION__, 0, param1._string_prepends.size());

  j["string_prepends"] = {{{"value", "a"}, {"is_parameter_name", true}},
                          {{"value", "b"}, {"is_parameter_name", false}}};
  api_parameter_response param2(j);
  Assertion::assert_equals(__FUNCTION__, 0, param2._string_appends.size());
  Assertion::assert_equals(__FUNCTION__, 2, param2._string_prepends.size());
  Assertion::assert_equals(__FUNCTION__, "a",
                           param2._string_prepends.at(0).first);
  Assertion::assert_equals(__FUNCTION__, "b",
                           param2._string_prepends.at(1).first);
  Assertion::assert_true(__FUNCTION__, param2._string_prepends.at(0).second);
  Assertion::assert_false(__FUNCTION__, param2._string_prepends.at(1).second);

  j["string_appends"] = {{{"value", "a"}, {"is_parameter_name", true}},
                         {{"value", "b"}, {"is_parameter_name", false}}};
  api_parameter_response param3(j);
  Assertion::assert_equals(__FUNCTION__, 2, param3._string_appends.size());
  Assertion::assert_equals(__FUNCTION__, 2, param3._string_prepends.size());
  Assertion::assert_equals(__FUNCTION__, "a",
                           param3._string_appends.at(0).first);
  Assertion::assert_equals(__FUNCTION__, "b",
                           param3._string_appends.at(1).first);
  Assertion::assert_true(__FUNCTION__, param3._string_appends.at(0).second);
  Assertion::assert_false(__FUNCTION__, param3._string_appends.at(1).second);
}

void api_parameter_test() {
  std::cout << std::endl << "API parameters tests : " << std::endl;
  Assertion::test(test_api_parameter_base_construct,
                  "test_api_parameter_base_construct");
  Assertion::test(test_api_paramter_base_values,
                  "test_api_paramter_base_values");
  Assertion::test(test_api_parameter_request_construct,
                  "test_api_parameter_request_construct");
  Assertion::test(test_api_parameter_response_construct,
                  "test_api_parameter_response_construct");
}
