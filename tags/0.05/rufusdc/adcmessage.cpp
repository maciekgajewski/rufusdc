// Copyright (C) 2008 Maciek Gajewski <maciej.gajewski0@gmail.com>
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

#include <sstream>
#include <stdexcept>
#include <boost/foreach.hpp>

#include "adcmessage.h"

namespace RufusDc
{

// ============================================================================
// Constructor
AdcMessage::AdcMessage()
{
	_msgType = '\0';
}

AdcMessage::AdcMessage( char type, const std::string& code )
{
	setType( type );
	setCode( code );
}

// ============================================================================
// Constructor
AdcMessage::~AdcMessage()
{
}

// ============================================================================
// Assemble message
void AdcMessage::encode( ostream& out ) const
{
	if ( ! _msgType )
	{
		throw std::invalid_argument("Message type not initialized");
	}
	if ( _msgCode.length() != 3 )
	{
		throw std::invalid_argument("Message code not initialized");
	}
	
	out << _msgType << _msgCode;
	for( list<string>::const_iterator it = _params.begin(); it != _params.end(); ++it )
	{
		// add separator
		out << ' ';
		// param
		string escaped = escape( *it );
		out <<  escaped;
	}
}

// ============================================================================
// Decode
void AdcMessage::decode( istream& in )
{
	char c;
	in >> c;
	setType(c);
	
	string code;
	in >> code;
	setCode( code );
	
	while( in.eof() )
	{
		string param;
		in >> param;
		string unescaped = unescape( param );
		_params.push_back( unescaped );
	}
}

// ============================================================================
// Set type
void AdcMessage::setType( char type )
{
	switch( type )
	{
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'H':
		case 'I':
		case 'U':
			_msgType = type;
			return;
	}
	
	throw std::invalid_argument("Invalid message type");
}

// ============================================================================
// Set Code
void AdcMessage::setCode( const string& code )
{
	if ( code.length() != 3 )
	{
		throw std::invalid_argument("Invalid message code");
	}
	_msgCode = code;
}

// ============================================================================
// Add simple param
void AdcMessage::addParam( const string& param )
{
	_params.push_back( param );
}

// ============================================================================
// Add named param
void AdcMessage::addParam( const string& name, const string& value )
{
	addParam( name + value );
}

// ============================================================================
// escape
string AdcMessage::escape( const string& str )
{
	string result;
	
	BOOST_FOREACH( const char c, str )
	{
		switch( c )
		{
			case ' ':
				result.append("\\s");
				break;
				
			case '\n':
			case '\r':
				result.append("\\n");
				break;
				
			case '\\':
				result.append("\\\\");
				break;
				
			default:
				result.push_back(c);
		}
	}
	
	return result;
}

// ============================================================================
// unescape
string AdcMessage::unescape( const string& str )
{
	string result;
	
	for( int i = 0; i < str.length(); i++ )
	{
		char c = str[i];
		if ( c == '\\' && i < str.length() - 1 )
		{
			i++;
			c = str[i];
			char u ;
			switch( c )
			{
				case 's':
					u = ' ';
					break;
				case 'n':
					u = '\n';
					break;
				default:
					u = c;
			}
			
			result.push_back( u );
		}
		else
		{
			result.push_back( c );
		}
	}
	
	return result;
}


}
