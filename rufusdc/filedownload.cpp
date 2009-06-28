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
#include <boost/foreach.hpp>

// local
#include "client.h"
#include "connectionrequest.h"
#include "hub.h"
#include "directconnection.h"

#include "filedownload.h"

namespace RufusDc
{


// constants
static const char* TMPFILE_SUFFIX = ".rdctmp";
static const char* NFOFILE_SUFFIX = ".rdcnfo";

// ============================================================================
// Constructor
FileDownload::FileDownload()
	: Download()
	, _timer( Client::ref().ioService() )
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
	if ( _path.empty() || _hub.empty() || _nick.empty() || _size == 0 )
	{
		throw std::logic_error("File download not configured");
	}
	
	// create temp files
	createTemporaryFile();
	createInfoFile();
	
	// intialize map
	_map.initialize( _size );
	
	uint64_t start, len;
	findWhatToDownload( NULL, start, len );
	
	// initialize list of sources
	shared_ptr<Source> pSrc( new Source );
	
	pSrc->nick = _nick;
	pSrc->hub = _hub;
	pSrc->path = _path;
	
	_sources.push_back( pSrc );
	
	dumpInfo();
	
	connectToSource( pSrc );
}

// ============================================================================
// Initializes temporary file
void FileDownload::createTemporaryFile()
{
	filesystem::path filePath( _path );
	string tmpBaseName = filePath.leaf() + TMPFILE_SUFFIX;
	
	filesystem::path tmpDirPath( Client::ref().settings().downloadDirectory );
	
	_tmpFileName = filesystem::path( tmpDirPath / tmpBaseName ).file_string();
	
	// open the file
	_tmpFile.open( _tmpFileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
	
}

// ============================================================================
// Creates info file
void FileDownload::createInfoFile()
{
	filesystem::path filePath( _path );
	string tmpBaseName = filePath.leaf() + NFOFILE_SUFFIX;
	
	filesystem::path tmpDirPath( Client::ref().settings().downloadDirectory );
	
	_infoFileName = filesystem::path( tmpDirPath / tmpBaseName ).file_string();
	
	// open the file
	fstream infoFile;
	infoFile.open( _infoFileName.c_str(), ios_base::out | ios_base::trunc );
	
	// TODO dump some initial info into the file
}

// ============================================================================
// Dumps info into info file
void FileDownload::dumpInfo()
{
	// open file
	fstream infoFile;
	infoFile.open( _infoFileName.c_str(), ios_base::out | ios_base::trunc );
	
	// dump file name
	filesystem::path filePath( _path );
	infoFile << "file " << escape( filePath.leaf() ) << endl;
	
	// dump file size
	infoFile << "size " << _size << endl;
	
	// dump tth
	infoFile << "tth " << _tth << endl;
	
	// dump sources
	BOOST_FOREACH( const shared_ptr<Source>& pSrc, _sources )
	{
		infoFile << "source " << escape( pSrc->hub ) <<" "<< escape( pSrc->nick ) << " "<< escape(pSrc->path) << endl;
	}
	
	// dump dowloaded map
	infoFile << "map " << _map.toString();
}

// ============================================================================
// Escape
string FileDownload::escape( const string& str )
{
	string result;
	for( int i = 0; i < str.length(); i++ )
	{
		char c = str[i];
		switch( c )
		{
			case ' ':
				result += "\\s";
				break;
				
			case '\n':
				result += "\\n";
				break;
				
			case '\r':
				result += "\\r";
				break;
				
			case '\t':
				result += "\\t";
				break;
			
			case '\\':
				result += "\\\\";
				break;
			
			default:
				result.push_back( c );
		}
	}
	
	return result;
}

// ============================================================================
// Connect to source
void FileDownload::connectToSource( const shared_ptr<Source>& pSrc )
{
	try
	{
		HubManager::ref().requestConnection
			( pSrc->hub
			, pSrc->nick
			, boost::bind( &FileDownload::onConnected, this, pSrc, _1, _2 )
			);
	}
	catch( const std::exception& e )
	{
		// TODO ?
	}
}

// ============================================================================
// Connected
void FileDownload::onConnected( const shared_ptr<Source>& pSrc, const Error& err, const shared_ptr<DirectConnection>& pConn )
{
	if ( ! err )
	{
		// maybe it was completed in the meantine?
		if ( _state == Completed )
		{
			pConn->disconnect();
			return;
		}
		pSrc->successfullConnection = posix_time::second_clock::local_time();
		pSrc->pConnection = pConn;
		
		// hide all "A.I." behind the method
		uint64_t start, len;
		findWhatToDownload( pSrc.get(), start, len );
		
		string path = pSrc->path; // TODO this won't work
		if ( ! _tth.empty() )
		{
			path = string("TTH/") + _tth;
		}
		else
		{
			cerr << "WARNING - downloading w/o TTH. Probably will fail" << endl;
		}
		
		pConn->downloadFile
			( path
			, start
			, len
			, boost::bind( &FileDownload::onDataIncoming, this, _1, _2 )
			, boost::bind( &FileDownload::onDownloadCompleted, this, pSrc, _1 )
			);
		
		_state = Active;
	}
	else
	{
		// TODO sorry Winetou...
	}
}

// ============================================================================
// On data incoming
void FileDownload::onDataIncoming( vector<char>& data, uint64_t offset )
{
	// was completed in the meantime?
	if ( _state == Completed )
	{
		return;
	}
	
	_tmpFile.seekp( offset );
	_tmpFile.write( data.data(), data.size() );
	
	_map.markAsDownloaded( offset, data.size() );
	
	//cerr << "Downloaded "<< _map.downloaded() <<" of "<<_size << endl;
	if ( _map.remaining() == 0 )
	{
		completeDownload();
	}
}

// ============================================================================
// ON download completed
void FileDownload::onDownloadCompleted( const shared_ptr<Source>& pSrc, const Error& err )
{
	// not until manager cames
	//pSrc->pConnection.reset()
	
	// update state
	_state = Idle;
	BOOST_FOREACH( const shared_ptr<Source>& pSrc, _sources )
	{
		if ( pSrc->pConnection )
		{
			if ( pSrc->pConnection->state() == Connection::Connected )
			{
				_state = Active;
				return;
			}
		}
	}
	
}

// ============================================================================
// find hat to download
void FileDownload::findWhatToDownload( Source* pSrc, uint64_t& start, uint64_t& len )
{
	_map.findFirstFree( start, len );
}

// ============================================================================
// Download completed
void FileDownload::completeDownload()
{
	// change state 
	_state = Completed;
	
	// remove 'tmp' suffix from temp file
	filesystem::path filePath( _path );
	string baseName = filePath.leaf();
	_tmpFile.close();
	
	rename( _tmpFileName.c_str(), baseName.c_str() );
	
	// remove nfo file
	unlink( _infoFileName.c_str() );
	
	// close all connections
	BOOST_FOREACH( const shared_ptr<Source>& pSrc, _sources )
	{
		if ( pSrc->pConnection )
		{
			pSrc->pConnection->disconnect();
		}
	}
	
	// announce success
	
}

}
