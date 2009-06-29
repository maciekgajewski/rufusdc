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
#include <stdexcept>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "downloadmap.h"

namespace RufusDc
{

// ============================================================================
// Constructor
DownloadMap::DownloadMap()
{
	_size = 0;
	_downloaded = 0;
}

// ============================================================================
// Destructor
DownloadMap::~DownloadMap()
{
}

// ============================================================================
// Initilaize
void DownloadMap::initialize( uint64_t size )
{
	_size = size;
	_downloaded = 0;
	_map.clear();
}

// ============================================================================
// To string
string DownloadMap::toString() const
{
	stringstream ss;
	
	BOOST_FOREACH( const Region& r, _map )
	{
		ss << r.start << ":" << r.len << " ";
	}
	
	return ss.str();
}

// ============================================================================
// from string
void DownloadMap::fromString(const string& str )
{
	// TODO
}

// ============================================================================
// mark as downloaded
void DownloadMap::markAsDownloaded( uint64_t start, uint64_t len )
{
	if ( start + len > _size )
	{
		throw std::logic_error( "region outside the file");
	}

	// simplest case
	if ( _map.empty() )
	{
		_map.push_back( Region( start, len ) );
		_downloaded = len;
		return;
	}
	
	// find first ovelapping
	list<Region>::iterator firstOverlapping = _map.end();
	
	for( list<Region>::iterator it = _map.begin(); it != _map.end(); ++it )
	{
		// is above?
		if ( it->start > (start+len) )
		{
			// insert here, leave
			_map.insert( it, Region( start, len ) );
			_downloaded += len;
			return;
		}
		// overlaps?
		else if ( it->start < (start+len) && (it->start+it->len) > start )
		{
			firstOverlapping = it;
			break;
		}
	}
	
	// is any overlapping found?
	if ( firstOverlapping != _map.end() )
	{
		uint64_t end = max( start+len, firstOverlapping->start + firstOverlapping->len );
		firstOverlapping->start = min( firstOverlapping->start, start );
		
		list<Region>::iterator next = firstOverlapping;
		++next;
		
		while( next != _map.end() && next->start <= end )
		{
			end = max( end, next->start + next->len );
			next = _map.erase( next );
		}
		
		firstOverlapping->len = end - firstOverlapping->start;
		
		countDownloaded();
	}
	else
	{
		// no. append at the ned of the list
		_map.push_back( Region( start, len ) );
		_downloaded += len;
	}
}

// ============================================================================
// downloaded
uint64_t DownloadMap::downloaded() const
{
	return _downloaded;
}

// ============================================================================
// remaining
uint64_t DownloadMap::remaining() const
{
	return _size - _downloaded;
}

// ============================================================================
// first free
bool DownloadMap::findFirstFree( uint64_t& start, uint64_t& len )
{
	if ( _size == 0 )
	{
		throw std::logic_error("Download map uninitialized");
	}
	
	if ( _downloaded == _size )
	{
		return false;
	}
	
	if ( _map.empty() )
	{
		start = 0;
		len = _size;
		return true;
	}

	if ( _map.front().start != 0 )
	{
		start = 0;
		len = _map.front().start;
		return true;
	}

	list<Region>::iterator first = _map.begin();
	start = first->start + first->len;
	
	++first;
	if ( first == _map.end() )
	{
		len = _size - start;
	}
	else
	{
		len = first->start - start;
	}
	
	return true;
}

// ============================================================================
// first big
bool DownloadMap::findFirstBig( uint64_t minSize, uint64_t& start, uint64_t& len )
{
	// TODO
	return false;
}

// ============================================================================
// count downloaded
void DownloadMap::countDownloaded()
{
	_downloaded = 0;
	
	BOOST_FOREACH( const Region& r, _map )
	{
		_downloaded += r.len;
	}
}

}
