#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#define ASIO_STANDALONE

#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/nlohmann/json.hpp"
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using nlohmann::json;
using std::lock_guard;
using std::make_pair;
using std::map;
using std::mutex;
using std::owner_less;
using std::pair;
using std::string;
using std::to_string;
using websocketpp::connection_hdl;

struct connection_data {};

/**
 * Websocket server, heavily inspired by
 * https://mayaposch.wordpress.com/2015/09/16/creating-a-websocket-server-with-websocket/
 */
class WebsocketServer {
public:
  static bool init();
  static void run();
  static void stop();

  static bool send_json(const connection_hdl &hdl, const json &j);

private:
  WebsocketServer() {}

  static websocketpp::server<websocketpp::config::asio> _server;
  static map<connection_hdl, connection_data, owner_less<connection_hdl>>
      _websockets;
  static unsigned int _port;
  static mutex _connections_mut;

  static const connection_hdl &get_handle_ref(connection_hdl hdl);

  // callbacks
  static bool on_open(connection_hdl hdl);
  static void on_close(connection_hdl hdl);
};

#endif
