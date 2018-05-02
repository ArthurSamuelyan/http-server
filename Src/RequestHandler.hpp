#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>

namespace HTTP {
namespace Server {

struct Reply;
struct Request;

/// The common handler for all incoming requests
class RequestHandler {
public:
	RequestHandler( const RequestHandler& ) = delete;				// Uncopyable
	RequestHandler& operator=( const RequestHandler& ) = delete;	//

	/// Construct with a directory containing files to be served.
	explicit RequestHandler( const std::string& docRoot );

	/// Handle a request and produce a reply.
	void handleRequest( const Request& request, Reply& reply );

private:
	/// The directory containing the files to be served.
	std::string _docRoot;
	/// Perform URL-decoding on a string. Returns false if encoding was invalid.
	static bool urlDecode( const std::string& in, std::string& out );
};

} // namespace Server
} // namespace HTTP

#endif // HTTP_REQUEST_HANDLER_HPP
