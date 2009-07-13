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
#include <iostream>

// bind
#include <boost/bind.hpp>

// local
#include "client.h"
#include "filelist.h"
#include "downloadmanager.h"

#include "filelistdownload.h"

namespace RufusDc
{

// ============================================================================
// Constructor
FileListDownload::FileListDownload(): Download()
{
	_inBuffer.reserve( 64 * 1024 ); // 64kB. Total address space capacity of Z80. O tempora, o mores!
}

// ============================================================================
// Destructor
FileListDownload::~FileListDownload()
{
}

// ============================================================================
// Starts download
void FileListDownload::start()
{
	// do it in one shot - just download the list
	try
	{
		HubManager::ref().requestConnection
			( _hub
			, _nick
			, boost::bind( &FileListDownload::onConnected, this, _1, _2 )
			);
	}
	catch( const std::exception& e )
	{
		// TODO change state, emit some system message?
		cerr << "Error downloading file list from " << _nick <<"@"<<_hub<<":"<< e.what() << endl;
	}
}

// ============================================================================
// On connected
void FileListDownload::onConnected( const Error& err, const shared_ptr<DirectConnection>& pConnection )
{
	if ( ! err )
	{
		try
		{
			// download list
			if ( pConnection->hasFeature("XmlBZList") )
			{
				pConnection->downloadFile
					( "files.xml.bz2"
					, 0, 0
					, boost::bind( &FileListDownload::onIncomingData, this, _1, _2 )
					, boost::bind( &FileListDownload::downloadCompleted, this, _1 )
					);
				
				_pConnection = pConnection;
			}
			else
			{
				// fail!
				cerr << "Error downloading file list: don't know how to download list other than files.xml.bz2" << endl;
				setState( Failed );
			}
		}
		catch( const std::exception& e )
		{
			cerr << "Error downloading file list: " << e.what() << endl;
			setState( Failed );
		}
	}
	else
	{
		cerr << "Error connecting to file list source : " << err.msg() << endl;
		setState( Failed );
	}
}

// ============================================================================
// on incoming data
void FileListDownload::onIncomingData( vector<char>& data, uint64_t offset )
{
	// TODO suboptimal. Too much copying 
	
	int currentSize = _inBuffer.size();
	_inBuffer.resize( currentSize + data.size() );
	
	for( int i = 0; i < data.size(); i++ )
	{
		_inBuffer[ currentSize + i ] = data[i];
	}
	
}

// ============================================================================
// On download completed
void FileListDownload::downloadCompleted( const Error& err )
{
	if ( ! err )
	{
		// TODO stats
		cerr << "File list transfer completed, speed: "<< ( _pConnection->totalTransferSpeed() / 1024.0 ) << "kB/s" << endl;
		try
		{
			// ok, data is here. now decompress using bzlib
			shared_ptr<FileList> pFileList( new FileList() );
			
			pFileList->fromBz2Data( _inBuffer );
			pFileList->setNick( _nick );
			pFileList->setHub( _hub );
			
			DownloadManager::ref().fileListReceived( pFileList ); // TODO replace with completion handler
			setState( Completed );
			
		}
		catch( const std::exception& e )
		{
			cerr << "Error deconding file list: " << e.what() << endl;
		}
	}
	else
	{
		cerr << "Error downloading file list: "<< err.msg() << endl;
		setState( Failed );
	}
}

}
