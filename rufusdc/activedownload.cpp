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
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>

// local
#include "protocolexception.h"
#include "client.h"

#include "activedownload.h"

namespace RufusDc
{

// ============================================================================
// Constructor
ActiveDownload::ActiveDownload( Client* pClient, shared_ptr<tcp::socket> pSocket )
		: Connection( pClient, "noaddress" )
{
	_pSocket = pSocket;
	_state = Connected;
	_fileLength = 0;
	
	// populate handlers
	_handlers["$MyNick"]     = boost::bind( &ActiveDownload::commandMyNick, this, _1 );
	_handlers["$Lock"]       = boost::bind( &ActiveDownload::commandLock, this, _1 );
	_handlers["$Direction"]  = boost::bind( &ActiveDownload::commandDirection, this, _1 );
	_handlers["$Key"]        = boost::bind( &ActiveDownload::commandKey, this, _1 );
	_handlers["$FileLength"] = boost::bind( &ActiveDownload::commandFileLength, this, _1 );
	_handlers["$ADCSND"]     = boost::bind( &ActiveDownload::commandADCSND, this, _1 );
	
}

// ============================================================================
// start
void ActiveDownload::start()
{
	// wait for MyNick
	recvCommand();
}

// ============================================================================
// Destructor
ActiveDownload::~ActiveDownload()
{
}

// ============================================================================
// On chat
void ActiveDownload::onIncomingChat( const string& msg )
{
	// TODO what should i do? probably disconnect?
}

// ============================================================================
// My nick
void ActiveDownload::commandMyNick( const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string nick = params.front();
		
		cerr << "ActiveDownload: nick received: " << nick << endl;
		
		signalRequest( nick, _pRequest );
		
		if ( _pRequest )
		{
			// TODO do something
			// TODO or nothing?
			cerr << "ActiveDownload: request recived, file requested: " << _pRequest->file() << endl;
		}
		else
		{
			disconnect();
		}
	}
	else
	{
		throw ProtocolException("Peer client sent $MyNick without params");
	}
}

// ============================================================================
// Command lock
void ActiveDownload::commandLock(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string lock = params.front();
		
		cerr << "ActiveDownload: lock received: " << lock << endl;
		
		string key = calculateKey( lock );
		string mylock = "EXTENDEDPROTOCOLWELCOMETOJAMAICAHOLIDAY";
		sendCommand( "$MyNick", assign::list_of( _pParent->settings().nick ) );
		sendCommand( "$Lock", assign::list_of( mylock )( string("Pk=rufusdc-0.1") ) );
		sendCommand( "$Direction", assign::list_of("Download")("42") );
		sendCommand( "$Key", assign::list_of(key) );
	}
	else
	{
		throw ProtocolException("Peer client sent $Lock without params");
	}
}

// ============================================================================
// Directions
void ActiveDownload::commandDirection(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string direction = params.front();
		
		// what now? drop if "Download"
		cerr << "ActiveDownload: direction get: " << direction << endl;
		
		if ( direction != "Upload" )
		{
			throw ProtocolException("Peer client misbehaved and requested download\n");
		}
	}
	else
	{
		throw ProtocolException("Peer client sent $Direction without params");
	}
}

// ============================================================================
// key
void ActiveDownload::commandKey(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string key = params.front();
		// i'm trusting you endlessly, my friend
		cerr << "ActiveDownload: got key\n";
		if ( _pRequest )
		{
			string fileRequests = str(format("file %1% 0 -1 ZL1") % _pRequest->file() );
		
			cerr << "ActiveDownload: sending $ADCGET " << fileRequests << endl;
			sendCommand( "$ADCGET", assign::list_of( fileRequests ) );
		}
		else
		{
			cerr << "ActiveDownload: $Key send, while no request\n";
			disconnect();
		}
	}
	else
	{
		throw ProtocolException("Peer client sent $Key without params");
	}
}

// ============================================================================
// File length
void ActiveDownload::commandFileLength(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string length = params.front();
		_fileLength = boost::lexical_cast<uint64_t>( length );
	
		cerr << "FileLength received: " << length << endl;
		// TODO receive here
	}
	else
	{
		throw ProtocolException("Peer client sent $FileLength without params");
	}
}

// ============================================================================
// ADCSND
void ActiveDownload::commandADCSND( const list<string>& params )
{
	if ( params.size() >= 4 )
	{
		list<string>::const_iterator it = params.begin();
		
		++it; // "file"
		string filename = *(it++);
		string startstr = *(it++);
		string endstr = *(it++);
		
		cerr << "received file data for file " << filename << " :" << startstr << " - " << endstr << endl;
		
		uint64_t start = lexical_cast<int64_t>( startstr );
		uint64_t end = lexical_cast<int64_t>( endstr );
		
		_recivedBytes = 0;
		_fileLength = end - start;
		
		recvData( _fileLength );
		
	}
	else
	{
		throw ProtocolException("Peer client sent $ADCSND with too few params");
	}
}

// ============================================================================
// On incoming data
void ActiveDownload::onIncomingData( vector<char>& buffer )
{
	// announce completion and say goodbye
	_pRequest->signalTransferCompleted( buffer, _pRequest.get() );
	
	disconnect();
}

}
