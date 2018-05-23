#ifndef FILESYSTEM_FUNCTIONS_HPP
#define FILESYSTEM_FUNCTIONS_HPP

#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

#include <boost/filesystem.hpp>

const char* KnownExtensions[] = {
	"png",
	"jpg",
	"gif" // ??? Do we need it?
};

enum ExtensionType{
	png = 0,
	jpg = 1,
	gif = 2
};

#define EXTENSION_NUMBER 3

unsigned int GetET( const std::string &extension ) {
	const char* extensionCStr = extension.c_str();
	unsigned int et;
	for( et = 0; et < EXTENSION_NUMBER; ++et )
		if( strcmp( extensionCStr, KnownExtensions[ et ] ) == 0 ) break;
	return et;
}

std::string GetStandardDirPath( const char* dirPath ) {
	std::string dirPathStr( dirPath );
	if( dirPathStr[ dirPathStr.length() - 1 ] != '/' )
		dirPathStr += "/";
	// If there will be troubles with absolute paths, alter this function.
	return dirPathStr;
}

std::string GetEffectiveFileName( const std::string &filePathStr ) {
	std::size_t lastSlashPos = filePathStr.find_last_of( "/" );
	std::size_t lastDotPos = filePathStr.find_last_of( "." );
	std::size_t EFNStart = lastSlashPos + 1; // If no '/' found, EFNStart will be 0
	std::size_t EFNLengt = 
		( lastSlashPos + 1 < lastDotPos && lastDotPos < std::string::npos ) ? 
		// EFNLength is always non-zero. For example, file 'Data/.file' has no extension, but a '.file' EFN
		lastDotPos - ( lastSlashPos + 1 ) : 
		std::string::npos;
	return filePathStr.substr( EFNStart, EFNLengt );
}

std::string GetExtension( const std::string &filePathStr ) {
	std::size_t lastSlashPos = filePathStr.find_last_of( "/" );
	std::size_t lastDotPos = filePathStr.find_last_of( "." );
	std::size_t extensionStart = 
		( lastSlashPos + 1 < lastDotPos && lastDotPos < std::string::npos ) ? 
		lastDotPos + 1 :
		std::string::npos;
	return filePathStr.substr( extensionStart );
}

#endif
