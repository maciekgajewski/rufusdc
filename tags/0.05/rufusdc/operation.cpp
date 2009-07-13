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

#include <iostream> // debug!

#include "operation.h"

namespace RufusDc
{

Operation::Operation( const string& description )
	: _description( description )
	, _changed( false )
	, _bulkChange( 0 )
{
	_progress = 0;
	_status   = Running;
}


Operation::~Operation()
{
	//std::cout << "~Operation()\n";
}

// ============================================================================
// beginChange
void Operation::beginChange()
{
	_bulkChange++;
}

// ============================================================================
// End change
void Operation::endChange()
{
	_bulkChange--;
	if ( _changed )
	{
		signalChanged();
		_changed = false;
	}
}

// ============================================================================
// set status
void Operation::setStatus( Status s )
{
	if ( s != _status )
	{
		_status = s;
		changed();
	}
}

// ============================================================================
// set description
void Operation::setStatusDescription( const string& sd  )
{
	if ( _statusDescription != sd )
	{
		_statusDescription = sd;
		changed();
	}
}

// ============================================================================
// set progress
void Operation::setProgress( int p )
{
	if ( _progress != p )
	{
		_progress = p;
		changed();
	}
}

// ============================================================================
// change
void Operation::changed()
{
	if ( _bulkChange > 0)
	{
		_changed = true; // signal will be called in endChange()
	}
	else
	{
		// call now
		signalChanged();
	}
}


}
