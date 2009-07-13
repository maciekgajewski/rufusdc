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

#include <boost/tokenizer.hpp>
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
	_status = 1;
	_sharesize = 0;

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
		if( description[ description.length()-1 ] == '$' ) // check last character
		{
			_description.assign( description.data(), description.length()-1 ); // copy all but last
		}
		else
		{
			throw ProtocolException(str(format("Cant parse user's description: %1%")%description));
		}
		
		// others
		try
		{
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep("$", "", boost::keep_empty_tokens);
	
			tokenizer tokens( other, sep );
	
			
			int index = 0;
			for( tokenizer::iterator it = tokens.begin(); it !=  tokens.end(); ++it, ++index )
			{
				switch( index )
				{
					case 1:
					{
						string conn_stat = *it;
						_status = conn_stat[ conn_stat.length() - 1 ]; // use last char
						_connection.assign( conn_stat.data(), conn_stat.length()-1 ); // copy all but last
						break;
					}
					
					case 2:
					{
						_email = *it;
						break;
					}
					
					case 3:
					{
						_sharesize = boost::lexical_cast<uint64_t>( *it );
						break;
					}
					
					default:
						;// nothing, ignore
				}
			}
		}
		catch(const std::exception& e)
		{
			throw ProtocolException(str(format("Cant parse user's parameters %1%: %2%")%other%e.what()));
		}
	}
	else
	{
		throw ProtocolException("Hub sent $MyINFO with too few params");
	}
}

}
