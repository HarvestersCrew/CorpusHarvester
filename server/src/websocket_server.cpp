#include "websocket_server.h"

websocketpp::server<websocketpp::config::asio> WebsocketServer::_server;
map<connection_hdl, connection_data, owner_less<connection_hdl>>
    WebsocketServer::_websockets;
unsigned int WebsocketServer::_port = 9002;
mutex WebsocketServer::_connections_mut;

bool WebsocketServer::init() {
  _server.init_asio();
  _server.set_reuse_addr(true);

  _server.set_open_handler(&WebsocketServer::on_open);
  _server.set_close_handler(&WebsocketServer::on_close);

  _server.listen(_port);

  _server.start_accept();

  return true;
}

void WebsocketServer::run() {
  logger::info("Starting server on port " + to_string(_port));
  _server.run();
}

bool WebsocketServer::send_json(const connection_hdl &hdl, const json &j) {
  websocketpp::lib::error_code ec;
  _server.send(hdl, j.dump(), websocketpp::frame::opcode::text,
               ec); // send text message.
  if (ec) {
    logger::error(ec.message());
    return false;
  }
  return true;
}

const connection_hdl &WebsocketServer::get_handle_ref(connection_hdl hdl) {
  if (_websockets.find(hdl) == _websockets.end()) {
    throw wss_cant_find_handler();
  }
  return _websockets.find(hdl)->first;
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
  _websockets[hdl] = connection_data();
  logger::debug("New connection to server");
  json j;
  j["hello"] = "world";
  WebsocketServer::send_json(hdl, j);
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
