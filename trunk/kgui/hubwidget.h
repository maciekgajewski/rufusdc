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
#ifndef KRUFUSDCHUBWIDGET_H
#define KRUFUSDCHUBWIDGET_H

// Qt
#include <QTimer>
class QAction;

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/Client.h>

// local
#include "tabcontent.h"
#include "usermodel.h"
#include "userinfo.h"

#include "ui_hubwidget.h"

namespace KRufusDc
{

/**
* @brief Displays HUB status.
* The widget displays data in two panels: chat messages in left panel, and user list in right panel.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class HubWidget
	: public TabContent
	, private Ui::HubWidget // with all due respect to Marc Mutz
	, public dcpp::ClientListener
{
	Q_OBJECT

public:
	/// Constructor
	///@param address hub address
	///@param parent parent widget
	///@param f window flags
	HubWidget( QString address, QWidget* parent, Qt::WindowFlags f = 0 );
	virtual ~HubWidget();
	
private Q_SLOTS: // x-threads messages

	/// Adds system message to chat window
	void addSystemMessage( const QString& msg );
	
	/// Adds chat message to chat window
	void addChatMessage( const QString& msg );
	
	/// User updated
	void userUpdated( const UserInfo& info );
	
	/// User removed
	void userRemoved(const UserInfo& info );
	
private Q_SLOTS: // UI events

	/// Pos up context menu on users lists
	void usersContextMenu( const QPoint & pos );
	
	/// Returns pressed in chat line
	void chatReturnPressed();
	
	/// Periodically updates user list
	void updateUsers();
	
	/// Initializes user list
	void populateUsers();

private: // Client callbacks

	virtual void on(dcpp::ClientListener::Connecting, dcpp::Client *) throw();
	virtual void on(dcpp::ClientListener::Connected, dcpp::Client *) throw();
	virtual void on(dcpp::ClientListener::UserUpdated, dcpp::Client *, const dcpp::OnlineUser &user) throw();
	virtual void on(dcpp::ClientListener::UsersUpdated, dcpp::Client *, const dcpp::OnlineUserList &list) throw();
	virtual void on(dcpp::ClientListener::UserRemoved, dcpp::Client *, const dcpp::OnlineUser &user) throw();
	virtual void on(dcpp::ClientListener::Redirect, dcpp::Client *, const std::string &address) throw();
	virtual void on(dcpp::ClientListener::Failed, dcpp::Client *, const std::string &reason) throw();
	virtual void on(dcpp::ClientListener::GetPassword, dcpp::Client *) throw();
	virtual void on(dcpp::ClientListener::HubUpdated, dcpp::Client *) throw();
	virtual void on(dcpp::ClientListener::Message, dcpp::Client *, const dcpp::OnlineUser &user, const std::string &message, bool thirdPerson) throw();
	virtual void on(dcpp::ClientListener::StatusMessage, dcpp::Client *, const std::string &message, int flag) throw();
	virtual void on(dcpp::ClientListener::PrivateMessage, dcpp::Client *, const dcpp::OnlineUser &from,
		const dcpp::OnlineUser &to, const dcpp::OnlineUser &replyTo, const std::string &message, bool thirdPerson) throw();
	virtual void on(dcpp::ClientListener::NickTaken, dcpp::Client *) throw();
	virtual void on(dcpp::ClientListener::SearchFlood, dcpp::Client *, const std::string &message) throw();

private: // methods

	/// Request file list from user
	void requestFileList( const QString& nick );
	
	/// Generates color mathcing current pallette
	void generateColors();
	
	/// Connects to hub
	void connectToHub( const QString& address );
	
	/// Fromats incoming message, inserting html tags
	QString formatMessage( const QString& msg );

private: // data
	
	/// Associated DCPP object
	dcpp::Client *_pClient;
	
	/// Data model for user list
	UserModel  _userModel;
	
	/// Timer used to periodically update user list
	QTimer    _userUpdateTimer;
	
	QAction* _pActionFileList;
	
	// colors
	QColor   _systemMessageColor;
	QColor   _userNickColor;
	
};

}
#endif // KRUFUSDCHUBWIDGET_H

// EOF


