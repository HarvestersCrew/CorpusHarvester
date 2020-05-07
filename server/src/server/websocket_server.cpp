#include "server/websocket_server.h"

websocketpp::server<websocketpp::config::asio> WebsocketServer::_server;
map<connection_hdl, shared_ptr<ConnectionData>, owner_less<connection_hdl>>
    WebsocketServer::_websockets;
unsigned int WebsocketServer::_port;
mutex WebsocketServer::_connections_mut;
WssLogstream WebsocketServer::_ls;
ostream WebsocketServer::_os(&WebsocketServer::_ls);
shared_ptr<seasocks::PrintfLogger> WebsocketServer::_file_server_logger =
    make_shared<seasocks::PrintfLogger>();
shared_ptr<seasocks::Server> WebsocketServer::_file_server;
unsigned int WebsocketServer::_file_server_port;
thread WebsocketServer::_file_server_thread;

bool WebsocketServer::init(unsigned int port, unsigned int file_port) {
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

  _port = port;
  _server.listen(_port);

  _server.start_accept();

  _file_server_port = file_port;
  _file_server = make_shared<seasocks::Server>(_file_server_logger);

  return true;
}

void WebsocketServer::run() {
  logger::info("Starting server on port " + to_string(_port));
  std::cout << "Starting server on port " + to_string(_port) << std::endl;
  if (logger::get_output() == logger::output::FILE) {
    std::cout << "Logger is now outputting to " << logger::get_output_path()
              << std::endl;
  }

  _file_server_thread = thread{WebsocketServer::run_file_server};
  logger::info("Started file server on port " + to_string(_file_server_port));

  _server.run();
}

void WebsocketServer::run_file_server() {
  Storage storage;
  string path = storage.get_root_folder_name();
  _file_server->serve(path.c_str(), _file_server_port);
}

bool WebsocketServer::send_error_json(const connection_hdl &hdl,
                                      const ExceptionWrapper &e,
                                      const optional<string> token) {
  json to_send;
  to_send["type"] = "error";
  to_send["data"] = e.exception_name();
  if (token)
    to_send["token"] = token.value();
  logger::error(e.exception_name() + ": " + e.what());
  return WebsocketServer::send_msg(hdl, to_send.dump());
}

bool WebsocketServer::send_json(const connection_hdl &hdl, const string &type,
                                const json &j, const optional<string> token) {
  json to_send;
  to_send["type"] = type;
  to_send["data"] = j;
  if (token)
    to_send["token"] = token.value();
  return WebsocketServer::send_msg(hdl, to_send.dump());
}

void WebsocketServer::broadcast_json(const string &type, const json &j) {
  for (auto it = _websockets.begin(); it != _websockets.end(); ++it) {
    WebsocketServer::send_json(it->first, type, j, nullopt);
  }
}

void WebsocketServer::broadcast_json(const pair<string, json> broadcast) {
  WebsocketServer::broadcast_json(broadcast.first, broadcast.second);
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

ConnectionData &WebsocketServer::get_data_ref(const connection_hdl &hdl) {
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
  WebsocketServer::send_json(hdl, "welcome", j, nullopt);
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
    WebsocketServer::send_error_json(hdl, e, nullopt);
  }
}

void WebsocketServer::handle_message(ConnectionData &con, const string msg) {
  optional<string> token = nullopt;
  try {
    json j = json::parse(msg);
    if (j.contains("token")) {
      token = j.at("token").get<string>();
    }
    auto res = server_handler::dispatch_request(con, j);
    WebsocketServer::send_json(con._hdl, res.first, res.second, token);

  } catch (const json::parse_error &e) {
    WebsocketServer::send_error_json(con._hdl, wss_invalid_json(), token);

  } catch (const json::out_of_range &e) {
    WebsocketServer::send_error_json(con._hdl, wss_invalid_json(), token);

  } catch (const ExceptionWrapper &e) {
    WebsocketServer::send_error_json(con._hdl, e, token);

  } catch (const std::exception &e) {
    stringstream ss;
    ss << "Encountered " << typeid(e).name() << ": " << e.what();
    WebsocketServer::send_error_json(
        con._hdl, ExceptionWrapper(ss.str(), "Unknown"), token);
  }
}
