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
#ifndef RUFUSDCCONNECTION_H
#define RUFUSDCCONNECTION_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/function.hpp>

namespace RufusDc
{

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class Client;


/**
* @brief Base class for connection hanlers.
* Base class for Direct anf Hub connection handlers. PErforms basic tasks, state minotoring and
* protocol decoding.
* All connections are created by client using appropriate factory method.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Connection
{
	
public:
	/// Connection state
	enum State
	{
		Disconnected,  ///< Disconnected, or failed to connect
		Connecting,    ///< Attepting to connect
		Connected,     ///< Connected and handhsake successfull
	};
	
	virtual ~Connection();
	
	/**
	* @brief Connects to hub.
	* Does nothing if hub already connected.
	* @return Potiner to operation object, whichcan be used to monitor connection status.
	*/
	virtual void connect();
	
	/**
	* @brief Disconnects.
	*/
	virtual void disconnect();
	
	/**
	* @brief Returns connection state
	* @return conection state
	*/
	State state() const { return _state; }
	
	/**
	 * @brief Returns locsal IP as string.
	 * Conection must be in 'Connected' state to make it work.
	 * @return local ip, as string
	 */
	string localIp() const;
	
	/**
	* @brief Sends command
	* @param cmd command (starting with $)
	* @param params command parameters
	*/
	void sendCommand( const string& cmd, const list<string>& params );
	

	// signals
	
	boost::signal< void (int) > signalStateChanged;             ///< Conection state changed
	boost::signal< void (const string&) > signalSystemMessage;  ///< system message

protected:
	
	/**
	* @brief Creates hub connection
	* @param address adress
	*/
	Connection( const string& address );
	
	// state variables
	
	State  _state;      ///< Connection state
	
	/**
	* @brief Changes state
	* This emits signalStateChanged
	* @param state new state.
	*/
	virtual void setState( State state );

	/// Hub's address. Never changes
	string _address;
	
	/// Resolver used to resolve peer address
	shared_ptr<tcp::resolver> _pResolver;
	
	/// Connection
	shared_ptr<tcp::socket> _pSocket;
	
	/// Buffer for incoming message
	asio::streambuf _inBuffer;
	
	/// Intermediate buffer, used to assemble command (async_read_until sucks!)
	string _command;

	/**
	* @brief Called when message is received.
	* Recognizes message, and calls onIcomingCommand or onIncomingChat. 
	* @param  msg recieived message
	*/
	void onIncomingMessage( const string& msg );
	
	/**
	* @brief Called on incoming DC command.
	* @param command command text, always starts with '$'
	* @param params command params
	*/
	virtual void onIncomingCommand( const string& command, const list<string>& params );
	
	/**
	* @brief Called when non-commend message comes in.
	* @param msg chat message.
	*/
	virtual void onIncomingChat( const string& msg ) = 0;
	
	/**
	* @brief Receives messages.
	* Starts asynchronous operation, which calls onRecvCommand upon completion.
	*/
	void recvCommand();
	
	/**
	* @brief Returns io_service
	* @return io_servide used to communicate
	*/
	io_service& ioService();
	
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
	* @brief Send raw message to peer
	* @param msg message
	*/
	void send( const string& msg );
	
	/**
	* @brief Adds system message.
	* This emits signalSystemMessage
	* @param msg message.
	*/
	void systemMessage( const string& msg );
	
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
	* @param pBuf buffer with sent data
	*/
	void onSend( const system::error_code& err, shared_ptr<asio::streambuf> pBuf );

	/**
	* @brief Handles result of 'recvCommand' operation.
	* @param err status
	* @param size bytes received
	*/
	void onRecvCommand( const system::error_code& err, int size );
	
	// command handlers
	
	typedef boost::function< void (const list<string>&) > CommandHandler;
	
	/// Mapping between command names and their handlers.
	map< string, CommandHandler > _handlers;
	
	/// Flag indicating binary data transfer
	bool _dataTransfer;

};

}

#endif // RUFUSDCCONNECTION_H

// EOF


