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
#ifndef RUFUSDCERROR_H
#define RUFUSDCERROR_H

#include <string>

namespace RufusDc
{

using namespace std;

/**
* Simple class containing operation status (success|failure) and messahe
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Error
{

public:
	
	/// Creates "no error" error
	Error(){}
	
	/// Creates error
	Error( const string& msg ): _msg(msg) {}
	
	/// Checks if operation failed
	operator bool() const { return ! _msg.empty(); }
	
	/// Returns error message
	string msg() const { return _msg; }

private:

	string _msg; ///< Error message

};

}

#endif // RUFUSDCERROR_H

// EOF


