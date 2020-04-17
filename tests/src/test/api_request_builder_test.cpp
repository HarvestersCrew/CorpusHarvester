#include "test/api_request_builder_test.h"

void test_types() {
  ApiDownloadBuilder a;
  Assertion::assert_equals(__FUNCTION__, 0, a.get_types().size());

  a.add_type(api_loader::api_type::IMAGE);
  Assertion::assert_equals(__FUNCTION__, 1, a.get_types().size());
  Assertion::assert_equals(__FUNCTION__, api_loader::api_type::IMAGE,
                           *a.get_types().find(api_loader::api_type::IMAGE));

  a.add_type(api_loader::api_type::IMAGE);
  Assertion::assert_equals(__FUNCTION__, 1, a.get_types().size());

  a.add_type(api_loader::api_type::TEXT);
  Assertion::assert_equals(__FUNCTION__, 2, a.get_types().size());
  Assertion::assert_equals(__FUNCTION__, api_loader::api_type::IMAGE,
                           *a.get_types().find(api_loader::api_type::IMAGE));
  Assertion::assert_equals(__FUNCTION__, api_loader::api_type::TEXT,
                           *a.get_types().find(api_loader::api_type::TEXT));

  try {
    a.add_type((api_loader::api_type)57);
    Assertion::assert_throw(__FUNCTION__, "api_request_builder_invalid_type");
  } catch (const api_request_builder_invalid_type &e) {
  }
  Assertion::assert_equals(__FUNCTION__, 2, a.get_types().size());

  a.clear_types();
  Assertion::assert_equals(__FUNCTION__, 0, a.get_types().size());
}

void test_requests() {
  ApiDownloadBuilder a;
  Assertion::assert_equals(__FUNCTION__, 0, a.get_requests().size());

  try {
    a.add_request("fake_api");
    Assertion::assert_throw(__FUNCTION__, "api_factory_name_not_found");
  } catch (const api_factory_name_not_found &e) {
  }

  long unsigned int idx = a.add_request("Twitter");

  try {
    a.add_request_parameter(idx, "q", "val", "=");
    Assertion::assert_throw(__FUNCTION__, "api_no_setting_exception", __LINE__);
  } catch (const api_no_setting_exception &e) {
  }

  try {
    a.add_request_parameter(idx, "query", "val", "<");
    Assertion::assert_throw(__FUNCTION__, "api_builder_incompatible_operator",
                            __LINE__);
  } catch (const api_builder_incompatible_operator &e) {
  }

  a.add_request_parameter(idx, "query", "val", "=");
  Assertion::assert_equals(__FUNCTION__, 1, a.get_requests().size());
  Assertion::assert_equals(__FUNCTION__, "Twitter",
                           a.get_requests().at(0).first->get_name());
  Assertion::assert_equals(__FUNCTION__, 1,
                           a.get_requests().at(0).second.size());

  a.clear_requests();
  Assertion::assert_equals(__FUNCTION__, 0, a.get_requests().size());
}

void test_builder_clear() {
  ApiDownloadBuilder a;
  a.add_type(api_loader::api_type::IMAGE);
  a.add_request("Twitter");
  Assertion::assert_equals(__FUNCTION__, 1, a.get_requests().size());
  Assertion::assert_equals(__FUNCTION__, 1, a.get_types().size());
  a.clear_all();
  Assertion::assert_equals(__FUNCTION__, 0, a.get_requests().size());
  Assertion::assert_equals(__FUNCTION__, 0, a.get_types().size());
}

void test_api_dl_remove_ops() {
  ApiDownloadBuilder a;
  long unsigned int idx = a.add_request("Twitter");
  a.add_request_parameter(idx, "query", "cats", "=");
  auto res = a.get_no_op_requests(a.get_requests());
  Assertion::assert_equals(__FUNCTION__, 1, res.size());
  Assertion::assert_equals(__FUNCTION__, "Twitter",
                           res.at(0).first->get_name());
  Assertion::assert_equals(__FUNCTION__, 1, res.at(0).second.size());
  Assertion::assert_equals(__FUNCTION__, "query",
                           res.at(0).second.find("query")->first);
  Assertion::assert_equals(__FUNCTION__, "cats",
                           res.at(0).second.find("query")->second);
}

void api_request_builder_test() {
  std::cout << std::endl << "APIs request builder tests : " << std::endl;
  Assertion::test(test_types, "test_types");
  Assertion::test(test_requests, "test_requests");
  Assertion::test(test_builder_clear, "test_builder_clear");
  Assertion::test(test_api_dl_remove_ops, "test_api_dl_remove_ops");
}
