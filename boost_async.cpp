#include <asyncServer.hpp>

std::string osci::make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

// Tcp connection declarations.
static osci::tcp_connection::pointer create(boost::asio::io_service& io_service) {
  return pointer(new tcp_connection(io_service));
}

tcp::socket& osci::tcp_connection::socket() {
  return socket_;
}

void osci::tcp_connection::start() {
  message_ = osci::make_daytime_string();

  boost::asio::async_write(socket_, boost::asio::buffer(message_),
      boost::bind(&osci::tcp_connection::handle_write, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

// Tcp server class declarations.
osci::tcp_server::tcp_server(boost::asio::io_service& io_service)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), 1090)) {
  
  start_accept();
}

void osci::tcp_server::start_accept() {
  osci::tcp_connection::pointer new_connection =
    osci::tcp_connection::create(acceptor_.get_io_service());

  acceptor_.async_accept(new_connection->socket(),
      boost::bind(&osci::tcp_server::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
}

void osci::tcp_server::handle_accept(osci::tcp_connection::pointer new_connection, const boost::system::error_code &error) {
  if (!error) {
    new_connection->start();
    start_accept();
  }
}

//int main() {
//  try {
//    boost::asio::io_service io_service;
//    tcp_server server(io_service);
//    io_service.run();
//  }
//  catch (std::exception& e) {
//    std::cerr << e.what() << std::endl;
//  }
//
//  return 0;
//}