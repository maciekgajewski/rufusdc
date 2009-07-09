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
#include "downloadinfo.h"
#include "utils.h"
#include "smartsorttreeitem.h"
#include "actionfactory.h"
#include "userinfo.h"

#include "transferwidget.h"

namespace KRufusDc {

/// QTreeWidgetItem that holds it's posityion agains other, despite of sorting
class FixedPosItem : public QTreeWidgetItem
{
public:
	/// value used to sort
	int value;
	
	/// Constructor
	FixedPosItem( QTreeWidget* pWidget, int type ) : QTreeWidgetItem( pWidget, type ), value(0) {}
	
	/// Comparison operator
	virtual bool operator< ( const QTreeWidgetItem & other ) const
	{
		const FixedPosItem* pItem = dynamic_cast<const FixedPosItem*>( &other );
		
		Q_ASSERT( pItem );
		
		Qt::SortOrder order = treeWidget()->header()->sortIndicatorOrder();
		if ( order == Qt::AscendingOrder )
		{
			return value < pItem->value;
		}
		else
		{
			return value > pItem->value;
		}
	}
};


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
	
	// init tree
	
	// create top level items, for downloads and uploads
	_pDownloadsItem = new FixedPosItem( pTree, TYPE_FIXED );
	_pDownloadsItem->setText( COLUMN_NAME, i18n("Downloads") );
	_pDownloadsItem->setIcon( COLUMN_NAME, KIcon("folder-green") );
	_pDownloadsItem->setFirstColumnSpanned ( true );
	_pDownloadsItem->setExpanded( true );
	_pDownloadsItem->value = 0;
	
	_pInactiveDownloadsItem = new FixedPosItem( pTree, TYPE_FIXED );
	_pInactiveDownloadsItem->setText( COLUMN_NAME, i18n("Inactive downloads") );
	_pInactiveDownloadsItem->setIcon( COLUMN_NAME, KIcon("folder-grey") );
	_pInactiveDownloadsItem->setFirstColumnSpanned ( true );
	_pInactiveDownloadsItem->setExpanded( true );
	_pInactiveDownloadsItem->value = 1;
	
	_pFinishedDownloadsItem = new FixedPosItem( pTree, TYPE_FIXED );
	_pFinishedDownloadsItem->setText( COLUMN_NAME, i18n("Finished downloads") );
	_pFinishedDownloadsItem->setIcon( COLUMN_NAME, KIcon("folder-blue") );
	_pFinishedDownloadsItem->setFirstColumnSpanned ( true );
	_pFinishedDownloadsItem->setExpanded( true );
	_pFinishedDownloadsItem->value = 2;
	
	_pUploadsItem = new FixedPosItem( pTree, TYPE_FIXED );
	_pUploadsItem->setText( COLUMN_NAME, i18n("Uploads") );
	_pUploadsItem->setIcon( COLUMN_NAME, KIcon("folder-red") );
	_pUploadsItem->setFirstColumnSpanned ( true );
	_pUploadsItem->setExpanded( true );
	_pUploadsItem->value = 3;
	
	connect( pTree, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(treeContextMenu(const QPoint &) ) );

	// init content
	initDownloads();
	
}

// ============================================================================
// Destructor
TransferWidget::~TransferWidget()
{
	dcpp::QueueManager::getInstance()->removeListener(this);
	dcpp::DownloadManager::getInstance()->removeListener(this);
	dcpp::UploadManager::getInstance()->removeListener(this);
}

// ============================================================================
// Download added
void TransferWidget::downloadAdded( const DownloadInfo& info )
{
	//qDebug("Dowload added, file: %s", qPrintable( info.path() ) );
	
	// TODO make sure there is no such download already
	SmartSortTreeItem* pItem = new SmartSortTreeItem( TYPE_DOWNLOAD );
	updateDownloadItem( pItem, info );
}

// ============================================================================
// Download removed
void TransferWidget::downloadRemoved( const DownloadInfo& info )
{
	// find appropriate item
	SmartSortTreeItem* pItem = findDownload( info.TTH() );

	Q_ASSERT( pItem );
	delete pItem; // have no mercy!
}

// ============================================================================
// Find download
SmartSortTreeItem* TransferWidget::findDownload( const QString& TTH )
{
	// create list of all download items
	QList<QTreeWidgetItem *> items;
	
	QList<QTreeWidgetItem*> parents;
	parents << _pInactiveDownloadsItem << _pDownloadsItem << _pFinishedDownloadsItem;
	Q_FOREACH( QTreeWidgetItem* parent, parents )
	{
		int childCount = parent->childCount();
		for( int i = 0; i < childCount; i++ )
		{
			items.append( parent->child(i) );
		}
	}
	
	// find the one
	Q_FOREACH( QTreeWidgetItem* pItem, items )
	{
		QString itemsTTH = pItem->text( COLUMN_TTH );
		if ( itemsTTH == TTH )
		{
			return dynamic_cast< SmartSortTreeItem* >( pItem );
		}
	}
	
	return NULL;
}

// ============================================================================
// Donload transfer added
void TransferWidget::downloadTransferUpdated( const TransferInfo& info )
{
	// find parent item
	SmartSortTreeItem* pParent = findDownload( info.TTH() );
	
	Q_ASSERT( pParent );
	
	// find download
	SmartSortTreeItem* pItem = findDownloadTransfer( pParent, info.CID() );
	if ( !pItem )
	{
		pItem = new SmartSortTreeItem( pParent, TYPE_USER );
	}

	Q_ASSERT( pItem );
	
	// create download
	updateTransferItem( pItem, info );
	
	updateDownloadFromTransfers( pParent );
}

// ============================================================================
// Transfer removed
void TransferWidget::downloadTransferRemoved( const TransferInfo& info )
{
	// find parent item
	SmartSortTreeItem* pParent = findDownload( info.TTH() );
	
	Q_ASSERT( pParent );
	
	// find download
	SmartSortTreeItem* pItem = findDownloadTransfer( pParent, info.CID() );
	Q_ASSERT( pItem );
	
	delete pItem; // goodbye!
}

// ============================================================================
// Update download item from transfer items
void TransferWidget::updateDownloadFromTransfers( SmartSortTreeItem* pItem )
{
	int childCount = pItem->childCount();
	double speed = 0;
	int64_t maxTime = 0;
	for ( int i = 0; i < childCount; i++ )
	{
		QTreeWidgetItem* pChild = pItem->child(i);
		TransferInfo info = pChild->data( 0, ROLE_DATA ).value<TransferInfo>();
		
		speed += info.averageSpeed();
		maxTime = qMax( maxTime, info.secondsLeft() );
	}
	
	DownloadInfo info = pItem->data( 0, ROLE_DATA ).value<DownloadInfo>();
	
	int64_t bytesRemaining = info.size() - info.transferred();
	int64_t timeCalculated = bytesRemaining/speed;
	
	int64_t time = qMax( timeCalculated, maxTime );
	
	// speed
	pItem->setText( COLUMN_SPEED, speedToString( speed ) );
	pItem->setValue( COLUMN_SPEED, speed );
	
	// time remaining
	pItem->setText( COLUMN_TIME_LEFT, timeToString( time ) );
	pItem->setValue( COLUMN_TIME_LEFT, double( time ) );
}

// ============================================================================
// Find download transfer
SmartSortTreeItem* TransferWidget::findDownloadTransfer( SmartSortTreeItem* pParent, const QString& CID )
{
	Q_ASSERT( pParent );
	
	int childCount = pParent->childCount();
	for ( int i = 0; i < childCount; ++i )
	{
		QTreeWidgetItem* pItem = pParent->child(i);
		TransferInfo info = pItem->data( 0, ROLE_DATA).value<TransferInfo>();
		
		if ( info.CID() == CID )
		{
			return dynamic_cast<SmartSortTreeItem*>( pItem );
		}
	}
	
	return NULL;
}

// ============================================================================
// Update download transfer
void TransferWidget::updateTransferItem( SmartSortTreeItem* pItem, const TransferInfo& info )
{
	// name/icon/data
	pItem->setText( COLUMN_NAME, info.userNick() );
	pItem->setIcon( COLUMN_NAME, KIcon("user-identity") );
	pItem->setValue( COLUMN_NAME, info.userNick().toLower() ); // to get case-insensitive search
	pItem->setData( 0, ROLE_DATA, QVariant::fromValue<TransferInfo>( info ) );
	
	// progress
	QString status = info.status();
	QString progress;
	double progresValue = 100;;
	if ( status.isEmpty() )
	{
		int64_t size = info.size();
		int64_t downloaded = info.transferred();
		double percent = 100.0*(double(downloaded)/double(size));
		progresValue = percent;
		progress = QString("%1% (%2 / %3)")
			.arg( percent, 0, 'f', 2 )
			.arg( sizeToString( downloaded ) )
			.arg( sizeToString( size ) );
	}
	else
	{
		progress = status;
	}
	pItem->setText( COLUMN_PROGRESS, progress );
	pItem->setValue( COLUMN_PROGRESS, progresValue );
	
	// speed
	QString speed = speedToString( info.averageSpeed() );
	pItem->setText( COLUMN_SPEED, speed );
	pItem->setValue( COLUMN_SPEED, info.averageSpeed() );
	
	// remaining time TODO format properly
	pItem->setText( COLUMN_TIME_LEFT, timeToString( info.secondsLeft() ) );
	pItem->setValue( COLUMN_TIME_LEFT, double(info.secondsLeft()) );
	
	// aux
	pItem->setData( 0, ROLE_DATA, QVariant::fromValue( info ) );
}

// ============================================================================
// download updatred
void TransferWidget::downloadUpdated( const DownloadInfo& info )
{
	// find appropriate item
	SmartSortTreeItem* pItem = findDownload( info.TTH() );
	
	Q_ASSERT( pItem );
	
	updateDownloadItem( pItem, info );
}

// ============================================================================
// update down
void TransferWidget::updateDownloadItem( SmartSortTreeItem* pItem, const DownloadInfo& info )
{
	// get state info
	QTreeWidgetItem* pParent = NULL;
	QIcon icon;
	if ( info.state() == DownloadInfo::FINISHED )
	{
		pParent = _pFinishedDownloadsItem;
		icon = KIcon("task-complete");
		pItem->setText( COLUMN_TIME_LEFT, "" );
		pItem->setValue( COLUMN_TIME_LEFT, 0.0 );
		
		pItem->setText( COLUMN_SPEED, "" );
		pItem->setValue( COLUMN_SPEED, 0.0 );
	}
	else if ( info.state() == DownloadInfo::WAITING )
	{
		pParent = _pInactiveDownloadsItem;
		icon = KIcon("user-away");
		pItem->setText( COLUMN_TIME_LEFT, "" );
		pItem->setValue( COLUMN_TIME_LEFT, 0.0 );
		
		pItem->setText( COLUMN_SPEED, "" );
		pItem->setValue( COLUMN_SPEED, 0.0 );
	}
	else
	{
		pParent = _pDownloadsItem;
		icon = KIcon("user-online");
	}
	
	// set parent
	QTreeWidgetItem* pCurrentParent = pItem->parent();
	if ( pParent != pCurrentParent )
	{
		if ( pCurrentParent )
		{
			pCurrentParent->removeChild( pItem );
		}
		pParent->addChild( pItem );
	}
	
	// file name
	if ( info.isFileList() )
	{
		pItem->setText( COLUMN_NAME, i18n("File list") ); // TODO add user name
		pItem->setValue( COLUMN_NAME, "" );
	}
	else
	{
		QString baseName = QFileInfo( info.path() ).fileName();
		
		pItem->setText( COLUMN_NAME, baseName );
		pItem->setValue( COLUMN_NAME, baseName.toLower() );
		pItem->setToolTip( COLUMN_NAME, info.path() );
	}
	
	// icon
	pItem->setIcon( COLUMN_NAME, icon );
	
	// progress
	if ( info.state() == DownloadInfo::FINISHED )
	{
		pItem->setText( COLUMN_PROGRESS, i18n("Finished") );
		pItem->setValue( COLUMN_PROGRESS, 100.0 );
	}
	else
	{
		int64_t size = info.size();
		int64_t downloaded = info.transferred();
		double percent = 100.0*(double(downloaded)/double(size));
		QString progress = QString("%1% (%2 / %3)")
			.arg( percent, 0, 'f', 2 )
			.arg( sizeToString( downloaded ) )
			.arg( sizeToString( size ) );
			
		pItem->setText( COLUMN_PROGRESS, progress );
		pItem->setValue( COLUMN_PROGRESS, percent );
	}
	
	// TTH
	pItem->setText( COLUMN_TTH, info.TTH() );
	
	// aux
	pItem->setData( 0, ROLE_DATA, QVariant::fromValue<DownloadInfo>( info ) );
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
		dcpp::DownloadList& downloads = pItem->getDownloads();
	
		// convert to transfer info
		DownloadInfo info;
		info.fromQueueItem( pItem );
		
		// add method to queue
		invoke("downloadAdded", Q_ARG( DownloadInfo, info ) );
		
		for( dcpp::DownloadList::iterator it = downloads.begin(); it != downloads.end(); ++it )
		{
			dcpp::Download* pDownload = *it;
			TransferInfo info;
			info.fromDcppDownload( pDownload );
			
			invoke("downloadTransferAdded", Q_ARG( TransferInfo, info ) );
		}
	}
	
	invoke("initTransferWidget"); // put it at the end of the queue
	dcpp::QueueManager::getInstance()->unlockQueue();
}

// ============================================================================
// upload updated
void TransferWidget::uploadUpdated( const TransferInfo& info )
{
	SmartSortTreeItem* pItem = findUpload( info );
	
	if ( !pItem )
	{
		pItem = new SmartSortTreeItem( _pUploadsItem );
	}
	
	updateUploadItem( pItem, info );
	
}

// ============================================================================
// upload removed
void TransferWidget::uploadRemoved( const TransferInfo& info )
{
	SmartSortTreeItem* pItem = findUpload( info );
	
	if ( pItem )
	{
		delete pItem;
	}
}

// ============================================================================
// Update upload item
void TransferWidget::updateUploadItem( SmartSortTreeItem* pItem, const TransferInfo& info )
{
	// re-use download code
	updateTransferItem( pItem, info );
	
	// fit file name and user nick in first column
	QString fileName = QFileInfo( info.path() ).fileName();
	
	pItem->setText( COLUMN_NAME, QString("%1 / %2@%3").arg( fileName ).arg( info.userNick() ).arg( info.userHub() ) );
}

// ============================================================================
// find upload item
SmartSortTreeItem* TransferWidget::findUpload( const TransferInfo& info )
{
	int childCount = _pUploadsItem->childCount();
	for ( int i = 0; i < childCount; ++i )
	{
		QTreeWidgetItem* pItem = _pUploadsItem->child(i);
		TransferInfo info = pItem->data( 0, ROLE_DATA).value<TransferInfo>();
		
		if ( info.CID() == info.CID() && info.TTH() == info.TTH() )
		{
			return dynamic_cast<SmartSortTreeItem*>( pItem );
		}
	}

	return NULL;
}

void TransferWidget::on(dcpp::DownloadManagerListener::Requesting, dcpp::Download* dl) throw()
{
	//qDebug("TransferWidget: DownloadManagerListener::Requesting");
	TransferInfo info;
	info.fromDcppDownload( dl );
	info.setStatus( i18n("Connecting" ) );
	
	invoke("downloadTransferUpdated", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::DownloadManagerListener::Starting, dcpp::Download* dl) throw()
{
	//qDebug("TransferWidget: DownloadManagerListener::Starting");
	TransferInfo info;
	info.fromDcppDownload( dl );
	info.setStatus( i18n("Starting" ) );
	
	invoke("downloadTransferUpdated", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::DownloadManagerListener::Tick, const dcpp::DownloadList& dls) throw()
{
	//qDebug("TransferWidget: DownloadManagerListener::Tick");
	
	for( dcpp::DownloadList::const_iterator it = dls.begin(); it != dls.end(); ++it )
	{
		dcpp::Download* pDownload = *it;
		
		TransferInfo info;
		info.fromDcppDownload( pDownload );
		
		invoke("downloadTransferUpdated", Q_ARG( TransferInfo, info ) );
	}
}

void TransferWidget::on(dcpp::DownloadManagerListener::Complete, dcpp::Download* dl) throw()
{
	//qDebug("TransferWidget: DownloadManagerListener::Complete");
	
	TransferInfo info;
	info.fromDcppDownload( dl );
	invoke("downloadTransferRemoved", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::DownloadManagerListener::Failed, dcpp::Download* dl, const std::string& reason) throw()
{
	//qDebug("TransferWidget: DownloadManagerListener::Failed");
	TransferInfo info;
	info.fromDcppDownload( dl );
	invoke("downloadTransferRemoved", Q_ARG( TransferInfo, info ) );
}

// QueueManager
void TransferWidget::on(dcpp::QueueManagerListener::Finished, dcpp::QueueItem* qi, const std::string&, int64_t size) throw()
{
	//qDebug("TransferWidget: QueueManagerListener::Finished");
	DownloadInfo info;
	info.fromQueueItem( qi );
	
	invoke( "downloadUpdated", Q_ARG( DownloadInfo, info ) );	
}

void TransferWidget::on(dcpp::QueueManagerListener::Removed, dcpp::QueueItem* qi) throw()
{
	//qDebug("TransferWidget: QueueManagerListener::Removed");
	DownloadInfo info;
	info.fromQueueItem( qi );
	
	invoke( "downloadRemoved", Q_ARG( DownloadInfo, info ) );	
}

// UploadManager
void TransferWidget::on(dcpp::UploadManagerListener::Starting, dcpp::Upload* ul) throw()
{
	//qDebug("TransferWidget: UploadManagerListener::Starting");
	
	TransferInfo info;
	info.fromDcppUpload( ul );
	
	invoke("uploadUpdated", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::UploadManagerListener::Tick, const dcpp::UploadList& uls) throw()
{
	//qDebug("TransferWidget: UploadManagerListener::Tick");
	for( dcpp::UploadList::const_iterator it = uls.begin(); it != uls.end(); ++it )
	{
		TransferInfo info;
		info.fromDcppUpload( *it );
		
		invoke("uploadUpdated", Q_ARG( TransferInfo, info ) );
	}
}

void TransferWidget::on(dcpp::UploadManagerListener::Complete, dcpp::Upload* ul) throw()
{
	//qDebug("TransferWidget: UploadManagerListener::Complete");
	
	TransferInfo info;
	info.fromDcppUpload( ul );
	
	invoke("uploadRemoved", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::UploadManagerListener::Failed, dcpp::Upload* ul, const std::string& reason) throw()
{
	//qDebug("TransferWidget: UploadManagerListener::Failed");
	
	TransferInfo info;
	info.fromDcppUpload( ul );
	
	invoke("uploadRemoved", Q_ARG( TransferInfo, info ) );
}

void TransferWidget::on(dcpp::QueueManagerListener::Added, dcpp::QueueItem *item) throw()
{
	//qDebug("TransferWidget: QueueManagerListener::Added");
	DownloadInfo info;
	info.fromQueueItem( item );
	
	invoke( "downloadAdded", Q_ARG( DownloadInfo, info ) );	
}

void TransferWidget::on(dcpp::QueueManagerListener::Moved, dcpp::QueueItem *item, const std::string &oldTarget) throw()
{
	qDebug("TransferWidget: QueueManagerListener::Moved");
}

void TransferWidget::on(dcpp::QueueManagerListener::SourcesUpdated, dcpp::QueueItem *item) throw()
{
	//qDebug("TransferWidget: QueueManagerListener::SourcesUpdated");
	DownloadInfo info;
	info.fromQueueItem( item );
	
	invoke( "downloadUpdated", Q_ARG( DownloadInfo, info ) );	
}

void TransferWidget::on(dcpp::QueueManagerListener::StatusUpdated, dcpp::QueueItem *item) throw()
{
	//qDebug("TransferWidget: QueueManagerListener::StatusUpdated");
	DownloadInfo info;
	info.fromQueueItem( item );
	
	invoke( "downloadUpdated", Q_ARG( DownloadInfo, info ) );	
}

// ============================================================================
// Tree context menu
void TransferWidget::treeContextMenu(const QPoint& point )
{
	// get current item
	QTreeWidgetItem* pCurrentItem = pTree->currentItem();
	if ( ! pCurrentItem )
	{
		return; // do nothing
	}
	
	// check item type
	int type = pCurrentItem->type();
	
	// fixed item
	if ( type == TYPE_FIXED )
	{
		// do nothing
		return;
	}
	// user item
	else if ( type = TYPE_USER )
	{
		// get user info
		TransferInfo transfer = pCurrentItem->data( 0, ROLE_DATA).value< TransferInfo >();
		UserInfo info;
		info.fromTransferInfo( transfer );
		QString nick = info.nick();
			
		// init menu
		KMenu menu( this );
		
		// title
		menu.addTitle( nick );
		
		// actions
		menu.addAction( ActionFactory::createUserAction( &menu, info, ActionFactory::CopyNick ) );
		menu.addAction( ActionFactory::createUserAction( &menu, info, ActionFactory::GetFileList ) );
		menu.addAction( ActionFactory::createUserAction( &menu, info, ActionFactory::MatchQueue ) );
		menu.addAction( ActionFactory::createUserAction( &menu, info, ActionFactory::GrantSlot ) );
		menu.addAction(  ActionFactory::createUserAction( &menu, info, ActionFactory::RemoveUserFromQueue ) );
		
		// go!
		QAction* pRes = menu.exec( pTree->mapToGlobal( point ) );
		if ( pRes )
		{
			pRes->trigger();
		}
		
	}

}

} // ns

// EOF


