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
#ifndef RUFUSDCCLIENT_H
#define RUFUSDCCLIENT_H

// std & boost
#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals.hpp>
#include <boost/thread/mutex.hpp>

// local
#include "downloadrequest.h"
#include "listener.h"

namespace RufusDc
{
using namespace boost;
using namespace std;
using namespace asio;

class Hub;
class Operation;

/**
* @brief Main DC client object
* Represents DC client. The client contains list of hubs and operations.
* All operation are performed asycnhronousely. Call the run() method perriodically to 
* update status of asynchronous operations.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Client
{
public:

	/// Client settings. Required for client operation
	struct Settings
	{
		Settings(); ///< Initializes with defaults
		
		int     maxUpload;    ///< Max upload speed [B/s]. negative for infinite
		int     maxDownload;  ///< Max download speed [B/s]. negative for inifiite
		string  nick;         ///< User nick (TODO should be per-hub)
		string  description;  ///< User description
		string  email;        ///< User email
		string  connection;   ///< Connection type
		int     uploadSlots;  ///< Upload slots
		int     tcpPort;      ///< TCP port for active connection. Negative for none
		int     udpPort;      ///< UDP port for active connection. Negative for none
	};

	Client();
	~Client();
	
	/**
		* @brief Updates status.
		* Call this method periodically to get client going.
		*/
	void run();
	
	/**
		* @brief Connects to HUB
		* @param hubAddr hub address
		*/
	void connectToHub( const string& hubAddr );
	
	/**
		* @brief Returns hub with specified address
		* @param hubAddr hub address
		* @return Pointer to hub object
		*/
	shared_ptr<Hub> getHub( const string& addr );
	
	/**
		* @brief Sets client settings.
		* @param settings new settings
		*/
	void setSettings( const Settings& settings ) { _settings = settings; }
	
	/**
		* @brief Returns settings
		* @return settings
		*/
	const Settings& settings() const { return _settings; }
	
	/**
		* @brief Returns client's IO service
		* @return reference to io service
		*/
	io_service& ioService() { return _service; }
	
	/**
	 * @brief Requests transfer
	 * @param hub     hub to which source is conected
	 * @param request request description
	 */
	void requestTransfer( Hub* pHub, const shared_ptr<DownloadRequest>& pRequest );
	
	// signals
	
	boost::signal< void ( Hub*, const string&) > signalSystemMessage;
	boost::signal< void ( Hub*, const string&) > signalChatMessage;
	
private:

	/// IO service used to network communication
	io_service _service;
	
	/// listening TCP socket
	// TODO acceptor here

	/// Map of hubs, by address
	typedef map< string, shared_ptr<Hub> > HubMap;
	
	/// list of hubs to which the client is connected
	HubMap _hubs;
	
	/// Mutex guarding _hubs
	boost::mutex _hubsMutex;
	
	/// Client's settings
	Settings _settings;
	
	/// TCP Listener
	Listener _tcpListener;
	
	/// Receives all hubs chat messages
	void onHubChatMessage( Hub* pHub, const string& msg );
	
	/// Receives all hubs sys messages
	void onHubSystemMessage( Hub* pHub, const string& msg );
	
	/// Boost slot, called when file list is downloaded succesfully.
	void fileListReceived( vector<char>& data, DownloadRequest* pRequest );
};

}

#endif // RUFUSDCCLIENT_H

// EOF

