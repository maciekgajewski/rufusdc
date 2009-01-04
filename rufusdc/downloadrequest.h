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
#ifndef RUFUSDCEXCPECTEDCONNECTION_H
#define RUFUSDCEXCPECTEDCONNECTION_H

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/signals.hpp>

namespace RufusDc
{
	using namespace std;
	using namespace boost;

/**
* @brief Expected incoming connection.
* Simple structure describing expected incoming connection and download request.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DownloadRequest
{
public:
	DownloadRequest();
	~DownloadRequest();

	// signals
	
	/// Emitted on transfer completion. Do not store pRequest.
	boost::signal< void ( vector<char>& data, DownloadRequest* pRequest ) > signalTransferCompleted;

	// Checks if expiry time passed
	bool isExpired();
	
	void setNick( const string& value )
	{
		_nick = value;
	}

	string nick() const
	{
		return _nick;
	}

	void setFile( const string& value )
	{
		_file = value;
	}
	

	string file() const
	{
		return _file;
	}

	void setExpiryTime( const posix_time::ptime& value )
	{
		_expiryTime = value;
	}
	

	posix_time::ptime expiryTime() const
	{
		return _expiryTime;
	}

	void setHub( const string& value )
	{
		_hub = value;
	}
	

	string hub() const
	{
		return _hub;
	}
	
	
private:
	string _nick;   ///< nick of user that should connect here
	string _file;   ///< path to file which should be downloaded
	string _hub;    ///< Hub to download from
	
	/// Time when the connection expires
	posix_time::ptime _expiryTime;

};

}

#endif // RUFUSDCEXCPECTEDCONNECTION_H

// EOF


