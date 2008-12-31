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
#ifndef RUFUSDCADCMESSAGE_H
#define RUFUSDCADCMESSAGE_H

#include <string>
#include <list>

using namespace std;

namespace RufusDc
{

/**
* @brief ADC message
* Use AdcMessage to construct AdcMessage
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class AdcMessage
{

	public:
		AdcMessage();
		/**
		 * @brief Creates initialized message
		 * @param type msg type
		 * @param code msg code
		 */
		AdcMessage( char type, const std::string& code );
		~AdcMessage();
		
		/**
		 * @brief Returns assembled message.
		 * @param out stream to write message into
		 * @exception invalid_argument when message not initialized.
		 */
		void encode( ostream& out ) const;
		
		/**
		 * @brief Decodes message from stream.
		 * @param in stream to read message from
		 */
		void decode( istream& in );
		
		/**
		 * @brief Sets message type.
		 * Type is one of: B C D E F H I U
		 * @param type message type
		 */
		void setType( char type );
		
		/**
		 * @brief Sets message code.
		 * Code is three-letter, uppercase message code.
		 * @param code message code
		 */
		void setCode( const string& code );
		
		/**
		 * @brief Adds param to message.
		 * Param is utf8 string.
		 * @param param message param
		 */
		void addParam( const string& param );
		
		/**
		 * @breif Adds named param
		 * @param name param name
		 * @param value param value
		 */
		void addParam( const string& name, const string& value );
		
	private:
	
		/// Converts reserverd chars to escaped chars in the string
		static string escape( const string& str );
		
		/// Converts escape sequences back to appropriate chars
		static string unescape( const string& str );
	
		char    _msgType;   ///< Message type
		string  _msgCode;   ///< Three-letter message code
		
		/// message params
		list<string> _params;

};

}

#endif // RUFUSDCADCMESSAGE_H

// EOF


