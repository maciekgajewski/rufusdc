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

// boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// local
#include "client.h"
#include "directconnection.h"
#include "connectionrequest.h"

#include "directconnectionmanager.h"

namespace RufusDc
{

static const int TIMEOUT_TIMER_INTERVAL = 20; // [s]

// ============================================================================
// Constructor
DirectConnectionManager::DirectConnectionManager( Client* pClient )
	: _pParent( pClient )
	, _timer( pClient->ioService() )
{
	assert( pClient );
	
	_timer.expires_from_now(boost::posix_time::seconds(TIMEOUT_TIMER_INTERVAL));
	_timer.async_wait( boost::bind( &DirectConnectionManager::onTimer, this ) );
}

// ============================================================================
// Destructor
DirectConnectionManager::~DirectConnectionManager()
{
}

// ============================================================================
// Adds new connection
void DirectConnectionManager::newConnection( const shared_ptr<DirectConnection>& pConnection )
{
	if ( pConnection->state() != Connection::Disconnected )
	{
		_connections.push_back( pConnection );
	}
	else
	{
		throw std::logic_error("Connection manager: disconnected conenction supplied");
	}
}

// ============================================================================
// Clear closed connections
void DirectConnectionManager::removeClosedConnections()
{
	list< shared_ptr<DirectConnection> >::iterator it = _connections.begin();
	while( it != _connections.end() )
	{
		if ( (*it)->state() == Connection::Disconnected )
		{
			cerr << "Connection to user " << (*it)->nick() << "disconnected. Removing" << endl;
			it = _connections.erase( it );
		}
		else
		{
			++it;
		}
	}
}

// ============================================================================
// Removes expired requests
void DirectConnectionManager::removeExpiredRequests()
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
// Add expected connection
void DirectConnectionManager::addRequest( const shared_ptr<ConnectionRequest>& pRequest )
{
	_requests.push_back( pRequest );
}


// ============================================================================
// 
void DirectConnectionManager::connectionStateChanged( DirectConnection* pRawPointer, int state )
{
	// first - find the connection
	shared_ptr<DirectConnection> pConnection;
	
	BOOST_FOREACH( const shared_ptr<DirectConnection>& pConn, _connections )
	{
		if ( pConn.get() == pRawPointer )
		{
			pConnection = pConn;
			break;
		}
	}
	if ( ! pConnection )
	{
		throw std::logic_error("DirectConnectionManager::connected called by unknown connection");
	}
	
	// now find matching request
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
			cerr << "DirectConnectionManager: Conn w unknown state. Dropping";
		}
	}
	// else - let it live as upload connection
}


// ============================================================================
// on timer
void DirectConnectionManager::onTimer()
{
	removeExpiredRequests();
	removeClosedConnections();
	
	// se you in next 20 s
	_timer.expires_at( _timer.expires_at() + boost::posix_time::seconds(TIMEOUT_TIMER_INTERVAL) );
	_timer.async_wait( boost::bind( &DirectConnectionManager::onTimer, this ) );
}

}
