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
QString sizeToString( int64_t size )
{
	if ( size < 0 ) return QString::number( size ); // for goofy users
	
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

// ============================================================================
// speed to string
QString speedToString( double speed )
{
	QString suffix = "B"; // use KiB instead?
	double value = speed;
	
	if ( value  > 1000 )
	{
		suffix = "KB";
		value /= 1024;
	}
	
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
	
	QString result = QString("%1 %2/s").arg( value, 0, 'f', 2 ).arg( suffix );
	
	return result;
}

QString timeToString( int seconds )
{
	if ( seconds < 60 )
	{
		return QString("%1s").arg( seconds );
	}
	
	int minutes = seconds / 60;
	seconds = seconds % 60;
	
	if ( minutes < 60 )
	{
		return QString("%1m%2s").arg(minutes).arg(seconds);
	}
	
	int hours = minutes / 60;
	minutes = minutes % 60;
	
	return QString("%1h%2m%3s").arg(hours).arg(minutes).arg(seconds);
}

} // namespace