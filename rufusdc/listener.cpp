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

// ============================================================================
// Constructor
Listener::Listener( Client* pClient )
	: _pParent( pClient )
	, _acceptor( pClient->ioService() )
{
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
	
	tcp::acceptor::reuse_address option(true);
	_acceptor.set_option(option);

	_acceptor.bind( tcp::endpoint( tcp::v4(), _port));
	_acceptor.listen();
	
	accept();
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
		// debug
		cerr << "Listener: incoming connection\n";
		
		// create connection
		shared_ptr<DirectConnection> pConnection 
			= shared_ptr<DirectConnection>
				( new DirectConnection( DirectConnectionManager::pointer(), pSocket ) );
		
		DirectConnectionManager::ref().newConnection( pConnection );
		pConnection->start(); // go!
		
		// wait for next
		accept();
	}
	else
	{
		// TODO handle error
		cerr << "Error accepting connection: " << err.message() << endl;
	}
}


// ============================================================================
// Request accept
void Listener::accept()
{
	// create new socket for incoming connections
	shared_ptr<tcp::socket> pSocket( new tcp::socket( _pParent->ioService() ) );
	_acceptor.async_accept( *pSocket, boost::bind( &Listener::onAccept, this, pSocket, placeholders::error ) );
}

} // namespace

