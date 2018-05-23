#ifndef HTML_GEN_HPP
#define HTML_GEN_HPP

#include <sstream>
#include <vector>
#include <arpa/inet.h>

#include "FilesystemFunctions.hpp"
#include "MiniPictures.hpp"

// Here we assume, MiniPictures are already created.

#define MINI_PICTURE_BACKGROUND "Support/Images/GenericPictureBackground.png"


#define MAX_MINI_WIDTH 200
#define MAX_MINI_HEIGHT 150

class HtmlPageGen{
	public:
		const unsigned int ColNum = 5;
		const unsigned int RowNum = 7;


		HtmlPageGen() {
			_maxIter = 0;
		}
		void setPicturePathStrs( std::vector<std::string>* picturePathStrs ) {
			_picturePathStrs = picturePathStrs;
			_maxIter = _picturePathStrs->size();
		}
		void setIter( unsigned int startIter ) {
			_startIter = startIter;
			_progressIter = _startIter;
		}
		unsigned int getPgNum() {
			return _startIter / ( ColNum * RowNum ) + 1;
		}
		unsigned int getMaxPgNum() {
			return _maxIter / ( ColNum * RowNum ) + 1; // ?? Do we need +1 here?
		}
		std::string getPage() {
			std::string page( "" );
			if( _picturePathStrs == NULL )
				return page;
			page += _head();
			for( unsigned int i = 0; ( i < RowNum && _progressIter < _maxIter ); i++ )
				page += _row();
			page += _tail();
			return page;
		}

	private:
		std::vector<std::string>* _picturePathStrs; // ???
		unsigned int _startIter;
		unsigned int _progressIter;
		unsigned int _maxIter;


		std::string _head() {
			std::stringstream headBuffer;
			headBuffer << "\
<!doctype html>\n\
<html>\n\
  <head>\n\
    <meta content=\"text/html\" charset=\"utf-8\" />\n\
    <title> ShareImage - a simple picture storage </title>\n\
	<link rel=\"stylesheet\" type=\"text/css\" href=\"Support/CSS/main.css\" />\n\
  </head>\n\
  <body\n\
  class=\"Body\">\n\
    <h1 class=\"Header\"> ShareImage </h1>\n\
	<p class=\"SmallText\">A simple site with images by Makarov Ivan and Samuelyan Arthur.</p>\n\
	<div\n\
	class=\"HatSpacer\">\n\
	</div>\n\
    <div\n\
    align=\"center\">\n\
      <table\n\
	  class=\"Table\"\n\
      frame=\"void\"\n\
      cols=\"" << ColNum << "\"\n\
      cellpadding=\"0\"\n\
      cellspacing=\"0\">\n";	  
			return headBuffer.str();
		}

		std::string _tail() {
			std::stringstream tailBuffer;
			tailBuffer << "\
      </table>\n\
    </div>\n\
  </body>\n\
</html>\n";
			return tailBuffer.str();
		}

		std::string _cell( const std::string &picturePathStr ) {
			// Getting picture size:
			std::string miniPicturePathStr = GetMiniPicturePathStr( picturePathStr );
			std::ifstream pic( miniPicturePathStr.c_str() );
			unsigned int width, height, leftMargin, topMargin;
			pic.seekg( 16 );
			pic.read( (char*)&width, 4 );
			pic.read( (char*)&height, 4 );
			width = ntohl( width );
			height = ntohl( height );

			std::cout << width << "x" << height << std::endl; // DEBUG LINE

			leftMargin = ( MAX_MINI_WIDTH - width ) / 2;
			topMargin = ( MAX_MINI_HEIGHT - height ) / 2;

			std::stringstream cellBuffer;
			cellBuffer << "\
          <td\n\
		  class=\"TableCell\">\n\
		    <a\n\
			class=\"FileLink\"\n\
			href=\"" << picturePathStr << "\"\n\
			target=\"_blank\">\n\
			  <div\n\
			  class=\"FileContainer\">\n\
			    <div\n\
				class=\"PictureContainer\">\n\
			      <img\n\
				  style=\"margin-top:" << topMargin << "px;margin-left:" << leftMargin << "px\"\n\
				  src=\"" << miniPicturePathStr << "\"\n\
				  alt=\"Nice pic, too bad, you cannot see it\" />\n\
			    </div>\n\
			    <p\n\
			    class=\"TextContainer\">\n\
			      " << picturePathStr << "\n\
			    </p>\n\
			  </div>\n\
		    </a>\n\
		  </td>\n";
			return cellBuffer.str();
		}

		std::string _emptyCell() {
			std::stringstream emptyCellBuffer;
			emptyCellBuffer << "\
          <td\n\
		  class=\"TableCell\">\n\
		    <div\n\
			class=\"CellSpacer\">\n\
			</div>\n\
		  </td>\n";
			return emptyCellBuffer.str();
		}

		std::string _row() {
			std::stringstream rowBuffer;
			rowBuffer << "\
        <tr>\n";
			unsigned int rowEndH = _progressIter + ColNum; // hypothetical
			unsigned int rowEndA = ( rowEndH <= _maxIter ) ? rowEndH : _maxIter; // actual

			for( unsigned int i = _progressIter; i < rowEndA; i++ )
				rowBuffer << _cell( (*_picturePathStrs)[ i ] );
			for( unsigned int j = rowEndA; j < rowEndH; j++ )
				rowBuffer << _emptyCell();

			_progressIter = rowEndA;
			rowBuffer << "\
        </tr>\n";
			return rowBuffer.str();
		}
};


void RefreshIndexes() {
	// Delete all .html files in root directory
	//
	// Walk through all the pictures, put their names into an array
	// & sort 'em
	std::vector<std::string> picturePathStrs;
	boost::filesystem::path picturesPath( "Pictures/" );
	for( boost::filesystem::directory_iterator pi( picturesPath ); pi != boost::filesystem::directory_iterator(); ++pi ) {
		std::string currPicturePathStr = pi->path().string();
		auto iter = picturePathStrs.begin();
		for( iter = picturePathStrs.begin(); iter != picturePathStrs.end() && *iter < currPicturePathStr; ++iter ) {}
		picturePathStrs.insert( iter, currPicturePathStr );
	}

	HtmlPageGen pgGen;
	pgGen.setPicturePathStrs( &picturePathStrs );
	pgGen.setIter( 0 );


	std::ofstream fout( "index.html" );
	fout << pgGen.getPage();
	// Later add page navigation and hrefs to neighbour pages
}

#endif
