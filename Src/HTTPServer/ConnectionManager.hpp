#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include "Connection.hpp"

namespace HTTP {
namespace Server {

/// Manages open connections so that they may be cleanly stopped when
/// the server needs to shut down.
class ConnectionManager {
public:
	ConnectionManager( const ConnectionManager& ) = delete;				// Uncopyable
	ConnectionManager& operator=( const ConnectionManager& ) = delete;	//

	/// Construct a connection manager.
	ConnectionManager();

	/// Add the specified connection to the manager and start it.
	void start( ConnectionPtr c );

	/// Stop the specified connection.
	void stop( ConnectionPtr c );

	/// Stop all connections.
	void stopAll();

private:

	/// The managed connections.
	std::set<ConnectionPtr> _connections;
};


} // namespace Server
} // namespace HTTP

#endif // HTTP_CONNECTION_MANAGER_HPP
