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

// dc++
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/FavoriteManager.h>
#include <dcpp/ShareManager.h>
#include <dcpp/ClientManager.h>
#include <dcpp/HubEntry.h>
#include <dcpp/ConnectionManager.h>

// local
#include "mainwindow.h"
#include "clientthread.h"

namespace KRufusDc
{

ClientThread* ClientThread::_pInstance = NULL;

// ============================================================================
// Constructor
ClientThread::ClientThread( MainWindow* pMainWindow )
 : QObject()
 , _pMainWindow( pMainWindow )
{
	Q_ASSERT( ! _pInstance );
	_pInstance = this;
}

// ============================================================================
// Destructor
ClientThread::~ClientThread()
{
	_pInstance = NULL;
}

// ============================================================================
// Start
void ClientThread::start()
{
	_thread.start();
	moveToThread( &_thread );
}

// ============================================================================
// Stop
void ClientThread::stop()
{
	_thread.quit();
	_thread.wait(); // TODO add timeout
}

// ============================================================================
// Invoke method across threads
void ClientThread::invoke
	( const char* method
	, QGenericArgument val0
	, QGenericArgument val1
	, QGenericArgument val2
	, QGenericArgument val3
	, QGenericArgument val4
	)
{
	Q_ASSERT( _pInstance );
	
	QMetaObject::invokeMethod
		( _pInstance
		, method
		, Qt::QueuedConnection
		, val0, val1, val2, val3, val4
		);
}

// ============================================================================
// Start listening
void ClientThread::startListening()
{
	dcpp::SearchManager::getInstance()->disconnect();
	dcpp::ConnectionManager::getInstance()->disconnect();

	if (dcpp::ClientManager::getInstance()->isActive())
	{
		try
		{
			dcpp::ConnectionManager::getInstance()->listen();
		}
		catch (const dcpp::Exception &e)
		{
			// TODO let user know somehow in GUI
			/*
			string primaryText = _("Unable to open TCP/TLS port");
			string secondaryText = _("File transfers will not work correctly until you change settings or turn off any application that might be using the TCP/TLS port.");
			typedef Func2<MainWindow, string, string> F2;
			F2* func = new F2(this, &MainWindow::showMessageDialog_gui, primaryText, secondaryText);
			WulforManager::get()->dispatchGuiFunc(func);
			*/
			qDebug("Unable to open TCP/TLS port");
		}

		try
		{
			dcpp::SearchManager::getInstance()->listen();
		}
		catch (const dcpp::Exception &e)
		{
			// TODO let user kow somehow in GUI
			/*
			string primaryText = _("Unable to open UDP port");
			string secondaryText = _("Searching will not work correctly until you change settings or turn off any application that might be using the UDP port.");
			typedef Func2<MainWindow, string, string> F2;
			F2* func = new F2(this, &MainWindow::showMessageDialog_gui, primaryText, secondaryText);
			WulforManager::get()->dispatchGuiFunc(func);
			*/
			qDebug("Unable to open UDP port");
		}
	}

	dcpp::ClientManager::getInstance()->infoUpdated();
}

// ============================================================================
// auto connect
void ClientThread::autoConnect()
{
	dcpp::FavoriteHubEntry *hub;
	dcpp::FavoriteHubEntryList &l = dcpp::FavoriteManager::getInstance()->getFavoriteHubs();

	for (dcpp::FavoriteHubEntryList::const_iterator it = l.begin(); it != l.end(); ++it)
	{
		hub = *it;

		if (hub->getConnect())
		{
			QString addr     = hub->getServer().c_str();
			QString encoding = hub->getEncoding().c_str();
			
			_pMainWindow->invoke("connectToHub", Q_ARG( QString, addr), Q_ARG( QString, encoding ) );
		}
	}
}

} // ns

// EOF

