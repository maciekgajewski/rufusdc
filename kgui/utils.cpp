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

#include "utils.h"

namespace KRufusDc
{

// ============================================================================
// size to str
QString sizeToString( uint64_t size )
{
	QString suffix = "KB"; // use KiB instead?
	double value = size / double(1024);
	
	if ( value  > 1000 )
	{
		suffix = "MB";
		value /= 1024;
	}
	
	if ( value  > 1000 )
	{
		suffix = "GB";
		value /= 1024;
	}
	
	if ( value  > 1000 )
	{
		suffix = "TB";
		value /= 1024;
	}
	
	QString result = QString("%1 %2").arg( value, 0, 'f', 2 ).arg( suffix );
	
	return result;
}
	
} // namespace