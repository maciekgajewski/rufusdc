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
#ifndef KRUFUSDCTRANSFERWIDGET_H
#define KRUFUSDCTRANSFERWIDGET_H

// Qt
class QTreeWidgetItem;

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/ConnectionManager.h>
#include <dcpp/DownloadManager.h>
#include <dcpp/LogManager.h>
#include <dcpp/QueueManager.h>
#include <dcpp/UploadManager.h>

// local
#include "tabcontent.h"
#include "ui_transferwidget.h"

namespace KRufusDc
{

class DownloadInfo;
class TransferInfo;
class FixedPosItem;

/**
 * Widget with lsit of current transfers.
 * @author Maciek Gajewski <maciej.gajewski0@gmail.com>
 */
class TransferWidget
	: public TabContent
	, private Ui::TransferWidget
	, public dcpp::ConnectionManagerListener
	, public dcpp::DownloadManagerListener
	, public dcpp::QueueManagerListener
	, public dcpp::UploadManagerListener
{
	Q_OBJECT
public:
	
	/// Constructor
	TransferWidget(QWidget* parent);
	
	/// Destructor
	virtual ~TransferWidget();

private Q_SLOTS: // inter-thread receivers

	/// Called when download is added
	void downloadAdded( const DownloadInfo& info );
	
	/// Called when queue item status changes
	void downloadUpdated( const DownloadInfo& info );
	
	/// Called when download transfer is updated
	void downloadTransferUpdated( const TransferInfo& info );
	
	/// Called when download transfer is removed
	void downloadTransferRemoved( const TransferInfo& info );
	
	/// Setups widget after content is added
	void initTransferWidget();
	
	/// Updated accumulated data from transfers
	void updateDownloadFromTransfers( QTreeWidgetItem* pItem );
	
	/// Called when upload transfer should be updated
	void uploadUpdated( const TransferInfo& info );
	
	/// Called when upload transfer should be removed
	void uploadRemoved( const TransferInfo& info );

private: //  methods

	/// Tree widget items
	enum Columns
	{
		COLUMN_NAME = 0,
		COLUMN_PROGRESS,
		COLUMN_SPEED,
		COLUMN_TIME_LEFT,
		COLUMN_TTH
	};
	
	/// Extra data associated with tree widget item
	enum Roles
	{
		ROLE_SORT = Qt::UserRole + 1, ///< Data used to sort
		ROLE_DATA                     ///< Auxiliary data
	};

	/// Initializes downloads list with items fro mdownload queue
	void initDownloads();
	
	/// Updates trabsfer widget with info data
	void updateDownloadItem( QTreeWidgetItem* , const DownloadInfo& info );
	
	/// Finds download with specified TTH
	QTreeWidgetItem* findDownload( const QString& TTH );
	
	/// Updates doenload-transfer item
	void updateDownloadTransferItem( QTreeWidgetItem* pItem, const TransferInfo& info );

	/// find dowlaod transfer
	QTreeWidgetItem* findDownloadTransfer( QTreeWidgetItem* pParent, const QString& CID );
	
	/// Updates upload item
	void updateUploadItem( QTreeWidgetItem* pItem, const TransferInfo& info );
	
	/// Find item for upload
	QTreeWidgetItem* findUpload( const TransferInfo& info );


private: // data

	/// Fixed tree item - parent of all active downloads
	FixedPosItem* _pDownloadsItem;
	
	/// Fixed tree item - parent of all uploads
	FixedPosItem* _pUploadsItem;

	/// Fixed tree item - parent of all inactive downloads
	FixedPosItem* _pInactiveDownloadsItem;
	
	/// Fixed tree item - parent of all finished downloads
	FixedPosItem* _pFinishedDownloadsItem;

protected: // Listener methods

	// DownloadManager
	virtual void on(dcpp::DownloadManagerListener::Requesting, dcpp::Download* dl) throw();
	virtual void on(dcpp::DownloadManagerListener::Starting, dcpp::Download* dl) throw();
	virtual void on(dcpp::DownloadManagerListener::Tick, const dcpp::DownloadList& dls) throw();
	virtual void on(dcpp::DownloadManagerListener::Complete, dcpp::Download* dl) throw();
	virtual void on(dcpp::DownloadManagerListener::Failed, dcpp::Download* dl, const std::string& reason) throw();
	// ConnectionManager
	virtual void on(dcpp::ConnectionManagerListener::Added, dcpp::ConnectionQueueItem* cqi) throw();
	virtual void on(dcpp::ConnectionManagerListener::Connected, dcpp::ConnectionQueueItem* cqi) throw();
	virtual void on(dcpp::ConnectionManagerListener::Removed, dcpp::ConnectionQueueItem* cqi) throw();
	virtual void on(dcpp::ConnectionManagerListener::Failed, dcpp::ConnectionQueueItem* cqi, const std::string&) throw();
	virtual void on(dcpp::ConnectionManagerListener::StatusChanged, dcpp::ConnectionQueueItem* cqi) throw();
	// QueueManager
	virtual void on(dcpp::QueueManagerListener::Finished, dcpp::QueueItem* qi, const std::string&, int64_t size) throw();
	virtual void on(dcpp::QueueManagerListener::Removed, dcpp::QueueItem* qi) throw();
	virtual void on(dcpp::QueueManagerListener::Added, dcpp::QueueItem *item) throw();
	virtual void on(dcpp::QueueManagerListener::Moved, dcpp::QueueItem *item, const std::string &oldTarget) throw();
	virtual void on(dcpp::QueueManagerListener::SourcesUpdated, dcpp::QueueItem *item) throw();
	virtual void on(dcpp::QueueManagerListener::StatusUpdated, dcpp::QueueItem *item) throw();
	// UploadManager
	virtual void on(dcpp::UploadManagerListener::Starting, dcpp::Upload* ul) throw();
	virtual void on(dcpp::UploadManagerListener::Tick, const dcpp::UploadList& uls) throw();
	virtual void on(dcpp::UploadManagerListener::Complete, dcpp::Upload* ul) throw();
	virtual void on(dcpp::UploadManagerListener::Failed, dcpp::Upload* ul, const std::string& reason) throw();

};

}

#endif
