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

#include <iostream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include "protocolexception.h"

#include "userinfo.h"

namespace RufusDc
{

using namespace boost;


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
// Parse
void UserInfo::parseMyINFO( const list<string>& params )
{
	if ( params.size() >= 4 )
	{
		// debug
		/*
		cerr << "My info: "<< params.size() << " params" << endl;
		BOOST_FOREACH( string p, params )
		{
			cerr << "  * " << p << endl;
		}
		*/
		list<string>::const_iterator it = params.begin();
		++it; // $ALL, not interesting (maybe check?)
		string nick        = *(it++);
		string description = *(it++);
		while( description[ description.length() -1 ] != '$' )
		{
			description += " ";
			description += *(it++);
		}
		string other       = *it;
		
		// extract data
		
		// nick is trivial
		_nick = nick;
		
		// description 
		smatch what;
		if( regex_match( description, what, regex("(.*)\\$") ) )
		{
			_description = what[1];
		}
		else
		{
			throw ProtocolException(str(format("Cant parse user's description: %1%")%description));
		}
		
		// others
		if( regex_match( other, what, regex("\\$(.+)(.)\\$(.*)\\$(\\d+)\\$") ) )
		{
			_connection    = what[1];
			_status        = string( what[2] )[0];
			_email         = what[3];
			
			
			_sharesize = boost::lexical_cast<uint64_t>( what[4] );
			
		}
		else
		{
			throw ProtocolException(str(format("Cant parse user's parameters: %1%")%other));
		}
	}
	else
	{
		throw ProtocolException("Hub sent $MyINFO with too few params");
	}
}

}
