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

// WARNING: all this need to be _before_ any Qt includes

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "rufusdc/client.h"
#include "rufusdc/hub.h"

// Qt
#include <QTimer>
#include <QCoreApplication>

#include "clientthread.h"

namespace KRufusDc
{

static const int PROCESS_EVENTS_INTERVAL = 1000; // event proceesing interval [ms]


// ============================================================================
// Constructor
ClientThread::ClientThread( QObject *parent )
		: QThread( parent )
{
	_pClient = new RufusDc::Client();
	_pTimer = NULL;
	_stopped = false;
}

// ============================================================================
// Destructor
ClientThread::~ClientThread()
{
}

// ============================================================================
// Thread routine
void ClientThread::run()
{
	// TODO remove this method if still empty
	//dive into endelss message loop
	exec();
}

// ============================================================================
// Run client
void ClientThread::runClient()
{
	//qDebug("runClient, tid: %u", int(QThread::currentThreadId()) );

	Q_ASSERT( _pClient );
	// ok, the message loop is running
	
	// attach method to timer
	_pTimer = new  boost::asio::deadline_timer
		( _pClient->ioService()
		, boost::posix_time::milliseconds( PROCESS_EVENTS_INTERVAL )
		);
	_pTimer->async_wait( boost::bind( &ClientThread::onTimer, this ) );
	
	forever
	{
		_pClient->run();
		if ( _stopped )
		{
			break;
		}
		msleep(1000); // this parameter should be tuned
	}
}

// ============================================================================
// Stop client
void ClientThread::stopClient()
{
	_stopped = true;
	_pClient->ioService().stop();
}

// ============================================================================
// Timer handler
void ClientThread::onTimer()
{
	//qDebug("onTimer, tid: %u", int(QThread::currentThreadId()) );
	Q_ASSERT( _pTimer );
	
	// process events form event loop. This will call all slots
	QCoreApplication::processEvents();
	
	// schedule next meeting
	_pTimer->expires_at( _pTimer->expires_at() + boost::posix_time::milliseconds( PROCESS_EVENTS_INTERVAL ) );
	_pTimer->async_wait( boost::bind( &ClientThread::onTimer, this ) );
}

// ============================================================================
// Connect hub
void ClientThread::slotConnectHub( const QString& addr )
{
	//qDebug("slotConnectHub, tid: %u", int(QThread::currentThreadId()) );

	boost::shared_ptr<RufusDc::Hub> pHub = _pClient->getHub( qPrintable( addr ) );
	pHub->connect();
}

// ============================================================================
// disconnect hub
void ClientThread::slotDisconnectHub( const QString& addr )
{
	boost::shared_ptr<RufusDc::Hub> pHub = _pClient->getHub( qPrintable( addr ) );
	pHub->disconnect();
}

} // namespace