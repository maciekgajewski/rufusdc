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

// rufusdc
#include "rufusdc/client.h"

// local
#include "hub.h"
#include "clientthreadhub.h"
#include "client.h"

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
HubWidget::HubWidget( Hub* pHub, QWidget* parent, Qt::WindowFlags f )
	: TabContent( parent )
	, Ui::HubWidget()
	, _pHub( pHub )
{
	Q_ASSERT( pHub );
	
	setupUi( this );
	
	_userUpdateTimer.setInterval( USERLIST_UPDATE_INTERVAL );
	
	connect( pHub, SIGNAL( hubMessage(int,QString)), SLOT( onHubMessage(int,QString) ) );
	connect( pHub, SIGNAL( hubNameChanged(QString)), SLOT( updateTitle() ) );
	connect( pHub, SIGNAL( hubTopicChanged(QString)), SLOT( updateTitle() ) );
	connect( & _userUpdateTimer, SIGNAL(timeout()), SLOT(updateUsers()) );
	
	connect( pUsers, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(usersContextMenu(const QPoint &) ) );
	connect( pChatInput, SIGNAL(returnPressed()), SLOT(chatReturnPressed() ) );
	
	// give hub some time to get user data
	QTimer::singleShot( USERLIST_POPULATE_DELAY, this, SLOT(populateUsers()) );
	
	// visual properties of widget's tab
	updateTitle();
	setTabIcon( KIcon("network-workgroup") );
	generateColors();
}

// ============================================================================
// Destructor
HubWidget::~HubWidget()
{	
}

// ============================================================================
// Update title
void HubWidget::updateTitle()
{
	QString title;
	
	// do we have name?
	if ( _pHub->name().isEmpty() )
	{
		// no, use just title
		title =  _pHub->address();
	}
	else
	{
		// use topic if available
		if ( _pHub->topic().isEmpty() )
		{
			title = _pHub->name();
		}
		else
		{
			title = QString("%1 (%2)").arg( _pHub->name(), _pHub->topic() );
		}
	}
	
	setTabTitle( title );
}

// ============================================================================
// incoming message
void HubWidget::onHubMessage( int type, const QString& msg )
{
	//qDebug("msg: tid:%d type:%d %s", int(QThread::currentThreadId()), type, qPrintable( msg ) );
	if ( type == Hub::System )
	{
		pChat->append( QString("<span style=\"color:%1\">%2</span>").arg( _systemMessageColor.name(), msg ) );
	}
	else
	{
		QString txt = formatMessage( msg );
		pChat->append( txt );
	}
}

// ============================================================================
// Format message
QString HubWidget::formatMessage( const QString& msg )
{
	QString formatted = msg;
	
	// 0. translate HTML special chars
	formatted.replace( "\n", "<br/>" );
	formatted.replace( " ", "&nbsp;" );
	
	// 1 - make '<nick>' ... at the beginning bold
	formatted.replace( QRegExp("^<([^>]*)>"), "<b>&lt;\\1&gt;</b>" );
	
	// 2 - make all user's nick occurences bold
	QString regex = QString("(%1)").arg( QRegExp::escape( _pHub->parent()->clientThread().client().settings().nick.c_str() ) );
	QString replace = QString("<b><span style=\"color:%1\">\\1</span></b>").arg( _userNickColor.name() );
	formatted.replace( QRegExp( regex ), replace );
	
	// convert url's to actual links
	formatted.replace( QRegExp( "(https?://[._A-Za-z0-9-/]+)" ), "<a href=\"\\1\">\\1</a>" );
	
	return formatted;
}

// ============================================================================
// Populate users
void HubWidget::populateUsers()
{
	QList<UserInfo> users = _pHub->anchor()->getUsers();
	_userModel.populate( users );
	
	pUsers->setModel( & _userModel );
	
	// resize columns to content
	for( int i = 0; i < _userModel.columnCount(); i++ )
	{
		pUsers->resizeColumnToContents( i );
	}
	
	_userUpdateTimer.start();
}

// ============================================================================
// Update users
void HubWidget::updateUsers()
{
	QMap< QString, UserInfo > added;
	QMap< QString, UserInfo > modified;
	QSet< QString>            removed;
	
	_pHub->anchor()->getChangedUsers( added, modified, removed );
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
void HubWidget::requestFileList( const QString& nick)
{
	_pHub->requestFileList( nick );
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
	
	_pHub->sendChatMessage( text );
}

}
