#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>

#include "Reply.hpp"
#include "Request.hpp"
#include "RequestHandler.hpp"
#include "RequestParser.hpp"

namespace HTTP {
namespace Server {

class ConnectionManager;

/// Represents a single connection from a client.
class Connection : public std::enable_shared_from_this<Connection> {
public:
	Connection( const Connection& ) = delete;				// Uncopyable
	Connection& operator=( const Connection& ) = delete;	//

	/// Construct a connection with the given socket.
	explicit Connection( 
		boost::asio::ip::tcp::socket socket,
		ConnectionManager& manager,
		RequestHandler& handler
	);

	/// Starts the first asynchronous operation for the connection.
	void start();

	/// Stop all asynchronous operations associated with the connection.
	void stop();

private:

	/// Perform an asynchronous read operation.
	void _read();

	/// Perform an asynchronous write operation.
	void _write();


	/// Socket for the connection.
	boost::asio::ip::tcp::socket _socket;

	/// The manager for the connection.
	ConnectionManager& _connectionManager;

	/// The handler used to process the incoming request.
	RequestHandler& _requestHandler;

	/// Buffer for incoming data.
	std::array<char, 8192> _buffer;

	/// The incoming request.
	Request _request;

	/// The parser for the incoming request.
	RequestParser _requestParser;

	/// The reply to be sent back to the client.
	Reply _reply;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

} // namespace Server
} // namespace HTTP

#endif // HTTP_CONNECTION_HPP
