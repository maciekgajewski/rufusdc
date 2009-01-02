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

// TEST: hubdc.lanet.net.pl:4012

#include <cassert>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>


#include "protocolexception.h"
#include "client.h"

#include "hub.h"

namespace RufusDc
{

map< string, Hub::HubCommandHandler > Hub::_handlers;

// ============================================================================
// Constructor
Hub::Hub( Client* pParent, const string& addr )
	: _pParent( pParent )
	, _address( addr )
{
	assert( pParent );
	
	// init handlers
	if ( _handlers.empty() )
	{
		_handlers["$Lock"] = &Hub::commandLock;
		_handlers["$HubName"] = &Hub::commandHubName;
		_handlers["$ValidateDenide"] = &Hub::commandValidateDenide;
		_handlers["$GetPass"] = &Hub::commandGetPass;
		_handlers["$LogedIn"] = &Hub::commandLogedIn;
		_handlers["$BadPass"] = &Hub::commandBadPass;
		_handlers["$Hello"] = &Hub::commandHello;
		_handlers["$MyINFO"] = &Hub::commandMyINFO;
		_handlers["$Quit"] = &Hub::commandQuit;
		_handlers["$HubTopic"] = &Hub::commandHubTopic;
	}
	
	_state = Disconnected;
}

// ============================================================================
// Destructor
Hub::~Hub()
{
}

// ============================================================================
// Changes state
void Hub::setState( State state )
{
	if ( state != _state )
	{
		_state = state;
		signalStateChanged( int(state) );
	}
}

// ============================================================================
// Connects to hub
shared_ptr<Operation> Hub::connect()
{
	string desc = str( format("Connecting to hub %1%") % _address );
	systemMessage( desc );
	
	setState( Connecting );
	
	// create connect operation
	_pConnectOperation = shared_ptr<Operation>( new Operation( desc ) );
	
	_pConnectOperation->setStatusDescription( "Resolving hub address" );
	
	try
	{
		// translate address string into more concrete data
		tcp::resolver::query query = parseAddress( _address );
		
		_pResolver = shared_ptr<tcp::resolver>( new tcp::resolver( ioService() ) );
		_pResolver->async_resolve
			( query
			, boost::bind
				( & Hub::onResolve
				, this
				, asio::placeholders::error
				, asio::placeholders::iterator
				)
			);
			
			
	}
	catch( const exception& e )
	{
		_pConnectOperation->beginChange();
			_pConnectOperation->setStatusDescription( e.what() );
			_pConnectOperation->setStatus( Operation::Failed );
		_pConnectOperation->endChange();
		
		systemMessage( str(format("Error conecting to hub: %1%") % e.what() ) );
		
		shared_ptr<Operation> ret = _pConnectOperation;
		
		// forget the operation
		_pConnectOperation.reset();
		
		return ret;
	}

	return _pConnectOperation;
}

// ============================================================================
// Disconect
void Hub::disconnect()
{
	lock_guard<mutex> guard( _usersMutex );

	systemMessage( "Disconnecting" );
	
	_pSocket.reset(); // this will remove and close socket
	_users.clear();
	setState( Disconnected );
}

// ============================================================================
// IO Service
io_service& Hub::ioService()
{
	return _pParent->ioService();
}

// ============================================================================
// Parse adress
tcp::resolver::query Hub::parseAddress( const string& address ) throw ( std::invalid_argument )
{
	// first check
	if ( address.empty() )
	{
		throw std::invalid_argument("hub address empty");
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
	throw std::invalid_argument("can't parse hub address");
	// NADA
}

// ============================================================================
// on resolve
void Hub::onResolve( const system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
	assert( _pConnectOperation );
	
	_pConnectOperation->setProgress( 25 ); 
	
	if ( ! err )
	{
		// resolved! use only first found endpoint
		_pConnectOperation->setStatusDescription("Connecting to host");
		// create socket, connect
		tcp::endpoint endpoint = *endpoint_iterator;
		
		_pSocket = shared_ptr<tcp::socket>( new tcp::socket( ioService() ) );
		
		_pSocket->async_connect
			( endpoint
			, boost::bind
				( &Hub::onConnect
				, this
				, asio::placeholders::error
				)
			);
		
	}
	else
	{
		// error
		_pConnectOperation->beginChange();
			_pConnectOperation->setStatusDescription( err.message() );
			_pConnectOperation->setStatus( Operation::Failed );
		_pConnectOperation->endChange();
		_pConnectOperation.reset();
		
		systemMessage( str(format("Error resolving hub address: %1%")%err.message() ) );
	}
}

void Hub::onConnect( const system::error_code& err )
{
	assert( _pConnectOperation );
	assert( _pSocket );
	
	_pConnectOperation->setProgress( 50 ); 
	
	if ( ! err )
	{
		_pConnectOperation->setStatusDescription("Logging in");
		
		// start connection here
		
		// start receiving messages here
		recv();
		
	}
	else
	{
		// error
		_pConnectOperation->beginChange();
			_pConnectOperation->setStatusDescription( err.message() );
			_pConnectOperation->setStatus( Operation::Failed );
		_pConnectOperation->endChange();
		_pConnectOperation.reset();
		
		systemMessage( str(format("Error connecting to hub: %1%")%err.message() ) );
	}
}

// ============================================================================
// Send
void Hub::send( const string& msg )
{
	if ( ! _pSocket || ! _pSocket->is_open() )
	{
		throw invalid_argument("not connected to hub");
	}
	
	ostream s( &_outBuffer );
	s << msg;
	s << '|'; // endline
	
	// debug
	//cerr << " >>>> sent: " << msg << endl;
	
	async_write
		( *_pSocket
		, _outBuffer
		, boost::bind
			( &Hub::onSend
			, this
			, placeholders::error
			)
		);
}

// ============================================================================
// Senb command
void Hub::sendCommand( const string& cmd, const list<string>& params )
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
void Hub::onSend( const system::error_code& err )
{
	// TODO
}

// ============================================================================
// Recv
void Hub::recv()
{
	asio::async_read_until
		( *_pSocket
		, _inBuffer
		, char('|')
		, boost::bind
			( &Hub::onRecv
			, this
			, asio::placeholders::error
			, placeholders::bytes_transferred
			)
		);
}

// ============================================================================
// on recv
void Hub::onRecv( const system::error_code& err, int size )
{
	assert( _pSocket );
	
	if ( ! err )
	{
		stringstream ss;
		ss << & _inBuffer;
		//cout << "incoming: " << ss.str() << endl;
		
		string incoming = ss.str();
		for( int i = 0; i < incoming.length(); i++ )
		{
			if ( incoming[i] == '|' )
			{
				onIncomingMessage( _command );
				_command = string();
			}
			else
			{
				_command.push_back( incoming[i] );
			}
		}
		
		// start receiving next one
		recv();
		
	}
	else
	{
		// error!
		//cout << "read error\n";
		disconnect();
		if ( _pConnectOperation )
		{
			_pConnectOperation->beginChange();
				_pConnectOperation->setStatus( Operation::Failed );
				_pConnectOperation->setStatusDescription( err.message() );
			_pConnectOperation->endChange();
			_pConnectOperation.reset();
		}
	}
}

// ============================================================================
// Incoming message
void Hub::onIncomingMessage( const string& msg )
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
		
		/* simple way of splitting - fails o some special chars
		stringstream ss( msg );
		ss >> command;
		while( ss.good() )
		{
			string param;
			ss >> param;
			params.push_back( param );
		}
		*/
		
		// manual method - split by spaces
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
// On incoming chat message.
void Hub::onIncomingChat( const string& msg )
{
	signalChatMessage( msg );
}

// ============================================================================
// On incoming command
void Hub::onIncomingCommand( const string& command, const list<string>& params )
{
	map<string,HubCommandHandler>::iterator hit = _handlers.find( command );
	if ( hit == _handlers.end() )
	{
		// TODO error
		cerr << "Unsupported hub command "<< command << endl;
		BOOST_FOREACH( string p, params )
		{
			cerr << " * " << p << endl;
		}
		return;
	}
	
	HubCommandHandler handler = hit->second;
	
	try
	{
		handler( this, params );
	}
	catch( const std::exception& e )
	{
		systemMessage( str( format("Error handling command '%1%' : %2%") % command % e.what() ) );
	}
}

// ============================================================================
// Calculate key
string Hub::calculateKey( const string& lock )
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
string Hub::escape( const string& str )
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
void Hub::systemMessage( const string& msg )
{
	signalSystemMessage( msg );
}

// ============================================================================
// $Lock handler
void Hub::commandLock( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		const string& lock = params.front();
		
		string key = calculateKey( lock );
		sendCommand( "$Supports", assign::list_of("NoGetINFO")("NoHello") ); // advertise other extesions here
		sendCommand( "$Key", assign::list_of(key) );
		sendCommand( "$ValidateNick", assign::list_of( escape(_pParent->settings().nick )) );
		// TODO $Supports
		
		// update progress
		if ( _pConnectOperation )
		{
			_pConnectOperation->setProgress(75);
		}
	}
	else
	{
		throw ProtocolException("Hub sent $Lock without params");
	}
}

// ============================================================================
// $HubName handler
void Hub::commandHubName( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		_hubName = params.front();
		
		signalNameChnged( _hubName );
	}
	else
	{
		throw ProtocolException("Hub sent $HubName without the name");
	}
}

// ============================================================================
// ValidateDenide
void Hub::commandValidateDenide( const list<string>& params )
{
	systemMessage( "Nick rejected" );
	if ( _pConnectOperation )
	{
		_pConnectOperation->beginChange();
			_pConnectOperation->setStatusDescription("Nick rejected");
			_pConnectOperation->setStatus(Operation::Failed);
		_pConnectOperation->endChange();
		_pConnectOperation.reset();
	}
	
	disconnect();
}

void Hub::commandGetPass( const list<string>& params )
{
	// TODO
}

void Hub::commandLogedIn( const list<string>& params )
{
	// TODO
}

void Hub::commandBadPass( const list<string>& params )
{
	// TODO
}

// ============================================================================
// on hello
void Hub::commandHello( const list<string>& params )
{
	// yuppi!
	
	if ( _pConnectOperation )
	{
		_pConnectOperation->beginChange();
			_pConnectOperation->setStatusDescription("Connected");
			_pConnectOperation->setStatus(Operation::Completed );
			_pConnectOperation->setProgress(100);
		_pConnectOperation->endChange();
		_pConnectOperation.reset();
	}
	systemMessage( str( format("Logged in to hub %1%") % _hubName ) );
	
	sendCommand( "$Version", assign::list_of("1,009") ); // for compatibility only
	send( "$GetNickList" );
	sendMyINFO();
	
	setState( Connected );
}

// ============================================================================
// send $MyINFO
void Hub::sendMyINFO()
{
	assert( _pParent );
	
	string connection  = escape( _pParent->settings().connection );
	string email       = escape( _pParent->settings().email );
	string nick        = escape( _pParent->settings().nick );
	string description = escape( _pParent->settings().description );
	
	uint64_t sharesize = 15372667328; // TODO get from somwhere
	
	uint8_t status = 1; // fixed value for now
	
	// create tag
	string version = "0.707"; // TODO get from some version.h
	string mode    = _pParent->settings().tcpPort > 0 ? "A" : "P"; // TODO add SOCKS5
	string hubs    = "1/0/0"; // TODO get actual data from client
	string slots   = str(format("%d") % _pParent->settings().uploadSlots );// TODO substract used slots
	
	string tag     = str(format("<++V:%1%,M:%2%,H:%3%,S:%4%>") % version % mode % hubs % slots );
	
	// construct message
	string param2 = str(format("%1%%2%$") % description % tag );
	string param3 = str(format("$%s%c$%s$%d$") % connection % status % email % sharesize );
	
	sendCommand( "$MyINFO", assign::list_of( string("$ALL") )( nick )( param2 )( param3 ) );
	
}

// ============================================================================
// MyInfo
void Hub::commandMyINFO( const list<string>& params )
{
	lock_guard<mutex> guard( _usersMutex );
	
	UserInfo info;
	
	try
	{
		info.parseMyINFO( params );
	}
	catch( const std::exception& e )
	{
		// is info acceptable regardless of error?
		if ( info.nick().length() == 0 )
		{
			throw;
		}
		cerr << "Recoverable error when parsing user '"<< info.nick() << "' params: " << e.what() << endl;
	}
	
	UserMap::iterator uit = _users.find( info.nick() );
	
	// new user?
	if ( uit == _users.end() )
	{
		_users[ info.nick() ] = info ;
		signalUserAdded( info );
	}
	// modified exisintg user
	else
	{
		uit->second = info; // TODO check if it works
		signalUserModified( info );
	}
}

// ============================================================================
// Quit
void Hub::commandQuit( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		lock_guard<mutex> guard( _usersMutex );

		string nick = params.front();
		
		_users.erase( _users.find( nick ) );
		signalUserRemoved( nick );
	}
	else
	{
		throw ProtocolException("Hub sent $Quit without params");
	}
}

// ============================================================================
// Force Move - kick
void Hub::commandForceMove( const list<string>& params )
{
	systemMessage( "HUB kicks us" );
	disconnect();
}

// ============================================================================
// get users
list<UserInfo> Hub::getUsers()
{
	lock_guard<mutex> guard( _usersMutex );
	
	list<UserInfo> result;
	
	for( UserMap::const_iterator it = _users.begin(); it != _users.end(); ++it )
	{
		result.push_back( it->second );
	}

	return result;
}

// ============================================================================
// Hub topic
void Hub::commandHubTopic( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		_hubTopic = params.front();
		signalTopicChnged( _hubTopic );
	}
	else
	{
		throw ProtocolException("Hub sent HubTopic without params");
	}
}

} // namespace
