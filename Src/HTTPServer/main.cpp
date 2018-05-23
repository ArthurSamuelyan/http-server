#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "Server.hpp"

int main( int argc, char* argv[] ) {
	try {
		// Check command line arguments.
		if( argc != 4 ) {
			std::cerr << "Usage: sudo ./Bin/HTTPServer <address> <port> <docRoot>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    sudo ./Bin/HTTPServer 0.0.0.0 80 Data\n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    sudo ./Bin/HTTPServer 0::0 80 Data\n";
			return 1;
		}

		// Initialise the server.
		HTTP::Server::Server s( argv[ 1 ], argv[ 2 ], argv[ 3 ] );
		// Run the server until stopped.
		s.run();
	} catch( std::exception& e ) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}
