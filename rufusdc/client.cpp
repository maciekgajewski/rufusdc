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
#include "filelistdownload.h"
#include "connectionrequest.h"

#include "client.h"

namespace RufusDc
{

static const int CONNECTION_REQUEST_TIMEOUT = 10; // conenction request timeot [s]
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
	
	const char* _home = getenv("HOME"); // TODO non portable
	if ( _home )
	{
		downloadDirectory = _home;
	}
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
// Download file list
void Client::downloadFileList( const string& hub, const string& nick )
{
	shared_ptr<FileListDownload> pDownload( new FileListDownload( this ) );
	
	pDownload->setSource( hub, nick );
	pDownload->start();
	
	_downloads.push_back( shared_ptr<Download>( pDownload ) );
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

// ============================================================================
// Request connection
void Client::requestConnection
	( const string& hub
	, const string& nick
	, const ConnectionHandler& handler
	)
{
	cerr << "Requesting conection to user "<<nick<<"@"<<hub<<endl;
	HubMap::iterator hit =  _hubs.find( hub ) ;
	
	if ( hit == _hubs.end() )
	{
		throw std::logic_error("Hub is not connected");
	}
	
	Hub* pHub = hit->second.get();
	
	if ( ! pHub->hasUser( nick ) )
	{
		throw std::logic_error("User not connected");
	}
	
	// sent request
	string addr = str(format( "%1%:%2%") % pHub->localIp() % _tcpListener.port() );
	pHub->sendCommand( "$ConnectToMe", assign::list_of( nick )( addr ) );
	
	// ok, file the request form
	shared_ptr<ConnectionRequest> pRequest( new ConnectionRequest );
	pRequest->setNick( nick );
	pRequest->setHub( hub );
	pRequest->setConnectedHandler( handler );
	pRequest->setExpiryTime( posix_time::second_clock::local_time() + posix_time::seconds(CONNECTION_REQUEST_TIMEOUT) );
	
	_tcpListener.addRequest( pRequest );
}

// ============================================================================
// File list received
void Client::fileListReceived( const shared_ptr<FileList>& pList )
{
	// just emit signal
	signalIncomingFileList( pList );
}

} // mamespace
