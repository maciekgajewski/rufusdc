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

#include <unistd.h>

// rufusdc
#include "rufusdc/client.h"
#include "rufusdc/filelist.h"
#include "rufusdc/hubmanager.h"

// Qt
#include <QTimer>
#include <QFile>

#include "hub.h"

#include "client.h"

namespace KRufusDc
{

// ============================================================================
// Destructor
Client::Client( QObject *parent )
		: QObject( parent )
{
}

// ============================================================================
// Constructor
Client::~Client()
{
	_thread.quit();
}

// ============================================================================
// Stop
void Client::stop()
{
	qDebug("Sending stop signal to the other thread (tid: %d)", int(QThread::currentThreadId()));
	Q_EMIT stopThread();
	sleep(2); // give thread some time to die
}

// ============================================================================
// start
void Client::start()
{
	_thread.start();
	_thread.moveToThread( & _thread );
	
	// ui -> client
	connect( this, SIGNAL(startThread()), &_thread, SLOT(runClient()) );
	connect( this, SIGNAL(stopThread()), &_thread, SLOT(stopClient()) );
	
	connect( this, SIGNAL(signalConnectHub(QString)), &_thread, SLOT(slotConnectHub(QString)) );
	connect( this, SIGNAL(signalDisconnectHub(QString)), &_thread, SLOT(slotDisconnectHub(QString)) );
	
	
	connect
		( this 
		, SIGNAL(signalDownloadFile(QByteArray,QByteArray,QByteArray,QByteArray,quint64))
		, &_thread
		, SLOT(slotDownloadFile(QByteArray,QByteArray,QByteArray,QByteArray,quint64))
		);
	
	// client -> ui
	connect( &_thread, SIGNAL( signalFileListReceived() ), SLOT( fileListReceived() ) );
	
	Q_EMIT startThread();
}

// ============================================================================
// Create hub
Hub* Client::createHub( const QString& addr )
{
	Hub* pHub = new Hub( addr, this );
	
	return pHub;
}

// ============================================================================
// Connect hub
void Client::connectHub( const QString& addr )
{
	Q_EMIT signalConnectHub( addr );
}

// ============================================================================
// disconnect hub
void Client::disconnectHub( const QString& addr )
{
	Q_EMIT signalDisconnectHub( addr );
}

// ============================================================================
// File list received
void Client::fileListReceived()
{
	shared_ptr<RufusDc::FileList> pFileList;
	
	_thread.takeFileList( pFileList );
	
	if ( pFileList )
	{
		Q_EMIT signalFileListReceived( pFileList );
		
		// debug
		const std::string& xmldata = pFileList->xml();
		
		QFile dump("filelist.xml");
		dump.open( QIODevice::WriteOnly | QIODevice::Truncate );
		dump.write( xmldata.data(), xmldata.length() );
	}
	else
	{
		qWarning("Client thread announced new file list, but no list present!");
	}
}

// ============================================================================
// Downloads file
void Client::downloadFile
		( const QByteArray& hub
		, const QByteArray& nick
		, const QByteArray& path
		, const QByteArray& tth
		, quint64 size
		)
{
	Q_EMIT signalDownloadFile( hub, nick, path, tth, size );
}

}