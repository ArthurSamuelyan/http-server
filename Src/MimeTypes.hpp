#ifndef HTTP_MIME_TYPES_HPP
#define HTTP_MIME_TYPES_HPP

#include <string>

namespace HTTP {
namespace Server {
namespace MimeTypes {

	std::string extensionToType( const std::string& extension );

}
}
}

#endif
