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
#ifndef RUFUSDCHUB_H
#define RUFUSDCHUB_H

#include <string>
#include <map>
#include <set>

#include <boost/thread/mutex.hpp>

#include "userinfo.h"
#include "connection.h"

namespace RufusDc
{

/**
* @brief DC hub connection.
* This object represent DC hub connection.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Hub : public Connection
{
	friend class Client;

public:

	virtual ~Hub();
	
	/**
	* @brief Disconnects from hub
	* Does nothing if hub already disconnected.
	*/
	virtual void disconnect();
	
	// signals
	
	boost::signal< void (const string&) > signalChatMessage;    ///< Chat message incoming
	
	boost::signal< void (const UserInfo&) > signalUserAdded;    ///< New user added to user list
	boost::signal< void (const UserInfo&) > signalUserModified; ///< User modified on user list
	boost::signal< void (const string&) >   signalUserRemoved;  ///< User remved from user list
	
	boost::signal< void (const string&) > signalNameChnged;     ///< Hub name changed
	boost::signal< void (const string&) > signalTopicChnged;    ///< Hub topic changed
	
	// state
	
	/**
	* @brief Returns hub state
	* @return Current hub state
	*/
	State state() const { return _state; }
	
	/**
	* @brief Returns hub name
	* @return Hub name, as sent by hub.
	*/
	string hubName() const { return _hubName; }
	
	/**
	* @brief Returns hub topic
	* @return Hub topic, as sent by hub.
	*/
	string hubTopic() const { return _hubName; }
	
	/**
	* @brief Returns hub address
	* @return Hub addres, as sent in constructor
	*/
	string address() const { return _address; }
	
	/**
	* @brief Returns list of currently connected users
	* @return List of users
	*/
	list<UserInfo> getUsers();
	
	/**
	* @brief Requests file list for specified user.
	* @param nick user's nick
	*/
	void requestFileList( const string& nick );

private:
	
	/**
	* @brief Creates hub connection, using specified io service for comunication.
	* @param pClient parent
	* @param address hub's adress
	*/
	Hub( Client* pClient, const string& address );
	
	/**
	* @brief Sends MyINFO message to hub
	* All data needed to construct the message is get from Client
	*/
	void sendMyINFO();
	
	/**
	* @brief Called when chat comes in.
	* @param msg chat message.
	*/
	virtual void onIncomingChat( const string& msg );
	
	// state
	
	string _hubName;	///< Hub name, as send by hub.
	string _hubTopic;   ///< Hub topic
	
	/// Hub features, as annunced by hub
	set<string> _hubFeatures;
	
	/// User database type
	typedef map<string, UserInfo > UserMap;
	
	/// Users database. nick is the key
	UserMap _users;
	
	/// Mutex guarding _users
	boost::mutex _usersMutex;
	
	// async handles
	
	
	// command handlers
	void commandLock( const list<string>& params );
	void commandHubName( const list<string>& params );
	void commandValidateDenide( const list<string>& params );
	void commandGetPass( const list<string>& params );
	void commandLogedIn( const list<string>& params );
	void commandBadPass( const list<string>& params );
	void commandHello( const list<string>& params );
	void commandMyINFO( const list<string>& params );
	void commandQuit( const list<string>& params );
	void commandForceMove( const list<string>& params );
	void commandHubTopic( const list<string>& params );
	void commandUserIP( const list<string>& params );
	void commandSupports( const list<string>& params );
	void commandConnectToMe( const list<string>& params );
};

}

#endif // RUFUSDCHUB_H

// EOF


