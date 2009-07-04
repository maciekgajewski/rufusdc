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
#ifndef KRUFUSDCUSERINFO_H
#define KRUFUSDCUSERINFO_H

// Qt
#include <QMetaType>

// dcpp
namespace dcpp
{
	class OnlineUser;
}

namespace KRufusDc
{

/**
* @brief Structure containing user information
* Used to transform user information across
* threads.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class UserInfo
{

public:
	UserInfo();
	UserInfo( const UserInfo& src );
	~UserInfo();
	
	/// Fills in user info with dcpp's user indentity
	void fromDcppIdentity( const dcpp::OnlineUser& user );
	
	void setNick( const QString& value )
	{
		_nick = value;
	}
	

	QString nick() const
	{
		return _nick;
	}

	void setDescription( const QString& value )
	{
		_description = value;
	}
	

	QString description() const
	{
		return _description;
	}

	void setConnection( const QString& value )
	{
		_connection = value;
	}
	

	QString connection() const
	{
		return _connection;
	}

	void setStatus( const uint8_t& value )
	{
		_status = value;
	}
	

	uint8_t status() const
	{
		return _status;
	}

	void setSharesize( const uint64_t& value )
	{
		_sharesize = value;
	}
	

	uint64_t sharesize() const
	{
		return _sharesize;
	}

	void setEmail( const QString& value )
	{
		_email = value;
	}
	

	QString email() const
	{
		return _email;
	}
private:

	QString  _nick;
	QString  _description;
	QString  _connection;
	uint8_t  _status;
	uint64_t _sharesize;
	QString  _email;
};

}

Q_DECLARE_METATYPE(KRufusDc::UserInfo);

#endif // KRUFUSDCUSERINFO_H

// EOF


