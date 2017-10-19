// Copyright 2010 Dean Michael Berris.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <boost/network/include/http/client.hpp>
#include "client_types.hpp"

namespace net = boost::network;
namespace http = boost::network::http;

TYPED_TEST_CASE(HTTPClientTest, ClientTypes);

TYPED_TEST(HTTPClientTest, GetTest) {
  using client = TypeParam;
  typename client::request request("http://cpp-netlib.org/");
  client client_;
  typename client::response response;
  ASSERT_NO_THROW(response = client_.get(request));
  try {
    auto data = body(response);
    std::cout << data;
  } catch (...) {
    FAIL() << "Caught exception while retrieving body from GET request";
  }
  EXPECT_EQ("HTTP/1.", response.version().substr(0, 7));
  EXPECT_TRUE(response.status() == 200u ||
              (response.status() >= 300 && response.status() < 400));
}

#ifdef BOOST_NETWORK_ENABLE_HTTPS

TYPED_TEST(HTTPClientTest, GetHTTPSTest) {
  using client = TypeParam;
  typename client::request request("https://www.github.com/");
  client client_;
  typename client::response response = client_.get(request);
  EXPECT_TRUE(response.status() == 200 ||
              (response.status() >= 300 && response.status() < 400));
  try {
    auto data = body(response);
    std::cout << data;
  } catch (...) {
    FAIL() << "Caught exception while retrieving body from GET request";
  }
}

#endif

TYPED_TEST(HTTPClientTest, TemporaryClientObjectTest) {
  using client = TypeParam;
  typename client::request request("http://cpp-netlib.org/");
  typename client::response response;
  ASSERT_NO_THROW(response = client().get(request));
  auto range = headers(response);
  ASSERT_TRUE(!boost::empty(range));
  try {
    auto data = body(response);
    std::cout << data;
  } catch (...) {
    FAIL() << "Caught exception while retrieving body from GET request";
  }
  EXPECT_EQ("HTTP/1.", response.version().substr(0, 7));
  EXPECT_TRUE(response.status() == 200u ||
              (response.status() >= 300 && response.status() < 400));
}


typedef boost::network::http::basic_client<boost::network::http::tags::http_async_8bit_tcp_resolve, 1, 1> async_client;
#define EXC_PTR(cmd) try { cmd } catch (const std::exception_ptr& ex) { std::rethrow_exception(ex); }

TYPED_TEST(HTTPClientTest, ReuseResponse) {

  async_client client;

  async_client::request req("https://static.deepomatic.com/compass/NVR_ch2_J_20161209122514_20161209122514_orig.jpg");

  req.sni_hostname(req.host());

  auto response = client.get(req);

  while (!ready(response));

  EXC_PTR(response.status(););

  // we get the expected headers and body
  auto hdrs = headers(response);

  for (auto &h : hdrs) {
      std::cout << h.first << ": " << h.second << std::endl;
  }

  std::cout << "---" << std::endl;

  req.uri("http://example.com"); // changing the url

  response = client.get(req);

  while (!ready(response));

  // here the headers are the same as the previous response, they should be those from example.com
  auto hdrs2 = headers(response);

  for (auto &h : hdrs2) {
      std::cout << h.first << ": " << h.second << std::endl;
  }

}
