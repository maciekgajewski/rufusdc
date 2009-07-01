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

// Qt
#include <QThread>
#include <QAction>

// KDE
#include <KMenu>
#include <KIcon>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/Client.h>
#include <dcpp/ClientManager.h>
#include <dcpp/FavoriteManager.h>
#include <dcpp/HashManager.h>
#include <dcpp/SearchManager.h>
#include <dcpp/ShareManager.h>
#include <dcpp/UserCommand.h>

// local
#include "hubwidget.h"


//BEGIN debug routeines
#include <sys/time.h>
// ============================================================================
/// Returns current time in ms.
double getms()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	
	return (tv.tv_sec & 0xff ) * 1e3 + tv.tv_usec * 1e-3;
}
//END debug 

namespace KRufusDc
{

static const int USERLIST_POPULATE_DELAY = 1000; // delay between widget creation and user list population [ms]
static const int USERLIST_UPDATE_INTERVAL = 5000; // how often user list should be updated [ms]


// ============================================================================
// Constructor
HubWidget::HubWidget( QString address, QWidget* parent, Qt::WindowFlags /*f*/ )
	: TabContent( parent )
	, Ui::HubWidget()
	, _pClient( NULL )
{
	setupUi( this );
	
	_userUpdateTimer.setInterval( USERLIST_UPDATE_INTERVAL );
	
	connect( & _userUpdateTimer, SIGNAL(timeout()), SLOT(updateUsers()) );
	
	connect( pUsers, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(usersContextMenu(const QPoint &) ) );
	connect( pChatInput, SIGNAL(returnPressed()), SLOT(chatReturnPressed() ) );
	
	// give hub some time to get user data
	QTimer::singleShot( USERLIST_POPULATE_DELAY, this, SLOT(populateUsers()) );
	
	// visual properties of widget's tab
	setTabIcon( KIcon("network-workgroup") );
	generateColors();
	
	connectToHub( address );
	
	// TODO temporarly here, copied from populateUsers
	pUsers->setModel( & _userModel );

}

// ============================================================================
// Destructor
HubWidget::~HubWidget()
{
	// dsiconnect from speaker
	if ( _pClient )
	{
		_pClient->removeListener( this );
	}
}

// ============================================================================
// incoming system message
void HubWidget::addSystemMessage( const QString& msg )
{
	pChat->append( QString("<span style=\"color:%1\">%2</span>").arg( _systemMessageColor.name(), msg ) );
}

// ============================================================================
// incoming chat message
void HubWidget::addChatMessage( const QString& msg )
{
	QString txt = formatMessage( msg );
	pChat->append( QString("<p>")+txt+"</p>" );
}

// ============================================================================
// Format message
QString HubWidget::formatMessage( const QString& msg )
{
	QString formatted = msg;
	
	// 0. translate HTML special chars
	formatted.replace( "\n", "<br/>" );
	formatted.replace( " ", "&nbsp;" );
	
	// 2 - make all user's nick occurences bold
	QString nick = _pClient->getMyNick().c_str();
	QString regex = QString("(%1)").arg( QRegExp::escape( nick ) );
	QString replace = QString("<b><span style=\"color:%1\">\\1</span></b>").arg( _userNickColor.name() );
	formatted.replace( QRegExp( regex ), replace );
	
	// convert url's to actual links
	formatted.replace( QRegExp( "(https?://[._A-Za-z0-9-/%&\\?]+)" ), "<a href=\"\\1\">\\1</a>" );
	
	return formatted;
}

// ============================================================================
// Populate users
void HubWidget::populateUsers()
{
	/*
	QList<UserInfo> users = _pHub->anchor()->getUsers();
	_userModel.populate( users );
	
	pUsers->setModel( & _userModel );
	
	// resize columns to content
	for( int i = 0; i < _userModel.columnCount(); i++ )
	{
		pUsers->resizeColumnToContents( i );
	}
	*/
	_userUpdateTimer.start();
}

// ============================================================================
// Update users
void HubWidget::updateUsers()
{
	QMap< QString, UserInfo > added;
	QMap< QString, UserInfo > modified;
	QSet< QString>            removed;
	
	//_pHub->anchor()->getChangedUsers( added, modified, removed );
	int initialUserCount = _userModel.rowCount();
	//qDebug("Updatng users: added: %d, removed: %d, modified: %d. users on list: %d", added.size(), removed.size(), modified.size(), _userModel.rowCount() );
	
	//double start = getms();
	_userModel.update( added, modified, removed );
	//double end = getms();
	//qDebug("Updating users in %.2f ms", end-start );
	
	// if list was emty before ,reset view (why?)
	if ( ! initialUserCount )
	{
		pUsers->reset();
		// resize columns to content
		for( int i = 0; i < _userModel.columnCount(); i++ )
		{
			pUsers->resizeColumnToContents( i );
		}

	}
}

// ============================================================================
// Context menu
void HubWidget::usersContextMenu( const QPoint & pos )
{
	if ( pUsers->selectionModel() && pUsers->selectionModel()->hasSelection() )
	{
		// get selection
		int row = pUsers->selectionModel()->currentIndex().row();
		QString nick = _userModel.getUserInfo( row ).nick();
			
		// init menu
		QAction actionFileList( KIcon("view-list-tree"), i18n("Request file list"), this );
		KMenu menu( this );
		
		menu.addTitle( nick );
		menu.addAction( & actionFileList );
		
		QAction* pRes = menu.exec( pUsers->mapToGlobal( pos ) );
		
		// handle selection
		if ( pRes == & actionFileList )
		{
			requestFileList( nick );
		}
	}
}

// ============================================================================
// Request file list
void HubWidget::requestFileList( const QString& /*nick*/)
{
	//_pHub->requestFileList( nick );
	// TODO love me or leave me
}

// ============================================================================
// Generate colors
void HubWidget::generateColors()
{
	if ( palette().color( QPalette::Base ).value() > 128 )
	{
		// light background
		_systemMessageColor = Qt::darkRed;
		_userNickColor      = Qt::darkBlue;
		
	}
	else
	{
		// dark backgorund
		_systemMessageColor = Qt::red;
		_userNickColor      = Qt::blue;
	}
}

// ============================================================================
// Return pressed inchat
void HubWidget::chatReturnPressed()
{
	QString text = pChatInput->text();
	pChatInput->clear();
	
	//_pHub->sendChatMessage( text );
}

void HubWidget::connectToHub( const QString& address )
{
	Q_ASSERT( ! _pClient );
	
	_pClient = dcpp::ClientManager::getInstance()->getClient( qPrintable(address) );
	
	if ( _pClient )
	{
		_pClient->setEncoding("UTF-8"); // TODO pass
		_pClient->addListener(this);
		_pClient->connect();
		
		// set title to address
		setTabTitle( address );
	}
	else
	{
		deleteLater(); // abort, abort, abort!
	}
}

// ============================================================================
// User updated
void HubWidget::userUpdated( const UserInfo& info )
{
	// TODO quick and dirty
	QSet< QString > removed;
	QMap< QString, UserInfo > updated;
	updated[ info.nick() ] = info ;
	
	int initialUserCount = _userModel.rowCount();
	
	_userModel.update( updated, removed );
	
	// if list was empty before ,reset view (why?)
	if ( ! initialUserCount )
	{
		pUsers->reset();
		// resize columns to content
		for( int i = 0; i < _userModel.columnCount(); i++ )
		{
			pUsers->resizeColumnToContents( i );
		}

	}
}

// ============================================================================
// User removed
void HubWidget::userRemoved(const UserInfo& info )
{
	// TODO quick and dirty
	QSet< QString > removed;
	QMap< QString, UserInfo > updated;
	removed.insert( info.nick() );
	
	_userModel.update( updated, removed );
}

void HubWidget::on(dcpp::ClientListener::Connecting, dcpp::Client *) throw()
{
	// TODO use some visual indication instead, like throbber
	invoke( "addSystemMessage", Q_ARG( QString, i18n("Connecting") ) );
}

void HubWidget::on(dcpp::ClientListener::Connected, dcpp::Client * pClient) throw()
{
	//qDebug("HubWidget::on Connected");
	invoke( "addSystemMessage", Q_ARG( QString, i18n("Connected") ) );
	invoke( "setTabTitle", Q_ARG( QString, pClient->getHubName().c_str() ) );
}

void HubWidget::on(dcpp::ClientListener::UserUpdated, dcpp::Client *, const dcpp::OnlineUser &user) throw()
{
	UserInfo  info;
	info.fromDcppIdentity( user );

	invoke("userUpdated", Q_ARG( UserInfo, info ) );
}

void HubWidget::on(dcpp::ClientListener::UsersUpdated, dcpp::Client *, const dcpp::OnlineUserList &list) throw()
{
	//qDebug("HubWidget::on UsersUpdated, user count=%d", list.size() );
	
	for( dcpp::OnlineUserList::const_iterator it = list.begin(); it != list.end(); ++it )
	{
		UserInfo  info;
		dcpp::OnlineUser* pUser = *it;
		info.fromDcppIdentity( *pUser );
	
		// TODO add method that takes list in one shot
		invoke("userUpdated", Q_ARG( UserInfo, info ) );
	}
}

void HubWidget::on(dcpp::ClientListener::UserRemoved, dcpp::Client *, const dcpp::OnlineUser &user) throw()
{
	UserInfo  info;
	info.fromDcppIdentity( user );

	invoke("userRemoved", Q_ARG( UserInfo, info ) );
}

void HubWidget::on(dcpp::ClientListener::Redirect, dcpp::Client *, const std::string &address) throw()
{
	qDebug("HubWidget::on Redirected to: %s", address.c_str() );
}

void HubWidget::on(dcpp::ClientListener::Failed, dcpp::Client *, const std::string &reason) throw()
{
	//qDebug("HubWidget::on Failed");
	
	QString message = i18n("Connection failed: %1", reason.c_str() );
	
	invoke( "addSystemMessage", Q_ARG( QString, message ) );
}

void HubWidget::on(dcpp::ClientListener::GetPassword, dcpp::Client *) throw()
{
	qDebug("HubWidget::on GetPassword");
}

void HubWidget::on(dcpp::ClientListener::HubUpdated, dcpp::Client* pClient ) throw()
{
	invoke( "setTabTitle", Q_ARG( QString, pClient->getHubName().c_str() ) );
	// TODO update other data here if needed
}

void HubWidget::on
	(dcpp::ClientListener::Message
	, dcpp::Client *
	, const dcpp::OnlineUser &/*user*/
	, const std::string &message
	, bool /*thirdPerson*/
	) throw()
{
	invoke( "addChatMessage", Q_ARG( QString, message.c_str() ) ); // TODO use other data
}

void HubWidget::on(dcpp::ClientListener::StatusMessage, dcpp::Client *, const std::string &/*message*/, int /*flag*/) throw()
{
	qDebug("HubWidget::on StatusMessage");
}

void HubWidget::on(dcpp::ClientListener::PrivateMessage, dcpp::Client *, const dcpp::OnlineUser &/*from*/,
	const dcpp::OnlineUser &/*to*/, const dcpp::OnlineUser &/*replyTo*/, const std::string &/*message*/, bool /*thirdPerson*/) throw()
{
	qDebug("HubWidget::on PrivateMessage");
}

void HubWidget::on(dcpp::ClientListener::NickTaken, dcpp::Client *) throw()
{
	qDebug("HubWidget::on NickTaken");
}

void HubWidget::on(dcpp::ClientListener::SearchFlood, dcpp::Client *, const std::string &/*message*/) throw()
{
	qDebug("HubWidget::on SearchFlood");
}


} // namespace


// EOF

