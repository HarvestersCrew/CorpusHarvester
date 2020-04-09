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

  unordered_map<string, string> m;

  m.emplace("q", "cats");
  a.add_request("Twitter", m);
  Assertion::assert_equals(__FUNCTION__, 1, a.get_requests().size());
  Assertion::assert_equals(__FUNCTION__, "Twitter",
                           a.get_requests().at(0).first->get_name());
  Assertion::assert_equals(__FUNCTION__, 1,
                           a.get_requests().at(0).second.size());

  a.clear_requests();
  Assertion::assert_equals(__FUNCTION__, 0, a.get_requests().size());
}

void api_request_builder_test() {
  std::cout << std::endl << "APIs request builder tests : " << std::endl;
  Assertion::test(test_types, "test_types");
  Assertion::test(test_requests, "test_requests");
}
