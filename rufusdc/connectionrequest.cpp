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
#include "connectionrequest.h"

namespace RufusDc
{

// ============================================================================
// Constructor
ConnectionRequest::ConnectionRequest()
{
}

// ============================================================================
// Destructor
ConnectionRequest::~ConnectionRequest()
{
	cerr << "Connection request to "<<_nick<<"@"<<_hub<<" destroyed" << endl;
}

// ============================================================================
// Is expired
bool ConnectionRequest::isExpired()
{
	posix_time::ptime now( posix_time::second_clock::local_time());
	
	return now > _expiryTime;
}

// ============================================================================
// Connected
void ConnectionRequest::connected( shared_ptr<DirectConnection> pConnection )
{
	_connectedHandler( Error(), pConnection );
}

// ============================================================================
// Failed
void ConnectionRequest::failed( const Error& error )
{
	assert( error );
	
	_connectedHandler( error, shared_ptr<DirectConnection>() );
}

}
