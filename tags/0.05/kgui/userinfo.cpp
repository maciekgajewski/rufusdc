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

#include <QTextCodec>

#include "userinfo.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
UserInfo::UserInfo()
{
}

// ============================================================================
// Destructor
UserInfo::~UserInfo()
{
}

// ============================================================================
// Converter
void UserInfo::convert( const RufusDc::UserInfo& info, QTextCodec* pCodec )
{
	_sharesize = info.sharesize();
	_status = info.status();
	
	if ( pCodec )
	{
		_nick        = pCodec->toUnicode( info.nick().c_str() );
		_description = pCodec->toUnicode( info.description().c_str() );
		_connection  = pCodec->toUnicode( info.connection().c_str() );
		_email       = pCodec->toUnicode( info.email().c_str() );
	}
	else
	{
		_nick        = info.nick().c_str();
		_description = info.description().c_str();
		_connection  = info.connection().c_str();
		_email       = info.email().c_str();
	}
}


}
