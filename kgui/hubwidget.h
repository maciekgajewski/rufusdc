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
#include <QWidget>
#include <QTimer>
class QAction;

// local
#include "usermodel.h"
#include "ui_hubwidget.h"

namespace KRufusDc
{

class Hub;

/**
* @brief Displays HUB status.
* The widget displays data in two panels: chat messages in left panel, and user list in right panel.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class HubWidget : public QWidget, private Ui::HubWidget // with all due respect to Marc Mutz
{
	Q_OBJECT

public:
	HubWidget( Hub* pHub, QWidget* parent, Qt::WindowFlags f = 0 );
	virtual ~HubWidget();
	
private Q_SLOTS:

	void onHubMessage( int type, const QString& msg );
	
	/// Periodically updates user list
	void updateUsers();
	
	/// Initialies user list
	void populateUsers();
	
	/// Pos up context menu on users lists
	void usersContextMenu( const QPoint & pos );
	
private:
	
	Hub*       _pHub;      ///< Hub wrapper
	UserModel  _userModel; ///< Data model for user list
	
	/// Timer used to periodically update user list
	QTimer    _userUpdateTimer;
	
	QAction* _pActionFileList;
	
	/// Request file list from user
	void requestFileList( const QString& nick );

};

}
#endif // KRUFUSDCHUBWIDGET_H

// EOF


