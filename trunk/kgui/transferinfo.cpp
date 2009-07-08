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
#include <dcpp/Upload.h>
#include <dcpp/Download.h>
#include <dcpp/User.h>
#include <dcpp/ClientManager.h>



// local
#include "transferinfo.h"

namespace KRufusDc {

// ============================================================================
// Constructor
TransferInfo::TransferInfo()
{
}

// ============================================================================
// Destructor
TransferInfo::~TransferInfo()
{
}

// ============================================================================
// from upload
void TransferInfo::fromDcppUpload( dcpp::Upload* pUpload )
{
	Q_ASSERT( pUpload );

	_type = UPLOAD;
	fromDcppTransfer( pUpload );
}

// ============================================================================
// From download
void TransferInfo::fromDcppDownload( dcpp::Download* pDownload )
{
	Q_ASSERT( pDownload );
	
	_type = DOWNLOAD;
	fromDcppTransfer( pDownload );
}

// ============================================================================
// From transfer
void TransferInfo::fromDcppTransfer( dcpp::Transfer* pTransfer )
{
	
	_TTH = pTransfer->getTTH().toBase32().c_str();
	_path = pTransfer->getPath().c_str();
	_size = pTransfer->getSize();
	_transferred = pTransfer->getPos();
	_averageSpeed = pTransfer->getAverageSpeed();
	_CID = pTransfer->getUser()->getCID().toBase32().c_str();
	_secondsLeft = pTransfer->getSecondsLeft();

	// nick
	dcpp::StringList sl = dcpp::ClientManager::getInstance()->getNicks( pTransfer->getUser()->getCID() );
	if ( sl.size() > 0 ) _userNick = QString::fromUtf8( sl.front().c_str() );
	
	// hub
	sl = dcpp::ClientManager::getInstance()->getHubNames( pTransfer->getUser()->getCID() );
	if ( sl.size() > 0 ) _userHub = QString::fromUtf8( sl.front().c_str() );
}



} // namespace

// EOF


