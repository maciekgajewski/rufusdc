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
#ifndef KRUFUSDCACTIONSFACTORY_H
#define KRUFUSDCACTIONSFACTORY_H

// Qt
#include <QObject>
#include <QMap>
#include <QVariant>
class QAction;

// local
#include "userinfo.h"

namespace KRufusDc
{

/**
* @brief Factory of common UI actions
* Also servers as QOabject containing slot handling these action, and mapping QActions to params
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ActionFactory : public QObject
{
	Q_OBJECT
public:
	
	/// Actions related with user (UserInfo)
	enum UserAction
	{
		CopyNick,		///< Copy user nick
		GetFileList,	///< Request file list
		MatchQueue,		///< Match queue
		GrantSlot,		///< Grant slot
		RemoveUserFromQueue	///< Remove user from queue
	};
	
	/// Creates action
	///@param pParent parent object
	///@param type action type
	static QAction* createUserAction( QObject* pParent, const UserInfo& info, UserAction type );

private Q_SLOTS:	// user-related actions

	/// Removes mapping for action
	void actionDeleted( QObject* pAction );

	/// Handles user action trigger
	void userActionTriggered();

private: // data

	/// Data bound to action
	struct BoundData
	{
		int type;		///< Action type
		QVariant data;	///< auxiliary data
	};
	
	/// Map of actions->data bindings
	QMap< QObject*, BoundData > _binding;
	// oh god, why it just can't be done as easily as with {std::tr1|boost}::bind ???

	/// Hidden constructor
	ActionFactory() {}

	/// Singleton instance
	static ActionFactory* _pInstance;
	
	/// Initializes singleton instance
	///@warning ot thread safe, use only from GUI thread
	static void initInstance();

};

} // ns

#endif // KRUFUSDCACTIONSFACTORY_H

// EOF


