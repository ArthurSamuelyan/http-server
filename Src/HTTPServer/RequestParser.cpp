#include "RequestParser.hpp"

#include "Request.hpp"

namespace HTTP {
namespace Server {

RequestParser::RequestParser() : _state( methodStart ) {}

void RequestParser::reset() {
	_state = methodStart;
}

RequestParser::ResultType RequestParser::consume( 
	Request& request, 
	char input 
) {
	switch( _state ) {
		case methodStart:
			if( 
				!isChar( input ) || 
				isCtl( input ) || 
				isTspecial( input ) 
			) {
				return bad;
			} else {
				_state = method;
				request.method.push_back( input );
				return indeterminate;
			}
		case method:
			if( input == ' ' ) {
				_state = uri;
				return indeterminate;
			} else if(
				!isChar( input ) || 
				isCtl( input ) || 
				isTspecial( input ) 
			) {
				return bad;
			} else {
				request.method.push_back( input );
				return indeterminate;
			}
		case uri:
			if( input == ' ' ) {
				_state = httpVersionH;
				return indeterminate;
			} else if( isCtl( input ) ) {
				return bad;
			} else {
				// uri may contain Tspecial characters, may be empty
				request.uri.push_back( input );
				return indeterminate;
			}
		case httpVersionH:
			if( input == 'H' ) {
				_state = httpVersionT1;
				return indeterminate;
			} else {
				return bad;
			}

		case httpVersionT1:
			if( input == 'T' ) {
				_state = httpVersionT2;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionT2:
			if( input == 'T' ) {
				_state = httpVersionP;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionP:
			if( input == 'P' ) {
				_state = httpVersionSlash;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionSlash:
			if( input == '/' ) {
				request.httpVersionMajor = 0;
				request.httpVersionMinor = 0;
				_state = httpVersionMajorStart;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionMajorStart:
			if( isDigit( input ) ) {
				request.httpVersionMajor = 
					request.httpVersionMajor + ( input - '0' );
				_state = httpVersionMajor;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionMajor:
			if( input == '.' ) {
				_state = httpVersionMinorStart;
				return indeterminate;
			} else if( isDigit( input ) ) {
				request.httpVersionMajor = 
					request.httpVersionMajor + ( input - '0' );
				_state = httpVersionMajor;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionMinorStart:
			if( isDigit( input ) ) {
				request.httpVersionMinor =
					request.httpVersionMinor + ( input - '0' );
				_state = httpVersionMinor;
				return indeterminate;
			} else {
				return bad;
			}
		case httpVersionMinor:
			if( input == '\r' ) {
				_state = expectingNewline1;
				return indeterminate;
			} else if( isDigit( input ) ) {
				request.httpVersionMinor =
					request.httpVersionMinor + ( input - '0' );
				_state = httpVersionMinor;
				return indeterminate;
			} else {
				return bad;
			}
		case expectingNewline1:
			if( input == '\n' ) {
				_state = headerLineStart;
				return indeterminate;
			} else {
				return bad;
			}
		case headerLineStart:
			if( input == '\r' ) {
				_state = expectingNewline3; // ???
				return indeterminate;
			} else if( 
				!request.headers.empty() &&
				( input == ' ' || input == '\t' ) // what does '\t'
			) {
				_state = headerLws;
				return indeterminate;
			} else if( 
				!isChar( input ) ||
				isCtl( input ) ||
				isTspecial( input )
			) {
				return bad;
			} else {
				request.headers.push_back( Header() );
				request.headers.back().name.push_back( input );
				_state = headerName;
				return indeterminate;
			}
		case headerLws:
			if( input == '\r' ) {
				_state = expectingNewline2;
				return indeterminate;
			} else if ( input == ' ' || input == '\t' ) {
				return indeterminate;
			} else if ( isCtl( input ) ) {
				return bad;
			} else {
				_state = headerValue;
				request.headers.back().value.push_back( input );
				return indeterminate;
			}
		case headerName:
			if( input == ':' ) {
				_state = spaceBeforeHeaderValue;
				return indeterminate;
			} else if (
				!isChar( input ) ||
				isCtl( input ) ||
				isTspecial( input )
			) {
				return bad;
			} else {
				request.headers.back().name.push_back( input );
				return indeterminate;
			}
		case spaceBeforeHeaderValue:
			if( input == ' ' ) {
				_state = headerValue;
				return indeterminate;
			} else {
				return bad;
			}
		case headerValue:
			if( input == '\r' ) {
				_state = expectingNewline2;
				return indeterminate;
			} else if( isCtl( input ) ) {
				return bad;
			} else {
				request.headers.back().value.push_back( input );
				return indeterminate;
			}
		case expectingNewline2:
			if( input == '\n' ) {
				_state = headerLineStart;
				return indeterminate;
			} else {
				return bad;
			}
		case expectingNewline3:
			return ( input == '\n' ) ? good : bad;
		default:
			return bad;
	}
}

bool RequestParser::isChar( int c ) {
	return ( c >= 0 && c <= 127 );
}

bool RequestParser::isCtl( int c ) {
	return ( ( c >= 0 && c <= 31 ) || ( c == 127 ) );
}

bool RequestParser::isTspecial( int c ) {
	switch( c ) {
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default:
			return false;
	}
}

bool RequestParser::isDigit( int c ) {
	return ( c >= '0' && c <= '9' );
}

} // namespace Server
} // namespace HTTP


