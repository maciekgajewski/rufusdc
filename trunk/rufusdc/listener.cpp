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
#include "connectionrequest.h"
#include "directconnection.h"

#include "listener.h"

namespace RufusDc
{

static const int TIMEOUT_TIMER_INTERVAL = 20; // [s]

// ============================================================================
// Constructor
Listener::Listener( Client* pClient )
	: _pParent( pClient )
	, _acceptor( pClient->ioService() )
	, _timer( pClient->ioService() )
{
	assert( pClient );
	
	_timer.expires_from_now(boost::posix_time::seconds(TIMEOUT_TIMER_INTERVAL));
	_timer.async_wait( boost::bind( &Listener::onTimer, this ) );
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
		removeExpiredRequests();
	
		// debug
		cerr << "Listener: incoming connection\n";
			
		// is there any request left?
		if ( ! _requests.empty() )
		{
			shared_ptr<DirectConnection> pConnection 
				= shared_ptr<DirectConnection>( new DirectConnection( _pParent, pSocket ) );
				
			// connect
			boost::signals::connection connection = pConnection->signalStateChanged.connect
				( boost::bind
					( &Listener::slotConnectionStateChanged
					, this
					, pConnection.get()
					, _1
					)
				);
			
			pConnection->start();
			
			ConnDesc cd;
			cd.connection = pConnection;
			cd.signalConnection = connection;
			
			_connections.push_back( cd );
		
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
void Listener::removeCompletedConnections()
{
	assert(false); // is the method needed?
	/*
	list< shared_ptr<DirectConnection> >::iterator it = _downloads.begin();
	while( it != _downloads.end() )
	{
		if ( (*it)->state() == Connection::Disconnected )
		{
			it = _downloads.erase( it );
		}
		else
		{
			++it;
		}
	}
	*/
}

// ============================================================================
// Add expected connection
void Listener::addRequest( const shared_ptr<ConnectionRequest>& pRequest )
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
	shared_ptr<tcp::socket> pSocket( new tcp::socket( _pParent->ioService() ) );
	_acceptor.async_accept( *pSocket, boost::bind( &Listener::onAccept, this, pSocket, placeholders::error ) );
}

// ============================================================================
// Removes expired requests
void Listener::removeExpiredRequests()
{
	list< shared_ptr<ConnectionRequest> >::iterator it = _requests.begin();
	while( it != _requests.end() )
	{
		if ( (*it)->isExpired() )
		{
			cerr << "Connection requests to user " << (*it)->nick() << "@" <<  (*it)->hub() <<" expired" << endl;
			(*it)->failed(Error("Timed out"));
			it = _requests.erase( it );
		}
		else
		{
			++it;
		}
	}
}

// ============================================================================
// State changed
void Listener::slotConnectionStateChanged( DirectConnection* pConnection,  int state )
{
	// serach and remove approrpriate ConDesc
	list< ConnDesc >::iterator it = _connections.begin();
	
	while( it != _connections.end() ) 
	{
		if ( it->connection.get() == pConnection )
		{
			// stop observing
			it->signalConnection.disconnect();
			
			// handle state change
			connectionStateChanged( it->connection, state );
			
			// stop waitying for it
			_connections.erase( it );
			return;
		}
		++it;
	}
	
	cerr << "Listener::slotConnectionStateChanged: Unexpected connection changed state" << endl;
}

// ============================================================================
// Connection state changed
void Listener::connectionStateChanged( shared_ptr<DirectConnection> pConnection, int state )
{
	string nick = pConnection->nick();
	
	// find request for the connection
	shared_ptr<ConnectionRequest> pRequest;
	list< shared_ptr<ConnectionRequest> >::iterator it = _requests.begin();
	
	while( it != _requests.end() ) 
	{
		if ( (*it)->nick() == nick )
		{
			pRequest = *it;
			_requests.erase( it );
			break;
		}
		++it;
	}
	
	// is there a request?
	if ( pRequest )
	{
		
		if ( state == Connection::Disconnected )
		{
			//cerr << "Listener: Conn failed" << endl;
			pRequest->failed(Error("Connection failed"));
		}
		else if ( state == Connection::Connected )
		{
			//cerr << "Listener: Conn successfull" << endl;
			pRequest->connected( pConnection );
		}
		else
		{
			cerr << "Listener: Conn w unknown state. Dropping";
		}
	}
	else
	{
		cerr << "Listener: Conn w/o request. Dropping.";
		pConnection->disconnect();
	}
}

// ============================================================================
// on timer
void Listener::onTimer()
{
	removeExpiredRequests();
	
	// se you in next 20 s
	_timer.expires_at( _timer.expires_at() + boost::posix_time::seconds(TIMEOUT_TIMER_INTERVAL) );
	_timer.async_wait( boost::bind( &Listener::onTimer, this ) );}


}
