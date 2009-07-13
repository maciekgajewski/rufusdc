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

// boost
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/signal.hpp>

// Qt
#include <QTextCodec>

// rufusdc
#include "rufusdc/hubmanager.h"

// local
#include "hub.h"
#include "client.h"
#include "clientthread.h"
#include "clientthreadhub.h"

namespace KRufusDc
{

// ============================================================================
// Thread anchor constructor
ClientThreadHub::ClientThreadHub( Client* pParent, const QString& addr, QObject* qparent )
	: QObject(qparent)
	, _pCodec(0)
	, _pParent( pParent )
	, _address( addr )
{
	Q_ASSERT( pParent );

	// connect to client's thread
	ClientThread& thread = _pParent->clientThread();
}


// ============================================================================
// Destructor
ClientThreadHub::~ClientThreadHub()
{
}

// ============================================================================
// Start
void ClientThreadHub::start()
{
	_pHub = RufusDc::HubManager::ref().getHub( qPrintable( _address ) );
	
	Q_ASSERT( _pHub );
	
	// connect the object with actual hub implementation
	_pHub->signalChatMessage.connect( boost::bind( &ClientThreadHub::wtMessage, this, Hub::Chat, _1 ) );
	_pHub->signalSystemMessage.connect( boost::bind( &ClientThreadHub::wtMessage, this, Hub::System, _1 ) );
	
	_pHub->signalUserAdded.connect( boost::bind( &ClientThreadHub::wtUserAdded, this, _1 ) );
	_pHub->signalUserModified.connect( boost::bind( &ClientThreadHub::wtUserModified, this, _1 ) );
	_pHub->signalUserRemoved.connect( boost::bind( &ClientThreadHub::wtUserRemoved, this, _1 ) );
	
	_pHub->signalNameChanged.connect( boost::bind( &ClientThreadHub::wtHubNameChanged, this, _1 ) );
	_pHub->signalTopicChanged.connect( boost::bind( &ClientThreadHub::wtHubTopicChanged, this, _1 ) );
}

// ============================================================================
// other thread wt message
void ClientThreadHub::wtMessage( int type, const std::string& msg )
{
	if ( _pCodec )
	{
		Q_EMIT signalWtMessage( type, _pCodec->toUnicode( msg.c_str() ) );
	}
	else
	{
		Q_EMIT signalWtMessage( type, msg.c_str() );
	}
}

// ============================================================================
// Sets encoding
void ClientThreadHub::setHubEncoding( const QByteArray& name )
{
	_pCodec = QTextCodec::codecForName( name );
}

// ============================================================================
// User added
void ClientThreadHub::wtUserAdded( const RufusDc::UserInfo& rawInfo )
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
void ClientThreadHub::wtUserModified( const RufusDc::UserInfo& rawInfo )
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
void ClientThreadHub::wtUserRemoved( const std::string& rawNick )
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
void ClientThreadHub::getChangedUsers
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
QList<UserInfo> ClientThreadHub::getUsers()
{
	QMutexLocker locker ( & _userMutex );
	
	std::list<RufusDc::UserInfo> users = _pHub->getUsers();
	
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

// ============================================================================
// Requests file list
void ClientThreadHub::utRequestFileList( const QString& nick )
{
	QByteArray rawNick = _pCodec ? _pCodec->fromUnicode( nick ) : nick.toLocal8Bit();
	
	_pHub->requestFileList( rawNick.data() );
}

// ============================================================================
// Send chat message
void ClientThreadHub::utSendChatMessage( const QString& msg )
{
	QByteArray rawMsg = _pCodec ? _pCodec->fromUnicode( msg ) : msg.toLocal8Bit();
	
	_pHub->sendChatMessage( rawMsg.data() );
}

// ============================================================================
// Name changed
void ClientThreadHub::wtHubNameChanged( const std::string& name )
{
	if ( _pCodec )
	{
		Q_EMIT signalWtNameChanged( _pCodec->toUnicode( name.c_str() ) );
	}
	else
	{
		Q_EMIT signalWtNameChanged( name.c_str() );
	}
}

// ============================================================================
// Topic changed
void ClientThreadHub::wtHubTopicChanged( const std::string& topic )
{
	if ( _pCodec )
	{
		Q_EMIT signalWtTopicChanged( _pCodec->toUnicode( topic.c_str() ) );
	}
	else
	{
		Q_EMIT signalWtTopicChanged( topic.c_str() );
	}
}

}
