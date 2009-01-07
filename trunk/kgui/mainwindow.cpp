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

#include <KLocalizedString>
#include <KAction>
#include <KToolBar>
#include <KTabWidget>

#include "hub.h"
#include "client.h"
#include "connectdialog.h"
#include "hubwidget.h"
#include "filelistwidget.h"

#include "mainwindow.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
MainWindow::MainWindow( Client* pClient, QWidget* pParent )
		: KMainWindow(pParent)
		, _pClient( pClient )
{
	Q_ASSERT( pClient);
	
	initActions();
	initGui();
	
	resize( 800, 600 );
	
	// connect to client
	connect
		( pClient
		, SIGNAL(signalFileListReceived( const boost::shared_ptr<RufusDc::FileList>& ))
		, SLOT(fileListReceived( const boost::shared_ptr<RufusDc::FileList>&))
		);
}

// ============================================================================
// Destructor
MainWindow::~MainWindow()
{
}

// ============================================================================
// Init actions
void MainWindow::initActions()
{
	// connect
	KAction* actionConnect = new KAction( this );
		actionConnect->setText( i18n("Connect") );
		actionConnect->setIcon( KIcon("network-connect") );
		actionConnect->setShortcut( Qt::CTRL + Qt::Key_C );
		connect( actionConnect, SIGNAL(triggered()), SLOT(onActionConnect()) );

	toolBar("main")->addAction( actionConnect );
}

// ============================================================================
// Connect action handler
void MainWindow::onActionConnect()
{
	if ( ! _pDialogConnect )
	{
		_pDialogConnect = new ConnectDialog( this );
		connect( _pDialogConnect, SIGNAL(addressAccepted(QString)), SLOT(connectToHub(QString)) );
	}
	
	_pDialogConnect->show();
}

// ============================================================================
// Connect to hub
void MainWindow::connectToHub( const QString& str )
{
	Q_ASSERT( _pClient );
	
	Hub* pHub = _pClient->createHub( str );
	HubWidget* pWidget = new HubWidget( pHub, this );
	pHub->connect();
	int index = _pTabs->addTab( pWidget, str );
	_pTabs->setCurrentIndex( index );
}

// ============================================================================
// Initializes GUI elements
void MainWindow::initGui()
{
	_pTabs = new KTabWidget( this );
	setCentralWidget( _pTabs );
}

// ============================================================================
// file list received
void MainWindow::fileListReceived( const boost::shared_ptr<RufusDc::FileList>& pFileList )
{
	FileListWidget* pWidget = new FileListWidget( this );
	pWidget->setFileList( pFileList );
	
	QString title = QString( i18n("%1's file list")).arg( pFileList->nick().c_str() );
	
	int index = _pTabs->addTab( pWidget, title );
	_pTabs->setCurrentIndex( index );
}

}
