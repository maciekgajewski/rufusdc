// Copyright (C) 2009 Maciek Gajewski <maciej.gajewski0@gmail.com>
// Uses code from Linux DC++, Copyright © 2004-2008 Jens Oknelid, paskharen@gmail.com 
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
#include <dcpp/QueueManager.h>
#include <dcpp/UploadManager.h>
#include <dcpp/FinishedManager.h>
#include <dcpp/SearchManager.h>

// local
#include "mainwindow.h"
#include "searchquery.h"

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

// ============================================================================
// Downloads file list
void ClientThread::downloadFileList( const QString& cid )
{
	dcpp::UserPtr user = dcpp::ClientManager::getInstance()->findUser(dcpp::CID( cid.toStdString() ));
			
	if (user)
	{
		if (user == dcpp::ClientManager::getInstance()->getMe())
		{
			// Don't download file list, open locally instead
			// TODO
			qDebug("TODO: open own file list");
		}
		else
		{
			dcpp::QueueManager::getInstance()->addList(user, std::string(), dcpp::QueueItem::FLAG_CLIENT_VIEW);
		}
	}
}

// ============================================================================
// Match queue
void ClientThread::matchQueue( const QString& cid )
{
	dcpp::UserPtr user = dcpp::ClientManager::getInstance()->findUser(dcpp::CID( cid.toStdString() ));
			
	if (user)
	{
		if (user == dcpp::ClientManager::getInstance()->getMe())
		{
			// Don't download file list, open locally instead
			// TODO
			qDebug("TODO: open own file list");
		}
		else
		{
			dcpp::QueueManager::getInstance()->addList(user, std::string(), dcpp::QueueItem::FLAG_MATCH_QUEUE);
			// empty string above is a hub hint
		}
	}
}

// ============================================================================
// Grant extra slot
void ClientThread::grantSlot( const QString& cid )
{
	dcpp::UserPtr user = dcpp::ClientManager::getInstance()->findUser(dcpp::CID( cid.toStdString() ));
	if (user)
	{
		dcpp::UploadManager::getInstance()->reserveSlot( user, std::string() ); // empty string is a hub hint
	}
}

// ============================================================================
// Removes user from queue
void ClientThread::removeUserFromQueue( const QString& cid )
{
	dcpp::UserPtr user = dcpp::ClientManager::getInstance()->findUser(dcpp::CID( cid.toStdString() ));
	if (user)
	{
		dcpp::QueueManager::getInstance()->removeSource(user, dcpp::QueueItem::Source::FLAG_REMOVED);
	}
	else
	{
		qDebug("ClientThread::removeUserFromQueue: can't find user!");
	}
}

// ============================================================================
// Cance ldownload
void ClientThread::cancelDownload( const QString& path )
{
	dcpp::QueueManager::getInstance()->remove( path.toUtf8().data() ) ;
}

// ============================================================================
// Remove finished download
void ClientThread::removeFinishedDownload( const QString& path )
{
	dcpp::FinishedManager::getInstance()->remove( false, path.toUtf8().data() );
	qDebug("client thread: file removed: %s", path.toUtf8().data() );
}

// ============================================================================
// Remove all finished downloads
void ClientThread::removeAllFinishedDownloads()
{
	dcpp::FinishedManager::getInstance()->removeAll( false ); // false=downloads
	qDebug("Client thread- removed all");
}

// ============================================================================
// search for alternates
void ClientThread::searchForAlternates( const QString& tth )
{
	std::string name = tth.toUtf8().data();
	
	dcpp::SearchManager::getInstance()->search
		( name	// name
		, 0		// size
		, dcpp::SearchManager::TYPE_TTH // type mode (?)
		, dcpp::SearchManager::SIZE_DONTCARE // size mode
		, "ras" // TODO use random token
		);
}

// ============================================================================
// Search 
void ClientThread::search( const SearchQuery& query )
{
	dcpp::SearchManager::getInstance()->search
		( query.text().toUtf8().data()
		, query.size()
		, dcpp::SearchManager::TypeModes( query.type() )
		, dcpp::SearchManager::SizeModes( query.sizeMethod() )
		, query.token().toUtf8().data()
		);
}

} // ns

// EOF

