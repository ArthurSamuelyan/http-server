#include "ConnectionManager.hpp"

namespace HTTP {
namespace Server {

ConnectionManager::ConnectionManager() {}

void ConnectionManager::start( ConnectionPtr c ) {
	_connections.insert( c );
	c->start();
}

void ConnectionManager::stop( ConnectionPtr c ) {
	_connections.erase( c );
	c->stop();
}

void ConnectionManager::stopAll() {
	for( auto c: _connections )
		c->stop();
	_connections.clear();
}
	

} // namespace Server
} // namespace HTTP
