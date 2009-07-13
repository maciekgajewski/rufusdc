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
	: Singleton< Client >()
	, _tcpListener( this )
	, _connectionManager( this )
{
}

// ============================================================================
// Destructor
Client::~Client()
{
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


} // mamespace
