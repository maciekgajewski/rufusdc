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

// local
#include "client.h"

#include "listener.h"

namespace RufusDc
{

// ============================================================================
// Constructor
Listener::Listener( Client* pClient )
	: _pParent( pClient )
	, _acceptor( pClient->ioService() )
{
	assert( pClient );
}

// ============================================================================
// Destructor
Listener::~Listener()
{
}

// ============================================================================
// Sets port
void Listener::setPort( int port )
{
	if ( port != _port )
	{
		_port = port;
	}
}

// ============================================================================
// Starts listening
void Listener::startListening()
{
	if ( _port == 0 )
	{
		throw std::invalid_argument("Listener: no port configured");
	}
	
	_acceptor.open( tcp::v4() );
	_acceptor.bind( tcp::endpoint( tcp::v4(), _port));
	_acceptor.listen();
	
	// start listening if there are requests
	removeExpiredRequests();
	removeCompletedDownloads();
	
	if ( ! _requests.empty() )
	{
		accept();
	}
	
}

// ============================================================================
// Stops listening
void Listener::stopListening()
{
	_acceptor.close();
}

// ============================================================================
// on accept
void Listener::onAccept( shared_ptr<tcp::socket> pSocket, const system::error_code err )
{
	if ( ! err )
	{
		removeCompletedDownloads();
		removeExpiredRequests();
	
		// debug
		cerr << "Listener: incoming connection\n";
			
		// is there any request left?
		if ( ! _requests.empty() )
		{
			shared_ptr<ActiveDownload> pDownload 
				= shared_ptr<ActiveDownload>( new ActiveDownload( _pParent, pSocket ) );
				
			pDownload->signalRequest.connect( boost::bind( &Listener::takeRequest, this, _1, _2 ) );
			
			pDownload->start();
			
			_downloads.push_back( pDownload );
		
			// wait for next
			accept();
		}
		else
		{
			// close the connection, wait for better times
			cerr << "Incoming connection dropped, no one interested\n";
			//_pSocket.reset(); // if another accept was called in the meantime, we are screwed!
		}
	}
	else
	{
		// TODO handle error
		cerr << "Error accepting connection: " << err.message() << endl;
	}
}

// ============================================================================
// Clear completed downloads
void Listener::removeCompletedDownloads()
{
	list< shared_ptr<ActiveDownload> >::iterator it = _downloads.begin();
	while( it != _downloads.end() )
	{
		if ( (*it)->state() == Connection::Disconnected )
		{
			//BEGIN debug
			const shared_ptr<DownloadRequest>& rq = (*it)->request();
			if ( rq )
			{
				cerr << "Removing completed download from user " << rq->nick() << endl;
			}
			else
			{
				cerr << "Removing unassociated download" << endl;
			}
			//END debug
			it = _downloads.erase( it );
		}
		else
		{
			++it;
		}
	}
}

// ============================================================================
// Add expected connection
void Listener::addRequest( const shared_ptr<DownloadRequest>& pRequest )
{
	// is the first request?
	if ( _requests.empty() && _acceptor.is_open() )
	{
		accept(); // start accepting then!
	}
	
	_requests.push_back( pRequest );
}

// ============================================================================
// Request accept
void Listener::accept()
{
	// create new socket for incoming connections
	/*
	_pSocket = shared_ptr<tcp::socket>( new tcp::socket( _pParent->ioService() ) );
	// NOTE: wrong! cant use single pointer used by - possibly - multiple accept calls
	*/
	shared_ptr<tcp::socket> pSocket( new tcp::socket( _pParent->ioService() ) );
	_acceptor.async_accept( *pSocket, boost::bind( &Listener::onAccept, this, pSocket, placeholders::error ) );
}

// ============================================================================
// Removes expired requests
void Listener::removeExpiredRequests()
{
	list< shared_ptr<DownloadRequest> >::iterator it = _requests.begin();
	while( it != _requests.end() )
	{
		if ( (*it)->isExpired() )
		{
			cerr << "Doenload requests of file "<< (*it)->file() << " from user " << (*it)->nick() << " expired" << endl;
			it = _requests.erase( it );
		}
		else
		{
			++it;
		}
	}
}

// ============================================================================
// Take request
void Listener::takeRequest( const string& nick, shared_ptr<DownloadRequest>& out )
{
	list< shared_ptr<DownloadRequest> >::iterator it = _requests.begin();
	
	while( it != _requests.end() ) 
	{
		if ( (*it)->nick() == nick )
		{
			out = *it;
			_requests.erase( it );
			return;
		}
		++it;
	}
	
	// when not found, leave out intouched
	cerr << "Listener:: Cant fin'd request for nick " << nick << endl;
	
}

}
