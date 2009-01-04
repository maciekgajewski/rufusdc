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
#ifndef RUFUSDCFILELIST_H
#define RUFUSDCFILELIST_H

#include <string>
#include <vector>

namespace RufusDc
{

using namespace std;

/**
* @brief Another user's file list.
* This class represents another user's file list. Currently it storex raw XML data,
* maybe some XML parsing will be added in the future.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileList
{

public:
	FileList();
	~FileList();
	
	/// Initilizes list form bz2-compressed data.
	void fromBz2Data( vector<char>& data );

	/// Returns XML data
	const string& xml(); 

private:

	string _xml;

};

}

#endif // RUFUSDCFILELIST_H

// EOF


