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
#ifndef RUFUSDCLISTENER_H
#define RUFUSDCLISTENER_H

// boost
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

// local
#include "directconnection.h"

namespace RufusDc
{
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class Client;
class ConnectionRequest;

/**
* @brief Listnes for connections from other clients.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Listener
{

public:
	Listener( Client* pClient );
	~Listener();
	
	/**
	 * @brief Sets TCP port to listen on.
	 * To change port at runtime, call stopListening(), setPort(), then startListening() again.
	 * @param port IP port
	 */
	void setPort( int port );
	
	/**
	 * @brief Retrns IP port on which listener is listening.
	 * @return IP port
	 */
	int port() const { return _port; }
	
	/**
	 * @brief Starts listening.
	 */
	void startListening();
	
	/**
	 * @brief Stops listening.
	 */
	void stopListening();
	
private:

	int _port; ///< TCP port
	
	/// Parent object
	Client* _pParent;
	
	tcp::acceptor _acceptor; ///< Accepts incoming connection
	
	/// Accept handler
	void onAccept( shared_ptr<tcp::socket> pSocket, system::error_code err );
	
	/// Request accept
	void accept();
	
};

}

#endif // RUFUSDCLISTENER_H

// EOF

