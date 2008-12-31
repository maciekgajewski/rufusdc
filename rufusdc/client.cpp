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

#include <boost/bind.hpp>

#include "hub.h"

#include "client.h"

namespace RufusDc
{

// ============================================================================
// Constructor
Client::Client()
{
}

// ============================================================================
// Destructor
Client::~Client()
{
}

// ============================================================================
// Connect to hub
shared_ptr<Operation> Client::connectToHub( const string& hubAddr )
{
	// create hub. connect to it's signals
	shared_ptr<Hub> pHub = getHub( hubAddr );
	
	// connect!
	shared_ptr<Operation> operation = pHub->connect();
	_operations.push_back( operation );
	
	return operation;
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
	tcpPort      = 31996;
	udpPort      = 11455;
	connection   = "0.5"; // whatever this means
}

// ============================================================================
// Run event loop
void Client::run()
{
	_service.run();
}

} // mamespace
