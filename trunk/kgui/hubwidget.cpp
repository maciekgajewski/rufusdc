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
#include <QThread>

#include "hub.h"

#include "hubwidget.h"

namespace KRufusDc
{

static const int USERLIST_POPULATE_DELAY = 3000; // delay between widget creation and user list population [ms]
static const int USERLIST_UPDATE_INTERVAL = 3000; // how often user list should be updated [ms]


// ============================================================================
// Constructor
HubWidget::HubWidget( Hub* pHub, QWidget* parent, Qt::WindowFlags f )
	: QWidget( parent, f )
	, Ui::HubWidget()
	, _pHub( pHub )
{
	Q_ASSERT( pHub );
	
	setupUi( this );
	
	_userUpdateTimer.setInterval( USERLIST_UPDATE_INTERVAL );
	
	connect( pHub, SIGNAL( hubMessage(int,QString)), SLOT( onHubMessage(int,QString) ) );
	connect( & _userUpdateTimer, SIGNAL(timeout()), SLOT(updateUsers()) );
	
	// give hub some time to get user data
	QTimer::singleShot( USERLIST_POPULATE_DELAY, this, SLOT(populateUsers()) );
}

// ============================================================================
// Destructor
HubWidget::~HubWidget()
{
}

// ============================================================================
// incoming message
void HubWidget::onHubMessage( int type, const QString& msg )
{
	//qDebug("msg: tid:%d type:%d %s", int(QThread::currentThreadId()), type, qPrintable( msg ) );
	
	if ( type == Hub::System )
	{
		pChat->setTextColor( Qt::darkRed );
	}
	else
	{
		pChat->setTextColor( Qt::black ); // TODO use palette here
	}
	pChat->append( msg );
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
	QSet< QString>             removed;
	
	_pHub->anchor()->getChangedUsers( added, modified, removed );
	
	//qDebug("Updatng users: added: %d, removed: %d, modieifed: %d", added.size(), removed.size(), modified.size() );
	
	_userModel.update( added, modified, removed );
	
	// resize columns to content
	for( int i = 0; i < _userModel.columnCount(); i++ )
	{
		pUsers->resizeColumnToContents( i );
	}
}

}
