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

//std
#include <cassert>
#include <sstream>

// boost
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

// local
#include "protocolexception.h"
#include "client.h"

#include "hub.h"

namespace RufusDc
{

// ============================================================================
// Constructor
Hub::Hub( Client* pParent, const string& addr )
	: Connection( pParent, addr )
{
	// init handlers
	_handlers["$Lock"]           = boost::bind( &Hub::commandLock, this, _1 );
	_handlers["$HubName"]        = boost::bind( &Hub::commandHubName, this, _1 );
	_handlers["$ValidateDenide"] = boost::bind( &Hub::commandValidateDenide, this, _1 );
	_handlers["$GetPass"]        = boost::bind( &Hub::commandGetPass, this, _1 );
	_handlers["$LogedIn"]        = boost::bind( &Hub::commandLogedIn, this, _1 );
	_handlers["$BadPass"]        = boost::bind( &Hub::commandBadPass, this, _1 );
	_handlers["$Hello"]          = boost::bind( &Hub::commandHello, this, _1 );
	_handlers["$MyINFO"]         = boost::bind( &Hub::commandMyINFO, this, _1 );
	_handlers["$Quit"]           = boost::bind( &Hub::commandQuit, this, _1 );
	_handlers["$HubTopic"]       = boost::bind( &Hub::commandHubTopic, this, _1 );
	_handlers["$UserIP"]         = boost::bind( &Hub::commandUserIP, this, _1 );
	_handlers["$Supports"]       = boost::bind( &Hub::commandSupports, this, _1 );
}

// ============================================================================
// Destructor
Hub::~Hub()
{
}

// ============================================================================
// Disconect
void Hub::disconnect()
{
	lock_guard<mutex> guard( _usersMutex );
	_users.clear();

	Connection::disconnect();
}


// ============================================================================
// On incoming chat message.
void Hub::onIncomingChat( const string& msg )
{
	signalChatMessage( msg );
}

// ============================================================================
// $Lock handler
void Hub::commandLock( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		const string& lock = params.front();
		
		string key = calculateKey( lock );
		sendCommand( "$Supports", assign::list_of("NoGetINFO")("NoHello")("UserIP2") ); // advertise other extesions here
		sendCommand( "$Key", assign::list_of(key) );
		sendCommand( "$ValidateNick", assign::list_of( escape(_pParent->settings().nick )) );
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
		// DEBUG
		//cerr << "Recoverable error when parsing user '"<< info.nick() << "' params: " << e.what() << endl;
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
list<UserInfo> Hub::getUsers() const
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
// Has user
bool Hub::hasUser( const string& nick ) const
{
	lock_guard<mutex> guard( _usersMutex );
	
	UserMap::const_iterator it = _users.find( nick );
	
	return it != _users.end();
}

// ============================================================================
// Hub topic
void Hub::commandHubTopic( const list<string>& params )
{
	if ( params.size() > 0 )
	{
		_hubTopic = params.front();
		signalTopicChnged( _hubTopic );
		systemMessage( str(format("Hub topic is %1%") % _hubTopic ) );
	}
	else
	{
		throw ProtocolException("Hub sent HubTopic without params");
	}
}

// ============================================================================
// Request file list
void Hub::requestFileList( const string& nick )
{
	_pParent->downloadFileList( _address, nick );
}

// ============================================================================
// User IP
void Hub::commandUserIP( const list<string>& params )
{
	// nothing intresting here
}

// ============================================================================
// Supports
void Hub::commandSupports( const list<string>& params )
{
	BOOST_FOREACH( string param, params )
	{
		_hubFeatures.insert( param );
	}
}

// ============================================================================
// Connect to me
void Hub::commandConnectToMe( const list<string>& params )
{
	// TODO
}


} // namespace
