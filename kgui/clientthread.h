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
#ifndef KRUFUSDCCLIENTTHREAD_H
#define KRUFUSDCCLIENTTHREAD_H

// boost
#include <boost/asio/deadline_timer.hpp>
#include <boost/shared_ptr.hpp>

// Qt
#include <QThread>
#include <QMutex>
#include <QList>

namespace RufusDc
{
	class Client;
	class Hub;
	class FileList;
}


namespace KRufusDc
{

/**
* @brief Thread runing RufusDc::Client
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ClientThread : public QThread
{
	Q_OBJECT

public:

	// Hub message type
	enum MessageType
	{
		System, ///< System message
		Chat    ///< Chat message
	};

	ClientThread( QObject *parent = 0 );
	virtual ~ClientThread();
	
	/**
	 * @brief Access to RufusDc::Client
	 * @return reference to underlying client object
	 */
	RufusDc::Client& client() { return *_pClient; }
	
	/// Takes file list from corss-thread container.
	/// Leaves out buffer intouched when no file lists present.
	void takeFileList( boost::shared_ptr<RufusDc::FileList>& pOut );
	
Q_SIGNALS:

	// x-thread signals
	
	/// Emitted when file list buffer contains nex file list.
	/// After receivingm use takeFileList to obtain actualfile lsit from corss-thread container
	void signalFileListReceived();
	
public Q_SLOTS:

	// cross-thread slots
	void slotConnectHub( const QString& addr );
	void slotDisconnectHub( const QString& addr );
	
	/// Runs client form within the message loop
	void runClient();
	
	/// Stops client ant hte entire thread
	void stopClient();

private:

	// boost slots and handlers

	/// Called by ASIO timer
	void onTimer();
	
	/// Called when client recives xml file list
	void onFileListReceived( const boost::shared_ptr<RufusDc::FileList>& pFileList );
	
private:

	RufusDc::Client* _pClient;
	
	boost::asio::deadline_timer* _pTimer;
	
	bool _stopped; ///< STOP flag
	
	QMutex _fileListMutex; /// Mutex guarding file lists
	QList< boost::shared_ptr<RufusDc::FileList> > _fileLists; /// File lists

};

} // namespcae

#endif // KRUFUSDCCLIENTTHREAD_H

// EOF


