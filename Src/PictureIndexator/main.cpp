#include <string>
#include <iostream>

#include "FilesystemFunctions.hpp"
#include "MiniPictures.hpp"
#include "HTMLGen.hpp"

// How to walk through a directory?


// The program is supposed to create mini versions of pictures, 
// found in Data/Pictures/ and put 'em into Data/MiniPictures
//
// For example from picture 
//
// Data/Pictures/s_ample_picture.jpeg
//
// it will create
//
// Data/MiniPictures/s_ample_picture.png
//
// if file with name Data/MiniPictures/s_ample_picture.png 
// already exists, nothing will be done
//
// 
// It takes Data directory as the only argument
int main( int argc, char* argv[] ) {
	if( argc == 2 ) {
		std::string workDir = GetStandardDirPath( argv[ 1 ] );
		if( chdir( workDir.c_str() ) != 0 ) {
			std::cerr << "Wrong work directory!";
			return 1;
		}
		CreateMiniPictures();
		RefreshIndexes();
	} else {
		std::cerr << "Wrong usage: required only one argument - data directory relative path!" << std::endl; // relative?
	}
	return 0;
}
