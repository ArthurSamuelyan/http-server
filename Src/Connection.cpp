#include "Connection.hpp"

#include <utility> // what for?
#include <vector>
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"

namespace HTTP {
namespace Server {

Connection::Connection(
	boost::asio::ip::tcp::socket socket,
	ConnectionManager& manager,
	RequestHandler& handler
) : 
	_socket( std::move( socket ) ),
	_connectionManager( manager ),
	_requestHandler( handler )
{}

void Connection::start() {
	_read();
}

void Connection::stop() {
	_socket.close();
}

void Connection::_read() {
	auto self( shared_from_this() );
	_socket.async_read_some(
		boost::asio::buffer( _buffer ),
		[ this, self ]( 
			boost::system::error_code ec, 
			std::size_t bytesTransferred 
		) {
			if( !ec ) {
				RequestParser::ResultType result;
				std::tie( result, std::ignore ) =  // What is std::tie ?
					_requestParser.parse( 
						_request, 
						_buffer.data(), 
						_buffer.data() + bytesTransferred 
					);
				if( result == RequestParser::good ) {
					_requestHandler.handleRequest( _request, _reply );
					_write();
				} else if( result == RequestParser::bad ) {
					_reply = Reply::stockReply( Reply::badRequest );
					_write();
				} else {
					_read();
				}
			} else if( ec != boost::asio::error::operation_aborted ) {
				_connectionManager.stop( shared_from_this() );
			}
		}
	);
}

void Connection::_write() {
	auto self( shared_from_this() );
	boost::asio::async_write(
		_socket,
		_reply.toBuffers(),
		[ this, self ]( 
			boost::system::error_code ec, 
			std::size_t
		) {
			if( !ec ) {
				// Initiate graceful connection closure.
				boost::system::error_code ignoredEc;
				_socket.shutdown(
					boost::asio::ip::tcp::socket::shutdown_both,
					ignoredEc
				);
			}
			if( ec != boost::asio::error::operation_aborted ) {
				_connectionManager.stop( shared_from_this() );
			}
		}
	);
}

} // namespace Server
} // namespace HTTP
