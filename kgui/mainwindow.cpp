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
#include <QCloseEvent>

// KDE
#include <KLocalizedString>
#include <KAction>
#include <KToolBar>
#include <KSystemTrayIcon>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/QueueManager.h>
#include <dcpp/Download.h>

// local
#include "connectdialog.h"
#include "hubwidget.h"
#include "filelistwidget.h"
#include "tabwidget.h"
#include "transferwidget.h"

#include "mainwindow.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
MainWindow::MainWindow( QWidget* pParent )
		: KMainWindow(pParent)
{
	initActions();
	initGui();
	
	resize( 800, 600 ); // TODO wild guess, use sessions?
	
	// init systray
	KSystemTrayIcon* pSystray = new KSystemTrayIcon
		( QApplication::windowIcon()
		, this
		);
	pSystray->show();
	
	dcpp::QueueManager::getInstance()->addListener(this);
}

// ============================================================================
// Destructor
MainWindow::~MainWindow()
{
	dcpp::QueueManager::getInstance()->removeListener(this);
}

// ============================================================================
// Init actions
void MainWindow::initActions()
{
	// == main toolbar ==
	
	// connect
	KAction* actionConnect = new KAction( this );
		actionConnect->setText( i18n("Connect") );
		actionConnect->setIcon( KIcon("network-connect") );
		actionConnect->setShortcut( Qt::CTRL + Qt::Key_C );
		connect( actionConnect, SIGNAL(triggered()), SLOT(onActionConnect()) );
		
	// transfers
	KAction* actionTransfers = new KAction( this );
		actionTransfers->setText( i18n("Transfers") );
		actionTransfers->setIcon( KIcon("folder-downloads") );
		actionTransfers->setShortcut( Qt::CTRL + Qt::Key_T );
		connect( actionTransfers, SIGNAL(triggered()), SLOT(onActionTransfers()) );
	
	
	toolBar("main")->addAction( actionConnect );
	toolBar("main")->addAction( actionTransfers );
}

// ============================================================================
// Invoke method across threads
void MainWindow::invoke
	( const char* method
	, QGenericArgument val0
	, QGenericArgument val1
	, QGenericArgument val2
	, QGenericArgument val3
	, QGenericArgument val4
	)
{
	QMetaObject::invokeMethod
		( this
		, method
		, Qt::QueuedConnection
		, val0, val1, val2, val3, val4
		);
}

// ============================================================================
// Connect action handler
void MainWindow::onActionConnect()
{
	if ( ! _pDialogConnect )
	{
		_pDialogConnect = new ConnectDialog( this );
		connect
			( _pDialogConnect
			, SIGNAL(addressAccepted(QString,QString))
			, SLOT(connectToHub(QString,QString)) );
	}
	
	_pDialogConnect->show();
}

// ============================================================================
// On action transfers
void MainWindow::onActionTransfers()
{
	if ( ! _pTransfer )
	{
		_pTransfer = new TransferWidget( this );
		int idx = _pTabs->addTab( _pTransfer );
		_pTabs->setCurrentIndex( idx );
	}
	else
	{
		_pTabs->setCurrentIndex( _pTabs->indexOf( _pTransfer ) );
	}
}

// ============================================================================
// Connect to hub
void MainWindow::connectToHub( const QString& addr, const QString& encoding )
{
	// TODO make sure the hub is unique
	// TODO parse addr, use wrapper object
	HubWidget* pWidget = new HubWidget( addr, encoding, this );
	_pTabs->addTab( pWidget );
	
}

// ============================================================================
// File list downloaded
void MainWindow::fileListDownloaded( const QString& path, const QString& cid )
{
	FileListWidget* pWidget = new FileListWidget( this );
	pWidget->loadFromFile( path, cid );
	int idx = _pTabs->addTab( pWidget );
	_pTabs->setCurrentIndex( idx );
}

// ============================================================================
// Initializes GUI elements
void MainWindow::initGui()
{
	_pTabs = new TabWidget( this );
	
	setCentralWidget( _pTabs );
}

// ============================================================================
// Close event
/*
void MainWindow::closeEvent( QCloseEvent * event )
{
	//hide();
	setWindowState( Qt::WindowMinimized );
	event->ignore();
}
*/

// ============================================================================
// query close
bool MainWindow::queryClose()
{
	// do not close, just hide. closing via systray icon menu 
	hide();
	return false;
}

// ============================================================================
// On downlopad finished
void MainWindow::on(dcpp::QueueManagerListener::Finished, dcpp::QueueItem* qi, const std::string&, int64_t size) throw()
{
	if ( qi->isSet( dcpp::QueueItem::FLAG_CLIENT_VIEW | dcpp::QueueItem::FLAG_USER_LIST) )
	{
		QString listName = QString::fromUtf8( qi->getListName().c_str() );
		
		dcpp::UserPtr user = qi->getDownloads()[0]->getUser();
		QString cid = user->getCID().toBase32().c_str();
		
		invoke("fileListDownloaded", Q_ARG( QString, listName ), Q_ARG( QString, cid ) );
	}
}


} // ns

// EOF


