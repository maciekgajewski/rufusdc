// Copyright (C) 2008 Maciek Gajewski <maciej.gajewski0@gmail.com>
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

// libc
#include <stdlib.h>

// boost
#include <boost/bind.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

// local
#include "hub.h"
#include "filelist.h"
#include "filedownload.h"

#include "client.h"

namespace RufusDc
{

// ============================================================================
// Constructor
Client::Client()
	: _tcpListener( this )
{
}

// ============================================================================
// Destructor
Client::~Client()
{
}

// ============================================================================
// get hub
shared_ptr<Hub> Client::getHub( const string& addr )
{
	lock_guard<mutex> guard( _hubsMutex );
	
	// TODO convert addr to canonical form
	
	HubMap::iterator it = _hubs.find( addr );
	
	if ( it != _hubs.end() )
	{
		return it->second;
	}
	
	shared_ptr<Hub> pHub = shared_ptr<Hub>( new Hub( this, addr ) );
	pHub->signalChatMessage.connect( boost::bind( &Client::onHubChatMessage, this, pHub.get(), _1 ) );
	pHub->signalSystemMessage.connect( boost::bind( &Client::onHubSystemMessage, this, pHub.get(), _1 ) );
	
	_hubs[addr] = pHub;
	return pHub;
}

// ============================================================================
// on hub message - slot 
void Client::onHubChatMessage( Hub* pHub, const string& msg )
{
	signalChatMessage( pHub, msg );
}

// ============================================================================
// on hub sys msg - slot
void Client::onHubSystemMessage( Hub* pHub, const string& msg )
{
	signalSystemMessage( pHub, msg );
}

// ============================================================================
// Settings constructor.
Client::Settings::Settings()
{
	maxUpload    = -1;
	maxDownload  = -1;
	nick         = "[psiepole]rufusdc";
	uploadSlots  = 10;
	tcpPort      = 3196;
	udpPort      = 11455;
	connection   = "0.5"; // whatever this means
	downloadDirectory = getenv("$HOME"); // TODO non portable
}

// ============================================================================
// Run event loop
void Client::run()
{
	// start listening
	_tcpListener.setPort( _settings.tcpPort );
	_tcpListener.startListening();
	
	_service.run();
}

// ============================================================================
// request trransfer
void Client::requestTransfer( Hub* pHub, const shared_ptr<DownloadRequest>& pRequest )
{
	// here decide if we are doing passive or active transfer
	// send ConnectToMe
	string addr = str(format( "%1%:%2%") % pHub->localIp() % _tcpListener.port() );
	
	pHub->sendCommand( "$ConnectToMe", assign::list_of( pRequest->nick() )( addr ) );
	
	// put request into queue
	_tcpListener.addRequest( pRequest );
}

// ============================================================================
// Called when file list is received
void Client::fileListReceived( vector<char>& data, DownloadRequest* pRequest )
{
	cerr << "File list received from user " << pRequest->nick() << endl;
	
	shared_ptr<FileList> pList = shared_ptr<FileList>( new FileList() );
	
	try
	{
		pList->fromBz2Data( data );
		pList->setNick( pRequest->nick() );
		pList->setHub( pRequest->hub() );
		
		signalIncomingFileList( pList );
	}
	catch( const std::exception& e )
	{
		cerr << "Error decodng file list: " << e.what() << endl;
	}
	
}

// ============================================================================
// Download file list
void Client::downloadFileList( const string& hub, const string& nick )
{
	// TODO convert address to canonical form
	
	HubMap::iterator hit =  _hubs.find( hub ) ;
	
	if ( hit == _hubs.end() )
	{
		throw std::logic_error("Can't donwload list from hub that is not connected");
	}
	
	// TODO actually use download manager here
	
	shared_ptr<DownloadRequest> pRequest = shared_ptr<DownloadRequest>( new DownloadRequest );
	
	pRequest->setNick( nick );
	pRequest->setFile( "files.xml.bz2" );
	// set up timeout (10 s )
	pRequest->setExpiryTime( posix_time::second_clock::local_time() + posix_time::seconds(10) );
	pRequest->signalTransferCompleted.connect( boost::bind( &Client::fileListReceived, this, _1, _2 ) );
	
	requestTransfer( hit->second.get(), pRequest );
}

// ============================================================================
// Download file
void Client::downloadFile
	( const string& hub
	, const string& nick
	, const string& path
	, const string& tth
	, uint64_t size
	)
{
	// TODO check for duplicates

	shared_ptr<FileDownload> pDownload( new FileDownload( this ) );
	pDownload->setSource( hub, nick );
	pDownload->setPath( path );
	pDownload->setTth( tth );
	pDownload->setSize( size );
	
	pDownload->start();
	
	_downloads.push_back( shared_ptr<Download>( pDownload ) );
}

} // mamespace
