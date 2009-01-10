// Copyright (C) 2009 Maciek Gajewski <maciej.gajewski0@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// std
#include <stdexcept>

// boost
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

// local
#include "client.h"
#include "connectionrequest.h"
#include "hub.h"

#include "filedownload.h"

namespace RufusDc
{

// ============================================================================
// Constructor
FileDownload::FileDownload( Client* pParent ): Download( pParent )
{
	_receivedBytes = 0;
}

// ============================================================================
// Destructor
FileDownload::~FileDownload()
{
}

// ============================================================================
// Start
void FileDownload::start()
{
	if ( _path.empty() || _hub.empty() || _nick.empty() )
	{
		throw std::logic_error("File download ot configured");
	}
	createTemporaryFile();
	
	// TODO
}

// ============================================================================
// Initializes temporary file
void FileDownload::createTemporaryFile()
{
	const char* suffix = ".rdctmp";

	filesystem::path filePath( _path );
	string tmpBaseName = filePath.leaf() + suffix;
	
	filesystem::path tmpDirPath( _pParent->settings().downloadDirectory );
	
	_tmpFileName = filesystem::path( tmpDirPath / tmpBaseName ).file_string();
	
	// open the file
	_tmpFile.open( _tmpFileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
	
}

}
