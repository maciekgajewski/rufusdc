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
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>


// rufusdc
#include "directconnectionmanager.h"
#include "client.h"
#include "connectionrequest.h"

#include "hubmanager.h"

namespace RufusDc
{

// constants
static const int CONNECTION_REQUEST_TIMEOUT = 10; // conenction request timeot [s]

// ============================================================================
// constructor
HubManager::HubManager()
	: Singleton<HubManager>()
{
}

// ============================================================================
// destructor
HubManager::~HubManager()
{
}

// ============================================================================
// get hub
shared_ptr<Hub> HubManager::getHub( const string& addr )
{
	std::string key = makeCanonical( addr );
	
	HubMap::iterator it = _hubs.find( key );
	
	if ( it != _hubs.end() )
	{
		return it->second;
	}
	
	Hub::ptr pHub = Hub::ptr( new Hub( key ) );
	
	_hubs[addr] = pHub;
	return pHub;
}

// ============================================================================
// make canonical
std::string HubManager::makeCanonical( const std::string& addr )
{
	// TODO make adres canonical here
	return addr;
}

// ============================================================================
// Request connection
void HubManager::requestConnection
	( const string& hub
	, const string& nick
	, const ConnectionHandler& handler
	)
{
	// TODO support for cached connections here
	// TODO 2 not here, but in connection manager
	// TODO 3 move this method to conn manager
	
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
	string addr = str(format( "%1%:%2%") % pHub->localIp() % Client::ref().settings().tcpPort );
	pHub->sendCommand( "$ConnectToMe", assign::list_of( nick )( addr ) );
	
	// ok, file the request form
	shared_ptr<ConnectionRequest> pRequest( new ConnectionRequest );
	pRequest->setNick( nick );
	pRequest->setHub( hub );
	pRequest->setConnectedHandler( handler );
	pRequest->setExpiryTime( posix_time::second_clock::local_time() + posix_time::seconds(CONNECTION_REQUEST_TIMEOUT) );
	
	DirectConnectionManager::ref().addRequest( pRequest );
}


}

// EOF

