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
#ifndef RUFUSDCACTIVEDOWNLOAD_H
#define RUFUSDCACTIVEDOWNLOAD_H

// std
#include <set>

// boost
#include <boost/signals.hpp>
#include <boost/function.hpp>

// local
#include "error.h"

#include "connection.h"

namespace RufusDc
{

using namespace boost;
using namespace std;

class DirectConnectionManager;

/**
* @brief Direct connection to antoher DC client
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DirectConnection : public Connection
{

public:

	/// pointer type
	typedef boost::shared_ptr< DirectConnection > ptr;

	/// Transfer direction requested by peer
	enum Direction
	{
		UnknownDirection, ///< Unknown
		Download,         ///< Download (he wants something from us)
		Upload            ///< Upload (he is readu to provide us with file)
	};

	/// Incoming data handler type.
	/// First param is data itself
	/// Second param is offset when the data starts
	typedef boost::function< void ( vector<char>&, uint64_t ) > IncomingDataHandler;
	
	/// Hadler called upon operation completion
	typedef boost::function< void ( const Error& ) > CompletionHandler;

	/// Creates connected connection
	DirectConnection( DirectConnectionManager* pParent, shared_ptr<tcp::socket> pSocket );
	
	/// Destructor
	virtual ~DirectConnection();
	
	/// Starts communication
	void start();
	
	/// Disconnectes connection
	virtual void disconnect();
	
	/// Nick of connected user
	string nick() const { return _nick; }
	
	/// Direction requested by connected user
	Direction direction() const { return _direction; }
	
	/// Checks if client has specified feature
	bool hasFeature( const string& feat ) const;
	
	/// Downloads requested file from peer
	///@param fileName          name of file to download
	///@param offset            offset - where to start downloading
	///@param count             number of bytes to donwalod. 0 = infinitive
	///@param dataHandler       handler that should be called each time data is received
	///@param completionHandler handler that should be upon completion
	void downloadFile
		( const string& fileName
		, uint64_t offset
		, uint64_t count
		, const IncomingDataHandler& dataHandler
		, const CompletionHandler& completionHandler
		);
	
	/// Returns transfer speed - mean from beggign of transfer [b/s]
	double totalTransferSpeed() const;
	
	/// Returns current transfer speed [b/s]
	double currentTransferSpeed() const { return _currentTransferSpeed; }
	
private:

	/**
	 * @brief Handles incoming chat messages.
	 * @tofo how to handle chat messages on downlod client-client connection?
	 * @param msg 
	 */
	virtual void onIncomingChat( const string& msg );
	
	/**
	* @brief Receives binary data.
	* Switches into data receive mode, untile specified number of bytes is received.
	* @param size number of bytes to receive
	*/
	void recvData( uint64_t size );
	
	
	/**
	 * @brief Starts receiving binary data
	 * Actually starts transfer.
	 */
	void startReceiving();
	
	/**
	* @brief Changes state
	* Notifies manager
	* @param state new state.
	*/
	virtual void setState( State state );
	
	/// Handler used by startReceiving().
	void onDataReceived( const system::error_code& err, int size );
	
	// command handlers
	void commandMyNick( const list<string>& params );
	void commandLock(  const list<string>& params );
	void commandDirection(  const list<string>& params );
	void commandKey(  const list<string>& params );
	void commandFileLength(  const list<string>& params ); // obsolete
	void commandADCSND( const list<string>& params ); 
	void commandSupports( const list<string>& params );
	
	// state
	
	/// Parent manager
	DirectConnectionManager* _pManager;
	
	uint64_t  _fileLength;        ///< Downloaded file length
	uint64_t  _receivedBytes;     ///< Bytes received so far
	uint64_t  _currentFileOffset; ///< Offset from which data are currently received
	uint64_t  _bytesRequested;    ///< Number of bytes requested
	string    _nick;              ///< Remote user's nick
	Direction _direction;         ///< Direction requested by peer
	set<string> _features;        ///< Remote client capabilities (as announced by $Support)
	
	vector<char> _dataBuffer;     ///< Buffer for incoming data
	
	// transfer limit and speed
	
	int    _transferPool;         ///< Number of bytes the connection is allowed to receive
	bool   _transferLimited;      ///< If transfer is limited
	double _currentTransferSpeed; ///< Current transfer speed
	
	/// Total download time
	posix_time::time_duration _downloadTime;
	
	/// Temporary used to measure transfer speed
	posix_time::ptime _transferStart;
	
	
	/// Handler called when data comes in
	IncomingDataHandler _onIncomingData;
	
	/// Handler called when file downlopad is completed
	CompletionHandler _onDownloadCompleted;
	
};

}

#endif // RUFUSDCACTIVEDOWNLOAD_H

// EOF


