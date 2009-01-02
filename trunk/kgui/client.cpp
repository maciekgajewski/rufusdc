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

#include "rufusdc/client.h"

#include <QTimer>

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
	emit stopThread();
	sleep(1); // give thread some time to die
}

// ============================================================================
// start
void Client::start()
{
	_thread.start();
	_thread.moveToThread( & _thread );
	
	connect( this, SIGNAL(startThread()), &_thread, SLOT(runClient()) );
	connect( this, SIGNAL(stopThread()), &_thread, SLOT(stopClient()) );
	
	connect( this, SIGNAL(signalConnectHub(QString)), &_thread, SLOT(slotConnectHub(QString)) );
	connect( this, SIGNAL(signalDisconnectHub(QString)), &_thread, SLOT(slotDisconnectHub(QString)) );
	
	emit startThread();
}

// ============================================================================
// Create hub
Hub* Client::createHub( const QString addr )
{
	shared_ptr<RufusDc::Hub> pRDCHub = _thread.client().getHub( qPrintable( addr ) );
	
	Hub* pHub = new Hub( pRDCHub, this );
	
	return pHub;
}

// ============================================================================
// Connect hub
void Client::connectHub( const QString& addr )
{
	emit signalConnectHub( addr );
}

// ============================================================================
// disconnect hub
void Client::disconnectHub( const QString& addr )
{
	emit signalDisconnectHub( addr );
}

}
