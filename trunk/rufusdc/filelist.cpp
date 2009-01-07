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

// bzlib
#include <bzlib.h>

// std
#include <iostream>
#include <stdexcept>

// boost
#include <boost/format.hpp>

#include "filelist.h"

namespace RufusDc
{

using namespace boost;

// ============================================================================
// Constructor
FileList::FileList()
{
}

// ============================================================================
// Destructor
FileList::~FileList()
{
}

// ============================================================================
// From bz2 data stream
void FileList::fromBz2Data( vector<char>& data )
{
	const int shift = 7; // first 7 bytes should be dropped. why?
	

	unsigned int inSize = data.size() - shift;
	char* inBuf = data.data() + shift;
	
	unsigned int outSize = inSize * 10; // rough estimate
	
	char* outBuf = new char[ outSize ];
	
	int res = BZ2_bzBuffToBuffDecompress
		( outBuf
		, &outSize
		, inBuf
		, inSize
		, 0 // no small, be fast!
		, 1 // some verbosity (0-4)
		);
	
	if ( res == BZ_OK )
	{
		cerr << "file list decompressed from " << inSize << " to " << outSize << " bytes" << endl;
		_xml.assign( outBuf, outSize );
		delete[] outBuf;
	}
	else
	{
		delete[] outBuf;
		
		throw std::logic_error( str(format("Error decompresing filelist.xml.bz2. code: %1%") % res ).c_str() );
	}
	
}


}
