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
#include <dcpp/Download.h>
#include <dcpp/Upload.h>

// local
#include "transferwidget.h"

namespace KRufusDc {

// ============================================================================
// Constructor
TransferWidget::TransferWidget(QWidget* parent)
	: TabContent(parent)
{
	setupUi( this );
	setTabTitle( i18n("Transfers") );
	
	dcpp::QueueManager::getInstance()->addListener(this);
	dcpp::DownloadManager::getInstance()->addListener(this);
	dcpp::UploadManager::getInstance()->addListener(this);
	dcpp::ConnectionManager::getInstance()->addListener(this);
}

// ============================================================================
// Destructor
TransferWidget::~TransferWidget()
{
	dcpp::QueueManager::getInstance()->removeListener(this);
	dcpp::DownloadManager::getInstance()->removeListener(this);
	dcpp::UploadManager::getInstance()->removeListener(this);
	dcpp::ConnectionManager::getInstance()->removeListener(this);
}

void TransferWidget::on(dcpp::DownloadManagerListener::Requesting, dcpp::Download* dl) throw()
{
	qDebug("TransferWidget: DownloadManagerListener::Requesting");
}

void TransferWidget::on(dcpp::DownloadManagerListener::Starting, dcpp::Download* dl) throw()
{
	qDebug("TransferWidget: DownloadManagerListener::Starting");
}

void TransferWidget::on(dcpp::DownloadManagerListener::Tick, const dcpp::DownloadList& dls) throw()
{
	qDebug("TransferWidget: DownloadManagerListener::Tick");
	// TODO use this
	for( dcpp::DownloadList::const_iterator it = dls.begin(); it != dls.end(); ++it )
	{
		dcpp::Download* pDownload = *it;
		
		uint64_t size = pDownload->getSize();
		uint64_t left = pDownload->getBytesLeft();
		const char* path = pDownload->getPath().c_str();
		
		qDebug("TransferWidget: download tick, file: %s, size: %lld, left: %lld",
			path, size, left );
	}
}

void TransferWidget::on(dcpp::DownloadManagerListener::Complete, dcpp::Download* dl) throw()
{
	qDebug("TransferWidget: DownloadManagerListener::Complete");
}

void TransferWidget::on(dcpp::DownloadManagerListener::Failed, dcpp::Download* dl, const std::string& reason) throw()
{
	qDebug("TransferWidget: DownloadManagerListener::Failed");
}

// ConnectionManager
void TransferWidget::on(dcpp::ConnectionManagerListener::Added, dcpp::ConnectionQueueItem* cqi) throw()
{
	qDebug("TransferWidget: ConnectionManagerListener::Added");
}

void TransferWidget::on(dcpp::ConnectionManagerListener::Connected, dcpp::ConnectionQueueItem* cqi) throw()
{
	qDebug("TransferWidget: ConnectionManagerListener::Connected");
}

void TransferWidget::on(dcpp::ConnectionManagerListener::Removed, dcpp::ConnectionQueueItem* cqi) throw()
{
	qDebug("TransferWidget: ConnectionManagerListener::Removed");
}

void TransferWidget::on(dcpp::ConnectionManagerListener::Failed, dcpp::ConnectionQueueItem* cqi, const std::string&) throw()
{
	qDebug("TransferWidget: ConnectionManagerListener::Failed");
}

void TransferWidget::on(dcpp::ConnectionManagerListener::StatusChanged, dcpp::ConnectionQueueItem* cqi) throw()
{
	qDebug("TransferWidget: ConnectionManagerListener::StatusChanged");
}

// QueueManager
void TransferWidget::on(dcpp::QueueManagerListener::Finished, dcpp::QueueItem* qi, const std::string&, int64_t size) throw()
{
	qDebug("TransferWidget: QueueManagerListener::Finished");
}

void TransferWidget::on(dcpp::QueueManagerListener::Removed, dcpp::QueueItem* qi) throw()
{
	qDebug("TransferWidget: QueueManagerListener::Removed");
}

// UploadManager
void TransferWidget::on(dcpp::UploadManagerListener::Starting, dcpp::Upload* ul) throw()
{
	qDebug("TransferWidget: UploadManagerListener::Starting");
}

void TransferWidget::on(dcpp::UploadManagerListener::Tick, const dcpp::UploadList& uls) throw()
{
	qDebug("TransferWidget: UploadManagerListener::Tick");
}

void TransferWidget::on(dcpp::UploadManagerListener::Complete, dcpp::Upload* ul) throw()
{
	qDebug("TransferWidget: UploadManagerListener::Complete");
}

void TransferWidget::on(dcpp::UploadManagerListener::Failed, dcpp::Upload* ul, const std::string& reason) throw()
{
	qDebug("TransferWidget: UploadManagerListener::Failed");
}


} // ns

// EOF


