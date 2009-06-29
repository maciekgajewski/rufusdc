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

// boost
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

// Qt
#include <QTextCodec>
#include <QMutexLocker>

// rufusdc
#include "rufusdc/hub.h"

// local
#include "client.h"
#include "clientthreadhub.h"

#include "hub.h"

namespace KRufusDc
{

using namespace boost;

// ============================================================================
// Constructor
Hub::Hub( const QString& addr, Client *parent )
	: QObject( parent )
	, _pParent( parent )
	, _address( addr )
{
	Q_ASSERT( parent );
	
	_thread = new ClientThreadHub( parent, addr );
	_thread->setHubEncoding( "WINDOWS-1250" );
	_thread->moveToThread( &parent->clientThread() );
	
	// start the object in worker thread
	QMetaObject::invokeMethod( _thread, "start", Qt::QueuedConnection );
	
	
	// connect self to QObject in the otherthread
	QObject::connect( this, SIGNAL(wtRequestFileList(const QString&)), _thread,  SLOT(utRequestFileList(const QString&)) );
	QObject::connect( this, SIGNAL(wtSendChatMessage(const QString&)), _thread,  SLOT(utSendChatMessage(const QString&)) );
	
	QObject::connect( _thread, SIGNAL(signalWtMessage(int,const QString&)), SLOT(wtMessage(int,const QString&)) );
	QObject::connect( _thread, SIGNAL(signalWtTopicChanged(const QString&)), SLOT(wtTopicChanged(const QString&)) );
	QObject::connect( _thread, SIGNAL(signalWtNameChanged(const QString&)), SLOT(wtNameChanged(const QString&)) );
}

// ============================================================================
// Destructor
Hub::~Hub()
{
	_thread->deleteLater();
}

// ============================================================================
// message
void Hub::wtMessage( int type, const QString& msg )
{
	Q_EMIT hubMessage( type, msg );
}

// ============================================================================
// Connect
void Hub::connect()
{
	Q_ASSERT( _pParent );
	
	_pParent->connectHub( _address );
}

// ============================================================================
// Disconnect
void Hub::disconnect()
{
	_pParent->disconnectHub( _address );
}

// ============================================================================
// Requests file list
void Hub::requestFileList( const QString& nick )
{
	Q_EMIT 	wtRequestFileList( nick );

}

// ============================================================================
// name changed
void Hub::wtNameChanged( const QString& name )
{
	if ( name != _name )
	{
		_name = name;
		Q_EMIT hubNameChanged( name );
	}
}

// ============================================================================
// topic changed
void Hub::wtTopicChanged( const QString& topic )
{
	if ( topic != _topic )
	{
		_topic = topic;
		Q_EMIT hubTopicChanged( topic );
	}
}

// ============================================================================
// Send chat message
void Hub::sendChatMessage( const QString& msg )
{
	wtSendChatMessage( msg );
}


}

