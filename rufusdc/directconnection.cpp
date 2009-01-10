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
#include <boost/foreach.hpp>

// local
#include "protocolexception.h"
#include "client.h"

#include "directconnection.h"

namespace RufusDc
{

// ============================================================================
// Constructor
DirectConnection::DirectConnection( Client* pClient, shared_ptr<tcp::socket> pSocket )
		: Connection( pClient, "noaddress" )
{
	_pSocket = pSocket;
	_state = Connecting;
	_fileLength = 0;
	_direction = UnknownDirection;
	
	// populate handlers
	_handlers["$MyNick"]     = boost::bind( &DirectConnection::commandMyNick, this, _1 );
	_handlers["$Lock"]       = boost::bind( &DirectConnection::commandLock, this, _1 );
	_handlers["$Direction"]  = boost::bind( &DirectConnection::commandDirection, this, _1 );
	_handlers["$Key"]        = boost::bind( &DirectConnection::commandKey, this, _1 );
	_handlers["$FileLength"] = boost::bind( &DirectConnection::commandFileLength, this, _1 );
	_handlers["$ADCSND"]     = boost::bind( &DirectConnection::commandADCSND, this, _1 );
	_handlers["$Supports"]   = boost::bind( &DirectConnection::commandSupports, this, _1 );
	
}

// ============================================================================
// start
void DirectConnection::start()
{
	// wait for MyNick
	recvCommand();
}

// ============================================================================
// Destructor
DirectConnection::~DirectConnection()
{
}

// ============================================================================
// On chat
void DirectConnection::onIncomingChat( const string& msg )
{
	// TODO what should i do? probably disconnect?
}

// ============================================================================
// My nick
void DirectConnection::commandMyNick( const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string nick = params.front();
		
		cerr << "DirectConnection: nick received: " << nick << endl;
		_nick = nick;
	}
	else
	{
		throw ProtocolException("Peer client sent $MyNick without params");
	}
}

// ============================================================================
// Command lock
void DirectConnection::commandLock(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string lock = params.front();
		
		cerr << "DirectConnection: lock received: " << lock << endl;
		
		string key = calculateKey( lock );
		string mylock = "EXTENDEDPROTOCOLWELCOMETOJAMAICAHOLIDAY";
		sendCommand( "$MyNick", assign::list_of( _pParent->settings().nick ) );
		sendCommand( "$Lock", assign::list_of( mylock )( string("Pk=rufusdc-0.1") ) );
		sendCommand( "$Direction", assign::list_of("Download")("42") ); // TODO and what if we want to upload?
		sendCommand( "$Key", assign::list_of(key) );
	}
	else
	{
		throw ProtocolException("Peer client sent $Lock without params");
	}
}

// ============================================================================
// Directions
void DirectConnection::commandDirection(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string direction = params.front();
		
		// what now? drop if "Download"
		cerr << "DirectConnection: direction get: " << direction << endl;
		
		_direction = direction == "Download" ? Download : Upload;
	}
	else
	{
		throw ProtocolException("Peer client sent $Direction without params");
	}
}

// ============================================================================
// key
void DirectConnection::commandKey(  const list<string>& params )
{
	if ( params.size() >= 1 )
	{
		string key = params.front();
		// i'm trusting you endlessly, my friend
		cerr << "DirectConnection: got key\n";
		setState( Connected );
	}
	else
	{
		throw ProtocolException("Peer client sent $Key without params");
	}
}

// ============================================================================
// File length
void DirectConnection::commandFileLength(  const list<string>& params )
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
void DirectConnection::commandADCSND( const list<string>& params )
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
		
		_receivedBytes = 0;
		_fileLength = end - start;
		
		// use all data from buffer
		if ( _inBuffer.size() > 0 )
		{
			_receivedBytes += _inBuffer.size();
			_currentFileOffset += _inBuffer.size();
			
			vector<char> data( _inBuffer.size() );
			_inBuffer.sgetn( data.data(), _inBuffer.size() );
			_inBuffer.consume( _inBuffer.size() );
			
			//_pRequest->signalDataIncoming( data, _pRequest->offset() );
			_onIncomingData( data, _currentFileOffset );
			
		}
		
		
		recvData( _fileLength - _receivedBytes + 7 ); // TODO magick header 7 bytes here
		
	}
	else
	{
		throw ProtocolException("Peer client sent $ADCSND with too few params");
	}
}

// ============================================================================
// On incoming data
void DirectConnection::onIncomingData( vector<char>& buffer )
{
	// TODO handler should be checked here
	_receivedBytes += buffer.size();
	_onIncomingData( buffer, _currentFileOffset );
	_currentFileOffset += buffer.size();
	
	_onDownloadCompleted(Error());
	_onDownloadCompleted.clear();
}

// ============================================================================
// Supports
void DirectConnection::commandSupports( const list<string>& params )
{
	BOOST_FOREACH( string param, params )
	{
		_features.insert( param );
		cerr << "Client's feature: " << param << endl;
	}
}

// ============================================================================
// has featire?
bool DirectConnection::hasFeature( const string& feat ) const
{
	return _features.find(feat) != _features.end();
}

// ============================================================================
// Download file
void DirectConnection::downloadFile
	( const string& fileName
	, uint64_t offset
	, uint64_t count
	, const IncomingDataHandler& dataHandler
	, const CompletionHandler& completionHandler
	)
{
	_currentFileOffset = offset;
	_bytesRequested = count;
	_onIncomingData = dataHandler;
	_onDownloadCompleted = completionHandler;
	
	if ( _state == Connected && _direction == Upload && hasFeature("ADCGet" ) )
	{
		string strStart = lexical_cast<string>( offset );
		string strCount = count > 0 ? lexical_cast<string>( count ) : "-1";
		
		string fileRequests = str(format("file %1% %2% %3% ZL1") % fileName % strStart % strCount );
		
		cerr << "DirectConnection: sending $ADCGET " << fileRequests << endl;
		// TODO will other client live w/o this?
		sendCommand( "$ADCGET", assign::list_of( fileRequests ) );
	}
	else
	{
		disconnect();
		throw std::logic_error("Not connected, peer doesn't know ADCGET or doesn't want to send anythig to us");
	}
}

// ============================================================================
// disconnect
void DirectConnection::disconnect()
{
	// if anyone interested - let know that connection is lost
	if ( ! _onDownloadCompleted.empty() )
	{
		_onDownloadCompleted( Error("Connection lost") );
	}
	
	Connection::disconnect();
}

}
