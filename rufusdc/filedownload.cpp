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
#include "downloadrequest.h"
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
	
	// TODO serch for other sources here
	
	// get and connect to hub
	_pHub = _pParent->getHub( _hub );
	
	_pHub->signalStateChanged.connect( boost::bind( &FileDownload::hubStateChanged, this, _1 ) );
	
	if ( _pHub->state() == Hub::Connected )
	{
		requestTransfer();
	}
}

// ============================================================================
// Requesttransfer
void FileDownload::requestTransfer()
{
	const uint64_t chunk_size = 1 * 1024 * 1024; // 1 MB. absolutely experimantal value
	
	// request the transfer immediately!
	uint64_t bytesRequested = min( _size - _receivedBytes, chunk_size );
	
	
	shared_ptr<DownloadRequest> pRequest( new DownloadRequest );
	pRequest->setNick( _nick );
	pRequest->setFile( _path ); // TODO inconsistent naming!
	
	pRequest->setOffset( _receivedBytes );
	pRequest->setCount( bytesRequested );
	
	// set up timeout (10 s )
	pRequest->setExpiryTime( posix_time::second_clock::local_time() + posix_time::seconds(10) );
	
	// connect
	pRequest->signalDataIncoming.connect( boost::bind( &FileDownload::dataIncoming, this, _1, _2 ) );
	
	_pParent->requestTransfer( _pHub.get(), pRequest );
}

// ============================================================================
// Hub's state changed
void FileDownload::hubStateChanged( int state )
{
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

// ============================================================================
// data incoming
void FileDownload::dataIncoming( vector<char>& data, uint64_t offset )
{
	// TODO write to file
}

}
