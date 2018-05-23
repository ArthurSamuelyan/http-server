#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include <string>

namespace HTTP {
namespace Server {

struct Header {
	std::string name;
	std::string value;
};

}
}

#endif
