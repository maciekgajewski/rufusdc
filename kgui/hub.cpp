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

#include "hub.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
Hub::Hub( const shared_ptr<RufusDc::Hub>& pHub, Client *parent )
	: QObject( parent )
	, _pHub( pHub )
	, _pParent( parent )
{
	Q_ASSERT( parent );
	
	_thread = new ClientThreadHubAnchor( this );
	_thread->setHubEncoding( "WINDOWS-1250" );
	_thread->moveToThread( &parent->clientThread() );
	
	
	// connect self to QObject in the otherthread
	QObject::connect( _thread, SIGNAL(signalWtMessage(int,const QString&)), SLOT(wtMessage(int,const QString&)) );
	
	// cache this ASAP, to not fiddle threads later
	_address = pHub->address().c_str();
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
	emit hubMessage( type, msg );
}

// ============================================================================
// other thread wt message
void ClientThreadHubAnchor::wtMessage( int type, const std::string& msg )
{
	if ( _pCodec )
	{
		emit signalWtMessage( type, _pCodec->toUnicode( msg.c_str() ) );
	}
	else
	{
		emit signalWtMessage( type, msg.c_str() );
	}
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
// Sets encoding
void ClientThreadHubAnchor::setHubEncoding( const QByteArray& name )
{
	_pCodec = QTextCodec::codecForName( name );
}

// ============================================================================
// Thread anchor constructor
ClientThreadHubAnchor::ClientThreadHubAnchor( Hub* parent, QObject* qparent )
	: QObject(qparent)
	, _pCodec(0)
	, _pParent( parent )
{
	Q_ASSERT( parent );
	
	// connect to client's thread
	ClientThread& thread = parent->parent()->clientThread();
	
	// connect the object with actual hub implementation
	parent->hub()->signalChatMessage.connect( boost::bind( &ClientThreadHubAnchor::wtMessage, this, Hub::Chat, _1 ) );
	parent->hub()->signalSystemMessage.connect( boost::bind( &ClientThreadHubAnchor::wtMessage, this, Hub::System, _1 ) );
	
	parent->hub()->signalUserAdded.connect( boost::bind( &ClientThreadHubAnchor::wtUserAdded, this, _1 ) );
	parent->hub()->signalUserModified.connect( boost::bind( &ClientThreadHubAnchor::wtUserModified, this, _1 ) );
	parent->hub()->signalUserRemoved.connect( boost::bind( &ClientThreadHubAnchor::wtUserRemoved, this, _1 ) );
}

// ============================================================================
// User added
void ClientThreadHubAnchor::wtUserAdded( const RufusDc::UserInfo& rawInfo )
{
	QMutexLocker locker( & _userMutex );
	UserInfo info;
	info.convert( rawInfo, _pCodec );
	QString key = info.nick().toLower();
	
	//qDebug("ANCHOR: user added: %s", qPrintable(key) );
	
	// consistency check - user shouldn't be on modified list
	if ( _modifiedUsers.contains( key ) )
	{
		qWarning("User list inconsistent, user '%s' added, while on modified list", 
			qPrintable( info.nick() ) );
		_modifiedUsers.remove( key );
	}
	
	// is user in 'removed' list?
	if ( _removedUsers.contains( key ) )
	{
		// so, user was removed, then added again. let's pretend it was modified
		_removedUsers.remove( info.nick() );
		_modifiedUsers[ key ] = info;
	}
	else
	{
		_addedUsers[ key ] = info;
	}
	
	// TODO what if it is already on 'added' list?
}

// ============================================================================
// User modified
void ClientThreadHubAnchor::wtUserModified( const RufusDc::UserInfo& rawInfo )
{
	QMutexLocker locker( & _userMutex );
	
	UserInfo info;
	info.convert( rawInfo, _pCodec );
	QString key = info.nick().toLower();
	
	//qDebug("ANCHOR: user modified: %s", qPrintable(key) );

	// remove from removed. this is excetional situation
	if ( _removedUsers.contains( key ) )
	{
		qWarning("User list inconsisten, user '%s' modified, while reported as removed",
			qPrintable( info.nick() ) );
		_removedUsers.remove( key );
	}
	
	// user still fresh to observer?
	if ( _addedUsers.contains( key ) )
	{
		_addedUsers[ key ] = info;
	}
	else
	{
		_modifiedUsers[ key ] = info;
	}
}

// ============================================================================
// User removed
void ClientThreadHubAnchor::wtUserRemoved( const std::string& rawNick )
{
	QMutexLocker locker( & _userMutex );
	
	QString nick;
	if ( _pCodec )
	{
		nick = _pCodec->toUnicode( rawNick.c_str() );
	}
	else
	{
		nick = rawNick.c_str();
	}
	
	QString key = nick.toLower();
	
	//qDebug("ANCHOR: user removed: %s", qPrintable(key) );
	
	// user just added?
	if ( _addedUsers.contains( key ) )
	{
		// let's pretend he never existed
		_addedUsers.remove( key );
	}
	else
	{
		_modifiedUsers.remove( key );
		_removedUsers.insert( key );
	}
}

// ============================================================================
// Get chabged users
void ClientThreadHubAnchor::getChangedUsers
	( QMap< QString, UserInfo >&  added
	, QMap< QString, UserInfo >&  modified
	, QSet< QString>&             removed
	)
{
	QMutexLocker locker( & _userMutex );

	added    = _addedUsers;
	modified = _modifiedUsers;
	removed  = _removedUsers;
	
	_addedUsers.clear();
	_modifiedUsers.clear();
	_removedUsers.clear();
}

// ============================================================================
// Returns list of users
QList<UserInfo> ClientThreadHubAnchor::getUsers()
{
	QMutexLocker locker ( & _userMutex );
	
	list<RufusDc::UserInfo> users = _pParent->hub()->getUsers();
	
	QList<UserInfo> result;
	QMap< QString, UserInfo > sort;
	
	BOOST_FOREACH( RufusDc::UserInfo& rawInfo, users )
	{
		UserInfo info;
		info.convert( rawInfo, _pCodec );
		sort[ info.nick().toLower() ] = info;
	}
	
	// clear counters
	_addedUsers.clear();
	_modifiedUsers.clear();
	_removedUsers.clear();
	
	return sort.values();
}


}

