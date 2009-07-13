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
#ifndef RUFUSDCHUBMANAGER_H
#define RUFUSDCHUBMANAGER_H

// std
#include <string>

//rufusdc
#include "error.h"
#include "hub.h"
#include "singleton.h"

namespace RufusDc
{

// forwards
class DirectConnection;

/**
* Hub manager - manages list of currently connected hubs.
* 
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class HubManager : public Singleton< HubManager >
{
public:
    
	/// Handler function called when conection request ends
	typedef boost::function< void ( const Error&, const shared_ptr<DirectConnection>&) > ConnectionHandler;

    /// Constructor
    HubManager();
    
    /// Destructor
    virtual ~HubManager();
    
	/**
	* @brief Returns hub with specified address
	* @param hubAddr hub address
	* @return Pointer to hub object
	*/
	Hub::ptr getHub( const string& addr );
	
	/**
	 * @brief Requersts direct connection to another user.
	 * This method can yield three kinds of results:
	 *  - connection is successfull, handler is colled with connnection pointer
	 *  - connection couldn't be created, handler is called with error code
	 *  - hub is not connected or there is no such user - exception is thrown
	 * @param hub  hub to which owner of file is connected
	 * @param nick user's nick
	 * @param handler handler colled after connection
	 * @exception std::logic_error when hub is not connected, or there is no such user.
	 * @todo move to connection manager
	 */
	void requestConnection
		( const string& hub
		, const string& nick
		, const ConnectionHandler& handler
		);
		
	
private:

	/// Map of hubs, by address
	typedef map< string, Hub::ptr > HubMap;
	
	/// list of hubs to which the client is connected
	HubMap _hubs;
	
	/// Converts address to canonical form
	static std::string makeCanonical( const std::string& addr );
};

}

#endif // RUFUSDCHUBMANAGER_H

// EOF


