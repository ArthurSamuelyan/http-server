#include "RequestHandler.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include "MimeTypes.hpp"
#include "Reply.hpp"
#include "Request.hpp"

#include <iostream> // for debug

namespace HTTP {
namespace Server {

RequestHandler::RequestHandler( const std::string& docRoot ) : 
	_docRoot( docRoot ) {}

void RequestHandler::handleRequest( const Request& request, Reply& reply ) {
	// Decode url to path.
	std::string requestPath;
	if( !urlDecode( request.uri, requestPath ) ) {
		reply = Reply::stockReply( Reply::badRequest );
		return;
	}
	std::cout << "URI:\"" << request.uri << "\" Path:\"" << requestPath << "\"\n";

	// Request path must be absolute and not contain "..".
	if( 
		requestPath.empty() || 
		requestPath[ 0 ] != '/' || 
		requestPath.find( ".." ) != std::string::npos 
	) {
		Reply::stockReply( Reply::badRequest );
		return;
	}

	// If path ends in slash ( is a directory ) then add "index.html".
	if( requestPath[ requestPath.size() - 1 ] == '/' ) {
		requestPath += "index.html";
	}

	// Determine the file extension.
	std::size_t lastSlashPos = requestPath.find_last_of( "/" );
	std::size_t lastDotPos = requestPath.find_last_of( "." );
	std::string extension;
	if(
		lastDotPos != std::string::npos &&
		lastDotPos > lastSlashPos
	) {
		extension = requestPath.substr( lastDotPos + 1 ); 
		// Need to see std::string::substr
	}

	// Open the file to send back.
	std::string fullPath = _docRoot + requestPath;
	std::ifstream is( fullPath.c_str(), std::ios::in | std::ios::binary );
	if( !is ) {
		reply = Reply::stockReply( Reply::notFound );
		return;
	}

	// Fill out the reply to be set to the client.
	reply.status = Reply::ok;
	char buffer[ 512 ];
	while( is.read( buffer, sizeof( buffer ) ).gcount() > 0 )
		reply.content.append( buffer, is.gcount() );
	// need to see std::ifstream::gcount()
	reply.headers.resize( 2 );
	reply.headers[ 0 ].name = "Content-Length";
	reply.headers[ 0 ].value = std::to_string( reply.content.size() );
	reply.headers[ 1 ].name = "Content-Type";
	reply.headers[ 1 ].value = MimeTypes::extensionToType( extension );
}

bool RequestHandler::urlDecode( const std::string& in, std::string& out ) {
	out.clear();
	out.reserve( in.size() ); // ???
	for( std::size_t i = 0; i < in.size(); ++i ) {
		if( in[ i ] == '%' ) {
			if( i + 3 < in.size() ) {
				int value = 0;
				std::istringstream is( in.substr( i + 1, 2 ) );
				if( is >> std::hex >> value ) {
					out += static_cast<char>( value );
					// what does static_cast template ?
					i += 2;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if( in[ i ] == '+' ) {
			out += ' ';
		} else {
			out += in[ i ];
		}
	}
	return true;
}

} // namespace Server
} // namespace HTTP


