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
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include "protocolexception.h"
#include "client.h"

#include "connection.h"

namespace RufusDc
{

// ============================================================================
// Constructor
Connection::Connection( const string& address )
	: _address( address )
{
	_state = Disconnected;
	_dataTransfer = false;
}

// ============================================================================
// Destructor
Connection::~Connection()
{
	_pSocket.reset(); // this will remove and close socket
	_state = Disconnected;
	cerr << "Connection destroyed" << endl;
}

// ============================================================================
// Connects to hub
void Connection::connect()
{
	string desc = str( format("Connecting to %1%") % _address );
	systemMessage( desc );
	
	setState( Connecting );
	
	try
	{
		// translate address string into more concrete data
		tcp::resolver::query query = parseAddress( _address );
		
		_pResolver = shared_ptr<tcp::resolver>( new tcp::resolver( ioService() ) );
		_pResolver->async_resolve
			( query
			, boost::bind
				( & Connection::onResolve
				, this
				, asio::placeholders::error
				, asio::placeholders::iterator
				)
			);
			
			
	}
	catch( const std::exception& e )
	{
		systemMessage( str(format("Error conecting to %1%: %2%") % _address % e.what() ) );
	}
}

// ============================================================================
// Disconect
void Connection::disconnect()
{
	systemMessage( "Disconnecting" );
	cerr << "Connection disconnected" << endl;
	
	_pSocket.reset(); // this will remove and close socket
	setState( Disconnected );
}

// ============================================================================
// Changes state
void Connection::setState( State state )
{
	if ( state != _state )
	{
		_state = state;
		signalStateChanged( int(state) );
	}
}
// ============================================================================
// IO Service
io_service& Connection::ioService()
{
	return Client::ref().ioService();
}

// ============================================================================
// Parse adress
tcp::resolver::query Connection::parseAddress( const string& address ) throw ( std::invalid_argument )
{
	// first check
	if ( address.empty() )
	{
		throw std::invalid_argument("address empty");
	}
	
	// first attempt: try to parse host:port format
	regex e1("([\\w\\.]+):(\\d+)");
	smatch what;
	if( regex_match( address, what, e1 ) )
	{
		string addr;
		string port;
		
		addr = what[1];
		port = what[2];
		
		return tcp::resolver::query( addr, port );
	}
	// TODO attept to parse URL here: proto://addr:port
	throw std::invalid_argument("can't parse address");
	// NADA
}

// ============================================================================
// on resolve
void Connection::onResolve( const system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
	if ( ! err )
	{
		// create socket, connect
		tcp::endpoint endpoint = *endpoint_iterator;
		
		_pSocket = shared_ptr<tcp::socket>( new tcp::socket( ioService() ) );
		
		_pSocket->async_connect
			( endpoint
			, boost::bind
				( &Connection::onConnect
				, this
				, asio::placeholders::error
				)
			);
		
	}
	else
	{
		systemMessage( str(format("Error resolving address: %1%")%err.message() ) );
	}
}

// ============================================================================
// On connect
void Connection::onConnect( const system::error_code& err )
{
	if ( ! err && _pSocket ) // pSocket may be destroyed by disconect in the meantime
	{
		// start receiving messages here
		recvCommand();
	}
	else
	{
		// error
		systemMessage( str(format("Error connecting to %1%: %2%") % _address % err.message() ) );
	}
}

// ============================================================================
// Local Ip
string Connection::localIp() const
{
	assert( _pSocket );
	string addr = _pSocket->local_endpoint().address().to_string();
	return addr;
}

// ============================================================================
// Send
void Connection::send( const string& msg )
{
	if ( ! _pSocket || ! _pSocket->is_open() )
	{
		throw invalid_argument("not connected");
	}
	
	shared_ptr<asio::streambuf> pBuf( new asio::streambuf );
	ostream s( pBuf.get() );
	s << msg;
	s << '|'; // endline
	
	// debug
	//cerr << " >>>> sent: " << msg << endl;
	
	async_write
		( *_pSocket
		, *pBuf
		, boost::bind
			( &Connection::onSend
			, this
			, placeholders::error
			, pBuf
			)
		);
}

// ============================================================================
// Senb command
void Connection::sendCommand( const string& cmd, const list<string>& params )
{
	stringstream ss;
	ss << cmd;
	
	BOOST_FOREACH( string p, params )
	{
		ss << " " << p;
	}
	
	send( ss.str() );
}

// ============================================================================
// On Send
void Connection::onSend( const system::error_code& err, shared_ptr<asio::streambuf> pBuf  )
{
	pBuf.reset(); // release buffer
	if ( err )
	{
		cerr << "error sending: " << err.message() << endl;
		disconnect();
	}
}

// ============================================================================
// Incoming message
void Connection::onIncomingMessage( const string& msg )
{
	// TODO debug
	//cerr << "msg:" << msg << endl;
	
	if ( msg.empty() )
	{
		// do nothing
		//cerr << "Warning: empty message" << endl;
		return;
	}
	
	// is comand?
	if ( msg[0] == '$' )
	{
		string command;
		list<string> params;
		
		// manual method - split by spaces. stringstream sucks in this
		string substr;
		for( int i = 0; i < msg.length(); i++ )
		{
			if ( msg[i] == ' ' )
			{
				if ( substr.length() )
				{
					if ( command == "" )
					{
						command = substr;
					}
					else
					{
						params.push_back( substr );
					}
					substr = "";
				}
			}
			else
			{
				substr.push_back( msg[i] );
			}
		}
		if ( substr.length() )
		{
			params.push_back( substr );
		}
		
		
		onIncomingCommand( command, params );
	}
	else
	{
		onIncomingChat( msg );
	}
}

// ============================================================================
// On incoming command
void Connection::onIncomingCommand( const string& command, const list<string>& params )
{
	map<string,CommandHandler>::iterator hit = _handlers.find( command );
	if ( hit == _handlers.end() )
	{
		// TODO error
		cerr << "Unsupported command "<< command << endl;
		/*
		BOOST_FOREACH( string p, params )
		{
			cerr << " * " << p << endl;
		}
		*/
		return;
	}
	
	CommandHandler handler = hit->second;
	
	try
	{
		handler( params );
	}
	catch( const std::exception& e )
	{
		systemMessage( str( format("Error handling command '%1%' : %2%") % command % e.what() ) );
	}
}

// ============================================================================
// Calculate key
string Connection::calculateKey( const string& lock )
{
	string key;
	int len = lock.length();
	
	// NOTE: code pasted from DC reference (http://www.teamfair.info/DC-Protocol.htm)
	key.push_back( lock[0] xor lock[len-1] xor lock[len-2] xor 5 );
	
	for (int i = 1; i < len; i++ )
		key.push_back(  lock[i] xor lock[i-1] );
		
	// nibble-swap
	for (int i = 0; i < len; i++)
		key[i] = ((key[i]<<4) & 240) | ((key[i]>>4) & 15);
		
	// TODO escape
	return escape(key);
}

// ============================================================================
// escape
string Connection::escape( const string& str )
{
	string result;
	
	BOOST_FOREACH( char c, str )
	{
		switch( c )
		{
			case 0:
			case 5:
			case 36:
			case 96:
			case 124:
			case 126:
			{
				string seq = boost::str( format("/%%DCN%03d%%/") % int(c) );
				result +=  seq ;
				break;
			}
				
			default:
				result.push_back(c);
		}
	}
	
	return result;
}

// ============================================================================
// System message.
void Connection::systemMessage( const string& msg )
{
	signalSystemMessage( msg );
}

// ============================================================================
// Recv
void Connection::recvCommand()
{
	asio::async_read_until
		( *_pSocket
		, _inBuffer
		, char('|')
		, boost::bind
			( &Connection::onRecvCommand
			, this
			, asio::placeholders::error
			, placeholders::bytes_transferred
			)
		);
	
	_dataTransfer = false;
}


// ============================================================================
// on recv
void Connection::onRecvCommand( const system::error_code& err, int size )
{
	if ( ! err && _pSocket )
	{
		int data_in_buffer = _inBuffer.size();
		
		
		for( int i = 0; i < data_in_buffer; i++ )
		{
			char c = _inBuffer.sbumpc();
			
			if ( c == '|' )
			{
				onIncomingMessage( _command );
				_command.clear();
				
				// get out if switched to data transfer or disconnected
				if ( _dataTransfer || _state  == Disconnected )
				{
					break;
				}
			}
			else
			{
				_command.push_back( c );
			}
		}
		// put back command into buffer
		
		
		// if still connected, and data trasmitted, start receiving next one
		if ( _state != Disconnected && ! _dataTransfer )
		{
			recvCommand();
		}
		
	}
	else
	{
		// error!
		cerr << "onRecvCommand error: " << err.message() << endl;
		if ( err.value() != error::operation_aborted && _pSocket ) // that means tah the operatio ncomes form another life
		{
			disconnect();
		}
	}
}

}
