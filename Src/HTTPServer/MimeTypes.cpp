#include "MimeTypes.hpp"

//#include <map> // std::map

namespace HTTP {
namespace Server {
namespace MimeTypes {

struct StringMapping {
	const char* extension;
	const char* mimeType;
};

StringMapping mappings[] = {
	{ "gif"	, "image/gif"	},
	{ "htm"	, "text/html"	},
	{ "html", "text/html"	},
	{ "css" , "text/css"	},
	{ "jpg"	, "image/jpeg"	},
	{ "png"	, "image/png"	}
};

#define DefaultType "text/plain"

std::string extensionToType( const std::string& extension ) {
	for( StringMapping m: mappings ) {
		if( m.extension == extension ) {
			return m.mimeType;
		}
	}
	return DefaultType;
}

} // namespace MimeTypes
} // namespace Server
} // namespace HTTP 
