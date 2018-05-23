#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "Header.hpp"

namespace HTTP {
namespace Server {

struct Reply {
	enum StatusType {
		ok = 200,
		created = 201,
		accepted = 202,
		noContent = 204,
		multipleChoices = 300,
		movedPermanently = 301,
		movedTemporarily = 302,
		notModified = 304,
		badRequest = 400,
		unauthorized = 401,
		forbidden = 403,
		notFound = 404,
		internalServerError = 500,
		notImplemented = 501,
		badGateway = 502,
		serviceUnavailable = 503
	} status;
	std::vector<Header> headers;
	std::string content;
	/////////////////////////////
	// Convert the reply into a vector of buffers. The buffers
	// do not own the underlying blocks, therefore the reply object
	// must remain valid and not be changed until the write operation
	// has completed.
	std::vector<boost::asio::const_buffer> toBuffers();

	static Reply stockReply( StatusType status );	// stock = "запасной"
};

}
}

#endif


