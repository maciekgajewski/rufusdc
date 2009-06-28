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
#ifndef RUFUSDCDOWNLOADMAP_H
#define RUFUSDCDOWNLOADMAP_H

#include <string>
#include <list>

namespace RufusDc
{

using namespace std;

/**
* Map of downloaded areas in file.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DownloadMap
{
public:
	DownloadMap();
	~DownloadMap();
	
	/// Initializes empty map
	///@param size file size
	void initialize( uint64_t size );
	
	/// Returns map as string. Used to dump state into file
	string toString() const;
	
	///Reads map from string
	void fromString(const string& str );
	
	// operations
	
	/// Marks region as downloaded
	///@param start start of region
	///@param lenght of region
	void markAsDownloaded( uint64_t start, uint64_t len );
	
	/// Returns number of donwloaded bytes
	uint64_t downloaded() const;
	
	/// Returns number of remaining bytes
	uint64_t remaining() const;
	
	/// Finds first free (non-downloaded) block
	///@param start - out: start of the block
	///@param len - out: lenght of the block
	///@return \b true if any free block was found
	bool findFirstFree( uint64_t& start, uint64_t& len );
	
	/// Finds first free (non-downloaded) block not msaller than given size
	///@param minSize - min size of the block
	///@param start - out param - start of the block
	///@param len - out: lenght of the block
	///@return \b true if any free block was found
	bool findFirstBig( uint64_t minSize, uint64_t& start, uint64_t& len );
	
private:

	/// Region type
	struct Region
	{
		Region( uint64_t s, uint64_t l ):start(s),len(l){}
		uint64_t start; ///< start
		uint64_t len;   ///< length
	};

	/// Counts downloaded bytes
	void countDownloaded();

	/// Actual map. Holds downloaded blocks. Key holds startm value lenght of the block.
	list<Region> _map;
	
	/// File size
	uint64_t _size;
	
	/// Cached number of downloaded bytes
	uint64_t _downloaded;
	

};

}

#endif // RUFUSDCDOWNLOADMAP_H

// EOF


