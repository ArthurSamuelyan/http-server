#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include "Header.hpp"

namespace HTTP {
namespace Server {

/// A request received from client.
struct Request {
	std::string method;
	std::string uri;
	int httpVersionMajor;
	int httpVersionMinor;
	std::vector<Header> headers;
};

} // namespace Server
} // namespace HTTP

#endif
