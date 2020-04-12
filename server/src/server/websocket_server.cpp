#include "server/websocket_server.h"

websocketpp::server<websocketpp::config::asio> WebsocketServer::_server;
map<connection_hdl, shared_ptr<ConnectionData>, owner_less<connection_hdl>>
    WebsocketServer::_websockets;
unsigned int WebsocketServer::_port = 9002;
mutex WebsocketServer::_connections_mut;
WssLogstream WebsocketServer::_ls;
ostream WebsocketServer::_os(&WebsocketServer::_ls);

bool WebsocketServer::init() {
  _server.init_asio();
  _server.set_reuse_addr(true);

  logger::add_custom_output(make_shared<BroadcastLogOutput>());

  _server.get_alog().set_ostream(&_os);
  _server.get_elog().set_ostream(&_os);
  _server.set_error_channels(websocketpp::log::elevel::none);
  _server.set_access_channels(websocketpp::log::alevel::none);

  _server.set_open_handler(&WebsocketServer::on_open);
  _server.set_close_handler(&WebsocketServer::on_close);
  _server.set_message_handler(&WebsocketServer::on_message);

  _server.listen(_port);

  _server.start_accept();

  return true;
}

void WebsocketServer::run() {
  logger::info("Starting server on port " + to_string(_port));
  _server.run();
}

bool WebsocketServer::send_error_json(const connection_hdl &hdl,
                                      const ExceptionWrapper &e) {
  json to_send;
  to_send["type"] = "error";
  to_send["data"] = e.exception_name();
  logger::error(e.exception_name() + ": " + e.what());
  return WebsocketServer::send_msg(hdl, to_send.dump());
}

bool WebsocketServer::send_json(const connection_hdl &hdl, const string &type,
                                const json &j) {
  json to_send;
  to_send["type"] = type;
  to_send["data"] = j;
  return WebsocketServer::send_msg(hdl, to_send.dump());
}

void WebsocketServer::broadcast_json(const string &type, const json &j) {
  for (auto it = _websockets.begin(); it != _websockets.end(); ++it) {
    WebsocketServer::send_json(it->first, type, j);
  }
}

bool WebsocketServer::send_msg(const connection_hdl &hdl, const string &msg) {
  websocketpp::lib::error_code ec;
  _server.send(hdl, msg, websocketpp::frame::opcode::text,
               ec); // send text message.
  if (ec) {
    logger::error(ec.message());
    return false;
  }
  return true;
}

const ConnectionData &WebsocketServer::get_data_ref(const connection_hdl &hdl) {
  if (_websockets.find(hdl) == _websockets.end()) {
    throw wss_cant_find_handler();
  }
  return *(_websockets.find(hdl)->second);
}

void WebsocketServer::stop() {
  websocketpp::lib::error_code ec;
  _server.stop_listening(ec);
  if (ec) {
    logger::error("Failed to stop server listening: " + ec.message());
    return;
  }
  // Close all existing websocket connections.
  string data = "Terminating connection...";
  for (auto it = _websockets.begin(); it != _websockets.end(); ++it) {
    websocketpp::lib::error_code ec;
    _server.close(it->first, websocketpp::close::status::normal, data,
                  ec); // send text message.
    if (ec) {
      logger::error("Failed to stop server listening: " + ec.message());
    }
  }
  // Stop the endpoint.
  _server.stop();
}

bool WebsocketServer::on_open(connection_hdl hdl) {
  const lock_guard<mutex> lock(_connections_mut);
  _websockets.emplace(hdl, make_shared<ConnectionData>());
  _websockets[hdl]->_hdl = hdl;
  logger::debug("New connection to server");
  json j;
  j["hello"] = "world";
  WebsocketServer::send_json(hdl, "welcome", j);
  return true;
}

void WebsocketServer::on_close(connection_hdl hdl) {
  const lock_guard<mutex> lock(_connections_mut);
  if (_websockets.erase(hdl)) {
    logger::debug("Successful disconnection");
  } else {
    logger::error("Unsuccessful disconnection");
  }
}

void WebsocketServer::on_message(
    connection_hdl hdl,
    websocketpp::server<websocketpp::config::asio>::message_ptr message) {
  // Tries to send the request to the handler with the reference to the
  // connection in our login map
  try {
    // Create a thread to handle the query
    thread t(&WebsocketServer::handle_message,
             std::ref(WebsocketServer::get_data_ref(hdl)),
             message->get_payload());
    t.detach();
  } catch (const wss_cant_find_handler &e) {
    WebsocketServer::send_error_json(hdl, e);
  }
}

void WebsocketServer::handle_message(const ConnectionData &con,
                                     const string msg) {
  try {
    json j = json::parse(msg);
    auto res = server_handler::dispatch_request(con, j);
    WebsocketServer::send_json(con._hdl, res.first, res.second);

  } catch (const json::parse_error &e) {
    WebsocketServer::send_error_json(con._hdl, wss_invalid_json());

  } catch (const json::out_of_range &e) {
    WebsocketServer::send_error_json(con._hdl, wss_invalid_json());

  } catch (const ExceptionWrapper &e) {
    WebsocketServer::send_error_json(con._hdl, e);

  } catch (const std::exception &e) {
    stringstream ss;
    ss << "Encountered an error: " << typeid(e).name();
    WebsocketServer::send_error_json(con._hdl,
                                     ExceptionWrapper(ss.str(), "Unknown"));
  }
}
