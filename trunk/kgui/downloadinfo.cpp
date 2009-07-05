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

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/QueueItem.h>

// local
#include "downloadinfo.h"

namespace KRufusDc {

// ============================================================================
// Constructor
DownloadInfo::DownloadInfo()
{
}

// ============================================================================
// Destructor
DownloadInfo::~DownloadInfo()
{
}

// ============================================================================
// From queue item
void DownloadInfo::fromQueueItem( dcpp::QueueItem* pItem )
{
	Q_ASSERT( pItem );
	
	_TTH = pItem->getTTH().toBase32().c_str();
	_onlineUsers = pItem->countOnlineUsers();
	_path = QString::fromUtf8( pItem->getTargetFileName().c_str() );
	_size = pItem->getSize();
	_transferred = pItem->getDownloadedBytes();
	
	if ( pItem->isFinished() )
	{
		_state = FINISHED;
	}
	else if ( pItem->isRunning() )
	{
		_state = RUNNING;
	}
	else
	{
		_state = WAITING;
	}
}


}
