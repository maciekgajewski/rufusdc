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
#include <boost/date_time/posix_time/posix_time_types.hpp>

// local
#include "protocolexception.h"
#include "client.h"

#include "directconnection.h"

namespace RufusDc
{

static const int BUFFER_SIZE = 4000;

// ============================================================================
// Constructor
DirectConnection::DirectConnection(  DirectConnectionManager* pParent, shared_ptr<tcp::socket> pSocket )
		: Connection( "noaddress" )
		, _pManager( pParent )
{
	_pSocket = pSocket;
	_state = Connecting;
	_fileLength = 0;
	_direction = UnknownDirection;
	
	_transferPool = 0;
	_transferLimited = false;
	_currentTransferSpeed = 0;
	
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
		sendCommand( "$MyNick", assign::list_of( Client::ref().settings().nick ) );
		sendCommand( "$Supports", assign::list_of("ADCGet")("XmlBZList")("TTHL")("TTHF") );
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
// State changed
void DirectConnection::setState( State state )
{
	if ( state != _state )
	{
		Connection::setState( state );
		_pManager->connectionStateChanged( this, int(state) );
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
		
		_fileLength = end - start;
		int bytesInBuffer = 0;
		
		// use all data from buffer
		if ( _inBuffer.size() > 0 )
		{
			bytesInBuffer = _inBuffer.size();
			_currentFileOffset += _inBuffer.size();
			
			vector<char> data( _inBuffer.size() );
			_inBuffer.sgetn( data.data(), _inBuffer.size() );
			_inBuffer.consume( _inBuffer.size() );
			
			//_pRequest->signalDataIncoming( data, _pRequest->offset() );
			_onIncomingData( data, _currentFileOffset );
			
		}
		
		// switch into data receive mode
		recvData( _fileLength - bytesInBuffer ); // TODO find out why tjhere is a magic 7-bytes header!
		
	}
	else
	{
		throw ProtocolException("Peer client sent $ADCSND with too few params");
	}
}

// ============================================================================
// Recv
void DirectConnection::recvData( uint64_t size )
{
	// TODO do something if data still in buffer
	if ( _inBuffer.size() > 0 )
	{
		cerr << "WARNING: recvData: " << _inBuffer.size() << " bytes still in buffer" << endl;
	}
	
	// reset counters
	_receivedBytes = 0;
	_bytesRequested = size;
	
	
	// check id data can be received
	if ( _transferPool > 0 || ! _transferLimited )
	{
		startReceiving();
	}
	
	// stop receiving commands
	_dataTransfer = true;
}

// ============================================================================
// start receiving
void DirectConnection::startReceiving()
{
	assert( _pSocket );
	
	// prepare buffer
	_dataBuffer.resize( BUFFER_SIZE ); // actual content is irrelevant
	
	// start receiving
	_pSocket->async_read_some
		( asio::buffer( _dataBuffer.data(), _dataBuffer.size() )
		, boost::bind( &DirectConnection::onDataReceived, this, _1, _2 )
		);
	
	_transferStart = posix_time::microsec_clock::local_time();
}

// ============================================================================
// Received enough data
void DirectConnection::onDataReceived( const system::error_code& err, int size )
{
	if ( ! err )
	{
		posix_time::time_duration duration = ( posix_time::microsec_clock::local_time() - _transferStart ); // how long the data was downloaded
		_downloadTime += duration;
		
		// calculate momentary speed
		_currentTransferSpeed = ( 1E6 * size ) / duration.total_microseconds();
		
		int bytesIncoming = size;
		if ( size > ( _bytesRequested - _receivedBytes ) )
		{
			bytesIncoming = _bytesRequested - _receivedBytes;
		}
			
		if ( ! _onIncomingData.empty() )
		{
			_dataBuffer.resize( bytesIncoming ); // this always shrinks
			_onIncomingData( _dataBuffer, _currentFileOffset );
		}
		else
		{
			cerr << "WARNING: data incomong, bu no one interested!!!" << endl;
		}
		
		_currentFileOffset += bytesIncoming;
		_receivedBytes += size;
		
		// receive more?
		if ( _receivedBytes >= _bytesRequested )
		{
			// TODO debug: check it htere is a surplus
			if ( _receivedBytes > _bytesRequested )
			{
				cerr << "WARNING: too many bytes received: " << _receivedBytes - _bytesRequested << endl;
			}
			// the end
			_onDownloadCompleted(Error());
			_onDownloadCompleted.clear();
			
			// TODO switch to command mode
		}
		else if ( _transferPool > 0 || ! _transferLimited )
		{
			startReceiving();
		}
	
		// else - wait untill pool grows
	}
	else
	{
		// error
		cerr << "Error receving data:  " << err.message() << endl;
		
		if ( ! _onDownloadCompleted.empty() )
		{
			_onDownloadCompleted( Error(err.message()) );
			_onDownloadCompleted.clear();
		}
		
		disconnect();
	}
}

// ============================================================================
// Supports
void DirectConnection::commandSupports( const list<string>& params )
{
	BOOST_FOREACH( string param, params )
	{
		_features.insert( param );
		//cerr << "Client's feature: " << param << endl;
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
		
		//string fileRequests = str(format("file %1% %2% %3% ZL1") % fileName % strStart % strCount );
		string fileRequests = str(format("file %1% %2% %3%") % fileName % strStart % strCount ); // NOTE: ZL1 removed
		
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

// ============================================================================
// Total transfer speed
double DirectConnection::totalTransferSpeed() const
{
	return ( 1000.0 * _receivedBytes ) / _downloadTime.total_milliseconds();
}

}
