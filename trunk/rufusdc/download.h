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
#ifndef RUFUSDCDOWNLOAD_H
#define RUFUSDCDOWNLOAD_H

// std
#include <string>

// boost
#include <boost/shared_ptr.hpp>

namespace RufusDc
{
using namespace std;

class Client;

/**
* @brief File download
* Object of the calss represents file download.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Download
{
public:

	/// pointer type
	typedef boost::shared_ptr<Download> ptr;

	/// Stat of the download
	enum State
	{
		Idle,       ///< Idle. No transfer in progress
		Active,     ///< Active - data is transfered
		Completed,  ///< Completed successfully
		Failed      ///< Failed
	};

	virtual ~Download();
	
	/**
	 * @brief Sets original source of download
	 * @param hub  hub addr
	 * @param nick user's nick
	 */
	void setSource( const string& hub, const string& nick ) { _hub = hub; _nick = nick; }
	
	
	/**
	 * @brief Startys the download
	 */
	virtual void start() = 0;
	
	State state() const { return _state; }
	
protected:

	/// Constructor
	Download();
	
	/// Source hub address
	string _hub;
	
	/// source user nick
	string _nick;
	
	/// Download state
	State _state;
	
	/// Sets current state
	void setState( State s ) { _state = s; }

};

}

#endif // RUFUSDCDOWNLOAD_H

// EOF

