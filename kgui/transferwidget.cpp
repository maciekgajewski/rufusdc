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

// Qt
#include <QFileInfo>

// KDE
#include <KMenu>
#include <KIcon>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/Download.h>
#include <dcpp/Upload.h>

// local
#include "transferinfo.h"
#include "utils.h"

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
	
	// init tree
	
	// create two - top level items, for downloads and uploads
	_pDownloadsItem = new QTreeWidgetItem( pTree );
	_pDownloadsItem->setText( 0, i18n("Downloads") );
	_pDownloadsItem->setIcon( 0, KIcon("folder-green") );
	_pDownloadsItem->setFirstColumnSpanned ( true );
	
	_pUploadsItem = new QTreeWidgetItem( pTree );
	_pUploadsItem->setText( 0, i18n("Uploads") );
	_pUploadsItem->setIcon( 0, KIcon("folder-red") );
	_pUploadsItem->setFirstColumnSpanned ( true );
	
	initDownloads();
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

// ============================================================================
// Download added
void TransferWidget::downloadAdded( const TransferInfo& info )
{
	qDebug("Dowload added, file: %s", qPrintable( info.path() ) );
	
	// TODO make sure there is no such download already

	QTreeWidgetItem* pItem = new QTreeWidgetItem( _pDownloadsItem );
	
	QString baseName = QFileInfo( info.path() ).fileName();
	
	// file name
	pItem->setText( COLUMN_NAME, baseName );
	pItem->setToolTip( COLUMN_NAME, info.path() );
	
	// icon
	if ( info.onlineUsers() > 0 )
	{
		pItem->setIcon( COLUMN_NAME, KIcon("user-online") );
	}
	else
	{
		pItem->setIcon( COLUMN_NAME, KIcon("user-offline") );
	}
	
	// progress
	int64_t size = info.size();
	int64_t downloaded = info.transferred();
	double percent = 100.0*(downloaded/size);
	QString progress = QString("<b>%1%</b> (%2 / %3)")
		.arg( percent, 0, 'g', 2 )
		.arg( sizeToString( downloaded ) )
		.arg( sizeToString( size ) );
		
	pItem->setText( COLUMN_PROGRESS, progress );
	
	// TTH
	pItem->setText( COLUMN_TTH, info.TTH() );
	
	
}

// ============================================================================
// Init tree
void TransferWidget::initTransferWidget()
{
	// resize columns to content
	for( int i = 0; i < pTree->columnCount(); i++ )
	{
		pTree->resizeColumnToContents( i );
	}
	
	_pDownloadsItem->setExpanded( true );
}

// ============================================================================
// Initializes downloads
void TransferWidget::initDownloads()
{
	const dcpp::QueueItem::StringMap &ll = dcpp::QueueManager::getInstance()->lockQueue();
	
	for( dcpp::QueueItem::StringMap::const_iterator it = ll.begin(); it != ll.end(); ++it)
	{
		// extract QueueItem
		dcpp::QueueItem* pItem = it->second;
	
		// convert to transfer info
		TransferInfo info;
		info.fromQueueItem( pItem );
		
		// add method to queue
		invoke("downloadAdded", Q_ARG( TransferInfo, info ) );
		invoke("initTransferWidget"); // put it at the end of the queue
	}
	
	dcpp::QueueManager::getInstance()->unlockQueue();
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

void TransferWidget::on(dcpp::QueueManagerListener::Added, dcpp::QueueItem *item) throw()
{
	qDebug("TransferWidget: QueueManagerListener::Added");
	TransferInfo info;
	info.fromQueueItem( item );
	
	invoke( "downloadAdded", Q_ARG( TransferInfo, info ) );	
}

void TransferWidget::on(dcpp::QueueManagerListener::Moved, dcpp::QueueItem *item, const std::string &oldTarget) throw()
{
	qDebug("TransferWidget: QueueManagerListener::Moved");
}

void TransferWidget::on(dcpp::QueueManagerListener::SourcesUpdated, dcpp::QueueItem *item) throw()
{
	qDebug("TransferWidget: QueueManagerListener::SourcesUpdated");
}

void TransferWidget::on(dcpp::QueueManagerListener::StatusUpdated, dcpp::QueueItem *item) throw()
{
	qDebug("TransferWidget: QueueManagerListener::StatusUpdated");
}


} // ns

// EOF


