#ifndef MINI_PICTURES_HPP
#define MINI_PICTURES_HPP

#include "FilesystemFunctions.hpp"

std::string GetMiniPicturePathStr( const std::string &picturePathStr ) {
	return std::string( "MiniPictures/" ) + GetEffectiveFileName( picturePathStr ) + ".png";
}

void CreateMiniPicture( const std::string &picturePathStr ) {
	std::string miniPicturePathStr = GetMiniPicturePathStr( picturePathStr );
	std::cout << GetEffectiveFileName( picturePathStr ) << " needs a mini version" << std::endl; // DEBUG LINE
	std::cout << "Full path: " << picturePathStr << std::endl; // DEBUG LINE
	std::string extension = GetExtension( picturePathStr );

	std::string command; 
	command = std::string( "convert -sample 200x150 -blur 1x0.5 " ) + picturePathStr + " " + miniPicturePathStr;
	system( command.c_str() );

}

void CreateMiniPictures() {
	boost::filesystem::path picturesPath( "Pictures/" );
	boost::filesystem::path miniPicturesPath( "MiniPictures/" );
	for( boost::filesystem::directory_iterator pi( picturesPath ); pi != boost::filesystem::directory_iterator(); ++pi ) {
		std::string pEFN = GetEffectiveFileName( pi->path().string() );
		bool miniVersionFound = false;
		for( boost::filesystem::directory_iterator mpi( miniPicturesPath ); mpi != boost::filesystem::directory_iterator(); ++mpi ) {
			std::string mpEFN = GetEffectiveFileName( mpi->path().string() );
			if( pEFN == mpEFN ) {
				miniVersionFound = true;
				break;
			}
		}
		if( !miniVersionFound ) {
			CreateMiniPicture( pi->path().string() );
		}
	}
}

#endif
