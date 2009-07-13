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
#ifndef RUFUSDCUSERINFO_H
#define RUFUSDCUSERINFO_H

#include <list>
#include <string>

namespace RufusDc
{

using namespace std;


/**
* @brief Hub user information record
* Contains collected information on hub user.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class UserInfo
{

public:
	UserInfo();
	~UserInfo();

	/**
	 * @brief Parses MyINFO message
	 * Parses MyINFO messages and sets object accordingly
	 * @param params params sent alogn with MyINFO
	 * @exception ProtocolException if can't parse data.
	 */
	void parseMyINFO( const list<string>& params );

	void setNick( const string& theValue )
	{
		_nick = theValue;
	}

	string nick() const
	{
		return _nick;
	}

	void setDescription( const string& theValue )
	{
		_description = theValue;
	}

	string description() const
	{
		return _description;
	}

	void setConnection( const string& theValue )
	{
		_connection = theValue;
	}
	

	string connection() const
	{
		return _connection;
	}

	void setStatus( const uint8_t& theValue )
	{
		_status = theValue;
	}
	
	uint8_t status() const
	{
		return _status;
	}

	void setSharesize( const uint64_t& theValue )
	{
		_sharesize = theValue;
	}
	
	uint64_t sharesize() const
	{
		return _sharesize;
	}

	void setEmail( const string& theValue )
	{
		_email = theValue;
	}
	

	string email() const
	{
		return _email;
	}
	
	
private:

	string   _nick;
	string   _description;
	string   _connection;
	uint8_t  _status;
	uint64_t _sharesize;
	string   _email;
	

};

}

#endif // RUFUSDCUSERINFO_H

// EOF


