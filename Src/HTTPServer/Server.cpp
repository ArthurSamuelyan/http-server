#include "Server.hpp"

#include <signal.h>
#include <utility>

namespace HTTP {
namespace Server {

Server::Server(
	const std::string& address,
	const std::string& port,
	const std::string& docRoot
) :
	_ioContext( 1 ),
	_signals( _ioContext ),
	_acceptor( _ioContext ),
	_connectionManager(),
	_requestHandler( docRoot )
{
	// Register to handle the signals that indicate when the server should exit.
	// It is safe to register for the same signal multiple times in a program,
	// provided all registration for the specified signal is made through Asio.
	_signals.add( SIGINT );
	_signals.add( SIGTERM );
#if defined( SIGQUIT )
	_signals.add( SIGQUIT );
#endif // defined( SIGQUIT )

	_awaitStop();
	
	//Open the acceptor with the option to reuse the address (SO_REUSEADDR)
	boost::asio::ip::tcp::resolver resolver( _ioContext );
	boost::asio::ip::tcp::endpoint endpoint = 
		*resolver.resolve( address, port ).begin();
	_acceptor.open( endpoint.protocol() );
	_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
	_acceptor.bind( endpoint );
	_acceptor.listen();

	_accept();
}

void Server::run() {
	// The io_context::run() call will block until all asynchronous operations
	// have finished. While the server is running there is always at least one
	// asynchronous operation outstanding: the asynchronous accept call waiting 
	// for new incoming connections.
	_ioContext.run();
}

void Server::_accept() {
	_acceptor.async_accept(
		[ this ] (
			boost::system::error_code ec,
			boost::asio::ip::tcp::socket socket
		) {
			// Check whether the server was stopped by a signal before this
			// completion handler had a chance to run.
			if( !_acceptor.is_open() ) {
				return;
			}
			if( !ec ) {
				_connectionManager.start( 
					std::make_shared<Connection>(
						std::move( socket ),
						_connectionManager,
						_requestHandler
					)
				);
			}
			_accept();
		}
	);
}

void Server::_awaitStop() {
	_signals.async_wait(
		[ this ]( boost::system::error_code /*ec*/, int /*signo*/ ) {
			// The server is stopped by cancelling all outstanding
			// asynchronous operations. Once all operations have finished
			// the io_context::run() call will exit.
			_acceptor.close();
			_connectionManager.stopAll();
		}
	);
}


} // namespace Server
} // namespace HTTP
