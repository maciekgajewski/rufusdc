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

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signal.hpp>

#include "operation.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

namespace RufusDc
{

class AdcMessage;
class Client;

/**
* @brief DC hub connection.
* This object represent DC hub connection.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Hub
{
	friend class Client;
	
	public:
	
		/// Hub connection state
		enum State
		{
			Disconected,   ///< Disconnected
			Connected,     ///< Connected, but not logged in
			LoggedIn,      ///< Logged in
		};
	
		virtual ~Hub();
		
		/**
		 * @brief Connects to hub.
		 * Does nothing if hub already connected.
		 * @return Potiner to operation object, whichcan be used to monitor connection status.
		 */
		shared_ptr<Operation> connect();
		
		/**
		 * Disconnects.
		 */
		void disconnect();
		
		// signals
		
		boost::signal< void (const string&) > signalChatMessage;
		boost::signal< void (const string&) > signalSystemMessage;
		
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
		 * @brief Returns hub address
		 * @return Hub addres, as sent in constructor
		 */
		string address() const { return _address; }
	
	private:
		
		/**
		 * @brief Creates hub connection, using specified io service for comunication.
		 * @param pClient parent
		 * @param address hub's adress
		 */
		explicit Hub( Client* pClient, const string& address );
		
		/// Parses address into tcp query
		///@exception invalid_argument when address can't be parsed
		static tcp::resolver::query parseAddress( const string& address ) throw ( std::invalid_argument );
		
		/**
		 * @brief Calculates client's key.
		 * @param lock lock used to calculate key
		 * @return calculated key
		 */
		static string calculateKey( const string& lock );
		
		/**
		 * @brief Escapes string
		 * Converts vorbodden characters int oappropriate escape sequences.
		 * @param str raw string
		 * @return escaped string
		 */
		static string escape( const string& str );
		
		/**
		 * @brief Send raw message to hub.
		 * @param msg message
		 */
		void send( const string& msg );
		
		/**
		 * @brief Sends command
		 * @param cmd command (starting with $)
		 * @param params command parameters
		 */
		void sendCommand( const string& cmd, const list<string>& params );
		
		/**
		 * @brief Sends MyINFO message to hub
		 * All data needed to construct the message is get from Client
		 */
		void sendMyINFO();
		
		/**
		 * @brief Receives messages.
		 * Starts asynchronous operation, which calls onRecv upon completion.
		 */
		void recv();
		
		/**
		 * @brief Returns io_service
		 * @return io_servide used to commubicate
		 */
		io_service& ioService();
		
		/**
		 * @brief Called when message is received.
		 * Recognizes message, and calls onIcomingCommand or onIncomingChat. 
		 * @param  msg recieived message
		 */
		void onIncomingMessage( const string& msg );
		
		/**
		 * @brief Called when chat comes in.
		 * @param msg chat message.
		 */
		void onIncomingChat( const string& msg );
		
		/**
		 * @brief Called on incoming HUB command.
		 * @param command command text, always starts with '$'
		 * @param params command params
		 */
		void onIncomingCommand( const string& command, const list<string>& params );
		
		/**
		 * @brief Adds system message.
		 * This emits signalSystemMessage
		 * @param msg message.
		 */
		void systemMessage( const string& msg );

		/// Parent object
		Client* _pParent;
		
		/// Hub's address. Never changes
		string _address;
		
		/// Resolver used to resolve hub address
		shared_ptr<tcp::resolver> _pResolver;
		
		/// Connection
		shared_ptr<tcp::socket> _pSocket;
		
		/// Buffer for incoming message
		asio::streambuf _inBuffer;
		
		/// Intermediate buffer, used to assemble command (async_read_until sucks!)
		string _command;

		/// Buffer for outgoing message
		asio::streambuf _outBuffer;
		
		// state
		
		string _hubName;	///< Hub name, as send by hub.
		State  _state;      ///< Hub connection state
		
		// operations
		
		shared_ptr<Operation> _pConnectOperation;
		
		// async handles
		
		
		/**
		 * @brief Handles address resolution.
		 * @param err status code
		 * @param endpoint_iterator resolved adrress
		 */
		void onResolve( const system::error_code& err, tcp::resolver::iterator endpoint_iterator);
		
		/**
		 * @brief Hanldes connection to hub
		 * @param err status
		 */
		void onConnect( const system::error_code& err );

		/**
		 * @brief Handles result of 'send' operation.
		 * @param err status
		 */
		void onSend( const system::error_code& err );

		/**
		 * @brief Handles result of 'recv' operation.
		 * @param err status
		 * @param size bytes received
		 */
		void onRecv( const system::error_code& err, int size );
		
		// command handlers
		
		typedef boost::function< void (Hub*, const list<string>&) > HubCommandHandler;
		
		/// Mapping between command names and their handlers.
		static map< string, HubCommandHandler > _handlers;
		
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
};

}

#endif // RUFUSDCHUB_H

// EOF


