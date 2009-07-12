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

// Qt
#include <QAction>
#include <QClipboard>
#include <QApplication>

// KDE
#include <KLocalizedString>
#include <KIcon>

// local
#include "clientthread.h"
#include "actionfactory.h"

namespace KRufusDc
{
	
/// Singleton instance
ActionFactory* ActionFactory::_pInstance = NULL;

// ============================================================================
// init instance
void ActionFactory::initInstance()
{
	if ( ! _pInstance )
	{
		_pInstance = new ActionFactory();
	}
}

// ============================================================================
// Creates user-related action
QAction* ActionFactory::createUserAction( QObject* pParent, const UserInfo& info, UserAction type )
{
	initInstance(); // make sure instance is created
	
	QAction* pAction = new QAction( pParent );
	
	// bind data to action
	BoundData data;
	data.type = type;
	data.data = QVariant::fromValue( info );
	
	_pInstance->_binding[ pAction ] = data;
	connect( pAction, SIGNAL(destroyed( QObject* )), _pInstance, SLOT(actionDeleted( QObject* )) );
	connect( pAction, SIGNAL(triggered()), _pInstance, SLOT(userActionTriggered()) );
	
	
	switch( type )
	{
		case CopyNick:
			pAction->setText( i18n("Copy nick") );
			pAction->setIcon( KIcon("edit-copy") );
			pAction->setToolTip( i18n("Copy user's nick to clipboard") );
			break;
			
		case GetFileList:
			pAction->setText( i18n("Request file list") );
			pAction->setIcon( KIcon("view-list-tree") );
			pAction->setToolTip( i18n("Request and browse user's file list") );
			break;
			
		case MatchQueue:
			pAction->setText( i18n("Match queue") );
			pAction->setIcon( KIcon("document-previewy") );
			pAction->setToolTip( i18n("Match user's file list with download queue") );
			break;
			
		case GrantSlot:
			pAction->setText( i18n("Grant extra slot") );
			pAction->setIcon( KIcon("bookmark-new-list") );
			pAction->setToolTip( i18n("Grant user extra slot") );
			break;
			
		case RemoveUserFromQueue:
			pAction->setText( i18n("Remove from queue") );
			pAction->setIcon( KIcon("edit-delete") );
			pAction->setToolTip( i18n("Remove user from download queue") );
			break;
	
		default:
			qWarning("Unknown action type!");
	}
	
	return pAction;
}

// ============================================================================
// Create download action
QAction* ActionFactory::createDownloadAction( QObject* pParent, const DownloadInfo& info, DownloadAction type )
{
	initInstance(); // make sure instance is created
	
	QAction* pAction = new QAction( pParent );
	
	// bind data to action
	BoundData data;
	data.type = type;
	data.data = QVariant::fromValue( info );
	
	_pInstance->_binding[ pAction ] = data;
	connect( pAction, SIGNAL(destroyed( QObject* )), _pInstance, SLOT(actionDeleted( QObject* )) );
	connect( pAction, SIGNAL(triggered()), _pInstance, SLOT(downloadActionTriggered()) );
	
	
	switch( type )
	{
		case CancelDownload:
			pAction->setText( i18n("Cancel download") );
			pAction->setIcon( KIcon("edit-delete") );
			pAction->setToolTip( i18n("Cancel download") );
			break;
			
		case SearchAlternates:
			pAction->setText( i18n("Search for alternates") );
			pAction->setIcon( KIcon("edit-find") );
			pAction->setToolTip( i18n("Search for alternate sources") );
			break;
			
		default:
			qWarning("Unknown action type!");
	}
	
	return pAction;
}

// ============================================================================
// Action deleted
void ActionFactory::actionDeleted( QObject* pAction )
{
	_binding.remove( pAction );
}

// ============================================================================
// user action triggered
void ActionFactory::userActionTriggered()
{
	QObject* pAction = sender();
	if ( ! pAction )
	{
		qWarning("ActionFactory::userActionTriggere: can't find action!");
		return;
	}
	
	if ( ! _binding.contains( pAction ) )
	{
		qWarning("ActionFactory::userActionTriggere: can't find bindings for action!");
	}
	
	BoundData& data = _binding[ pAction ];
	UserInfo info = data.data.value< UserInfo >();
	
	switch( data.type )
	{
		case CopyNick:
			QApplication::clipboard()->setText( info.nick() );
			break;
			
		case GetFileList:
			ClientThread::invoke( "downloadFileList", Q_ARG( QString, info.cid() ) );
			break;
			
		case MatchQueue:
			ClientThread::invoke( "matchQueue", Q_ARG( QString, info.cid() ) );
			//addSystemMessage( i18n("Matching download queue with %1's file list").arg( info.nick() ) );
			break;
			
		case GrantSlot:
			ClientThread::invoke( "grantSlot", Q_ARG( QString, info.cid() ) );
			//addSystemMessage( i18n("Granting extra slot to %1").arg( info.nick() ) );
			break;
			
		case RemoveUserFromQueue:
			ClientThread::invoke( "removeUserFromQueue", Q_ARG( QString, info.cid() ) );
			//addSystemMessage( i18n("User %1 removed from queue").arg( info.nick() ) );
			break;
	
		default:
			qWarning("ActionFactory::userActionTriggered: Unknown action type!");
	}
}

// ============================================================================
// Download acton triggered
void ActionFactory::downloadActionTriggered()
{
	QObject* pAction = sender();
	if ( ! pAction )
	{
		qWarning("ActionFactory::userActionTriggere: can't find action!");
		return;
	}
	
	if ( ! _binding.contains( pAction ) )
	{
		qWarning("ActionFactory::userActionTriggere: can't find bindings for action!");
	}
	
	BoundData& data = _binding[ pAction ];
	DownloadInfo info = data.data.value< DownloadInfo >();
	
	switch( data.type )
	{
		case CancelDownload:
			ClientThread::invoke("cancelDownload", Q_ARG( QString, info.path() ) );
			break;
			
		case SearchAlternates:
			ClientThread::invoke("searchForAlternates", Q_ARG( QString, info.TTH() ) );
			break;
			
		default:
			qWarning("Unknown action type!");
	}
	
}

} // namespace

// EOF


