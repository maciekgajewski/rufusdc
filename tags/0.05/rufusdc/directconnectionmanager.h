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
#ifndef RUFUSDCDIRECTCONNECTIONMANAGER_H
#define RUFUSDCDIRECTCONNECTIONMANAGER_H

// boost
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

// local
#include "singleton.h"

namespace RufusDc
{
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class Client;
class ConnectionRequest;
class DirectConnection;

/**
* DirectConnectionManager owns and manages all direct connections.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DirectConnectionManager : public Singleton< DirectConnectionManager >
{

public:
	DirectConnectionManager( Client* pClient );
	~DirectConnectionManager();

	/**
	 * @brief Adds download request to the queue
	 * @param pRequest request object
	 */
	void addRequest( const shared_ptr<ConnectionRequest>& pRequest );
	
	/// Returns parent (The Client)
	Client* parent() { return _pParent; }
	
	/// Adds new conenction to the manager
	void newConnection( const shared_ptr<DirectConnection>& pConnection );
	
	/// Message from conection - state changed
	void connectionStateChanged( DirectConnection* pConn, int state );
	
private:
	
	/// Parent object
	Client* _pParent;
	
	/// Conenctions waiting for dispatching
	list< shared_ptr<DirectConnection> > _connections;
	
	/// Queue of requests
	list< shared_ptr<ConnectionRequest> > _requests;
	
	/// Timout timer
	boost::asio::deadline_timer _timer;

	/// Removes closed connections from list
	void removeClosedConnections();
	
	/// Removes timeouted requests from queue
	void removeExpiredRequests();
	
	/// Timer used to periodically remove timeout requests
	void onTimer();
	
};

}

#endif // RUFUSDCDIRECTCONNECTIONMANAGER_H

// EOF


