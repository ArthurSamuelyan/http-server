#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <tuple>

namespace HTTP {
namespace Server {

struct Request;

/// Parser for incoming requests.
class RequestParser {
public:
	/// Construct ready to parse the request method. Wat?
	RequestParser();

	/// Reset to initial parser state.
	void reset();

	/// Result of parse.
	enum ResultType { good, bad, indeterminate };

	/// Parse some data. The enum return value is good when a complete request 
	/// has beed parsed, bad if the data is invalid, indeterminate when more
	/// data is required. The InputIterator return value indicates how much of 
	/// the input has been consumed.
	template<typename InputIterator>
	std::tuple<ResultType, InputIterator> parse( 
		Request& request,
		InputIterator begin,
		InputIterator end
	) {
		while( begin != end ) {
			ResultType result = consume( request, *begin++ );
			if( result == good || result == bad )
				return std::make_tuple( result, begin );
		}
		return std::make_tuple( indeterminate, begin );
	}

private:
	/// Handle the next character of input.
	ResultType consume( Request& request, char input );

	/// Check if a byte is an HTTP character.
	static bool isChar( int c );

	/// Check if a byte is an HTTP control character.
	static bool isCtl( int c );

	/// Check if a byte is defined as an HTTP tspecial character.
	static bool isTspecial( int c );

	/// Check if a byte is a digit.
	static bool isDigit( int c );

	/// The current state of the parser.
	enum State {
		methodStart,
		method,
		uri,
		httpVersionH,
		httpVersionT1,
		httpVersionT2,
		httpVersionP,
		httpVersionSlash,
		httpVersionMajorStart,
		httpVersionMajor,
		httpVersionMinorStart,
		httpVersionMinor,
		expectingNewline1,
		headerLineStart,
		headerLws,
		headerName,
		spaceBeforeHeaderValue,
		headerValue,
		expectingNewline2,
		expectingNewline3
	} _state;
};

} // namespace Server
} // namespace HTTP

#endif // HTTP_REQUEST_PARSER_HPP
