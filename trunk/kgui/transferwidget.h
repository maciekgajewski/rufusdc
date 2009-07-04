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

class TransferInfo;

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
	void downloadAdded( const TransferInfo& info );
	
	/// Setups widget after content is added
	void initTransferWidget();

private: //  methods

	enum Columns
	{
		COLUMN_NAME = 0,
		COLUMN_PROGRESS,
		COLUMN_SPEED,
		COLUMN_TIME_LEFT,
		COLUMN_TTH
	};

	/// Initializes downloads list with items fro mdownload queue
	void initDownloads();

private: // data

	/// Fixed tree item - parent of all downloads
	QTreeWidgetItem* _pDownloadsItem;
	/// Fixed tree item - parent of all uploads
	QTreeWidgetItem* _pUploadsItem;

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
