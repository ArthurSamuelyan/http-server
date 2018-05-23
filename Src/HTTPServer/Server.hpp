#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>
#include <boost/asio.hpp>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"

namespace HTTP {
namespace Server {

/// The top-level class of the HTTP server
class Server {
public:
	Server( const Server& ) = delete;				// Noncopyable
	Server& operator=( const Server& ) = delete;	//

	/// Construct the server to listen on the specified TCP address and
	/// port, and serve up files from the given directory
	explicit Server( 
		const std::string& address, 
		const std::string& port, 
		const std::string& docRoot 
	);

	/// Run the server's io_context loop.
	void run();

private:

	/// Perform an asynchronous accept operation.
	void _accept();

	/// Wait for a request to stop the server.
	void _awaitStop();


	/// The io_context used to perform asynchronous operations.
	boost::asio::io_context _ioContext;

	/// The signal set is used to register for process termination notifications.
	boost::asio::signal_set _signals;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor _acceptor;

	/// The connection manager which owns all live connections.
	ConnectionManager _connectionManager;

	/// The handler for all incoming requests.
	RequestHandler _requestHandler;
};
	

} // namespace HTTP
} // namespace Server


#endif // HTTP_SERVER_HPP
