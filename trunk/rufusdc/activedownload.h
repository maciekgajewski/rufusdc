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

// boost
#include <boost/signals.hpp>

// local
#include "connection.h"

namespace RufusDc
{

using namespace boost;

class DownloadRequest;

/**
* @brief Incoming conection from anther client.
* This class doesn't use connect() method, but gets connected socket in constructor.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ActiveDownload : public Connection
{

public:
	ActiveDownload( Client* pClient, shared_ptr<tcp::socket> pSocket );
	virtual ~ActiveDownload();
	
	/// Starts communication
	void start();
	
	// signals
	
	/// Signla used to request DownloadRequest obect associated with nick
	boost::signal< void ( const string&, shared_ptr<DownloadRequest>& ) > signalRequest;
	
	
private:

	/**
	 * @brief Handles incoming chat messages.
	 * @tofo how to handle chat messages on downlod client-client connection?
	 * @param msg 
	 */
	virtual void onIncomingChat( const string& msg );
	
	
	/**
	 * @brief Called on incoming binary data
	 * @param buffer buffer containing the data
	 * @param size numer of bytes recived
	 */
	virtual void onIncomingData( vector<char>& buffer );
	
	// command handlers
	void commandMyNick( const list<string>& params );
	void commandLock(  const list<string>& params );
	void commandDirection(  const list<string>& params );
	void commandKey(  const list<string>& params );
	void commandFileLength(  const list<string>& params ); // obsolete
	void commandADCSND( const list<string>& params ); 
	
	// state
	
	string   _lock;         ///< Lock received from peer
	uint64_t _fileLength;   ///< Downloaded file length
	uint64_t _recivedBytes; ///< Bytes recived so far
	
	/// Request
	shared_ptr<DownloadRequest> _pRequest;
	

};

}

#endif // RUFUSDCACTIVEDOWNLOAD_H

// EOF


