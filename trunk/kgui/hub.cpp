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

#include "rufusdc/hub.h"

#include "client.h"

#include "hub.h"

namespace KRufusDc
{


// ============================================================================
// Constructor
Hub::Hub( const shared_ptr<RufusDc::Hub>& pHub, Client *parent )
	: QObject( parent )
	, _pHub( pHub )
	, _pParent( parent )
{
	Q_ASSERT( parent );
	
	_thread = new ClientThreadHubAnchor( this );
	
	// connect self to QObject in the otherthread
	_thread->moveToThread( &parent->clientThread() );
	QObject::connect( _thread, SIGNAL(signalWtMessage(int,const QString&)), SLOT(wtMessage(int,const QString&)) );
	
	// connect the object with actual hub implementation
	pHub->signalChatMessage.connect( boost::bind( &ClientThreadHubAnchor::wtMessage, _thread, Chat, _1 ) );
	pHub->signalSystemMessage.connect( boost::bind( &ClientThreadHubAnchor::wtMessage, _thread, System, _1 ) );
	
	// cache this ASAP, to not fiddle threads later
	_address = pHub->address().c_str();
}

// ============================================================================
// Destructor
Hub::~Hub()
{
}

// ============================================================================
// message

void Hub::wtMessage( int type, const QString& msg )
{
	emit hubMessage( type, msg );
}

// ============================================================================
// other thread wt message
void ClientThreadHubAnchor::wtMessage( int type, const std::string& msg )
{
	emit signalWtMessage( type, msg.c_str() );
}

// ============================================================================
// Connect
void Hub::connect()
{
	Q_ASSERT( _pParent );
	
	_pParent->connectHub( _address );
}

// ============================================================================
// Disconnect
void Hub::disconnect()
{
	_pParent->disconnectHub( _address );
}

}

