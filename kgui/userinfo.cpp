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

// Qt
#include <QTextCodec>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/User.h>
#include <dcpp/Client.h>

// local
#include "userinfo.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
UserInfo::UserInfo()
{
}

// ============================================================================
// Copy Constructor
UserInfo::UserInfo( const UserInfo& src )
	: _nick(src._nick)
	, _description(src._description)
	, _connection(src._connection)
	, _status(src._status)
	, _sharesize(src._sharesize)
	, _email(src._email)
{
}

// ============================================================================
// Destructor
UserInfo::~UserInfo()
{
}

// ============================================================================
// converter from dcc data type
void UserInfo::fromDcppIdentity( const dcpp::OnlineUser& user )
{
	const dcpp::Identity& identity = user.getIdentity();
	const dcpp::Client& client = user.getClient();
	
	const char* encoding = client.getEncoding().c_str();
	
	QTextCodec* pCodec = QTextCodec::codecForName( encoding );
	
	// TODO
	_nick		= pCodec->toUnicode( identity.getNick().c_str() );
	_connection	= identity.getConnection().c_str();
	_status		= identity.getSID();
	_sharesize	= identity.getBytesShared();
	_email		= pCodec->toUnicode( identity.getEmail().c_str() );
	_description	= pCodec->toUnicode( identity.getDescription().c_str() );
	
}


}
