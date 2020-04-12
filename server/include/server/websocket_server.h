#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#define ASIO_STANDALONE

#include "api/ManagerRequest.h"
#include "server/broadcast_log_output.h"
#include "server/server_handler.h"
#include "server/wss_logstream.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/nlohmann/json.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <typeinfo>
#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using nlohmann::json;
using std::lock_guard;
using std::make_pair;
using std::make_shared;
using std::map;
using std::mutex;
using std::ostream;
using std::owner_less;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::thread;
using std::to_string;
using websocketpp::connection_hdl;

struct ConnectionData {
  ManagerRequest _mr;
};

/**
 * Websocket server, heavily inspired by
 * https://mayaposch.wordpress.com/2015/09/16/creating-a-websocket-server-with-websocket/
 */
class WebsocketServer {
public:
  static bool init();
  static void run();
  static void stop();

  static bool send_error_json(const connection_hdl &hdl,
                              const ExceptionWrapper &e);
  static bool send_json(const connection_hdl &hdl, const string &type,
                        const json &j);
  static void broadcast_json(const string &type, const json &j);

  static bool send_msg(const connection_hdl &hdl, const string &msg);

private:
  WebsocketServer() {}

  static websocketpp::server<websocketpp::config::asio> _server;
  static map<connection_hdl, shared_ptr<ConnectionData>,
             owner_less<connection_hdl>>
      _websockets;
  static unsigned int _port;
  static mutex _connections_mut;
  static WssLogstream _ls;
  static ostream _os;

  static const connection_hdl &get_handle_ref(connection_hdl hdl);
  static const ConnectionData &get_data_ref(const connection_hdl &hdl);

  static void handle_message(const connection_hdl &hdl, const string msg);

  // callbacks
  static bool on_open(connection_hdl hdl);
  static void on_close(connection_hdl hdl);
  static void on_message(
      connection_hdl hdl,
      websocketpp::server<websocketpp::config::asio>::message_ptr message);
};

#endif
