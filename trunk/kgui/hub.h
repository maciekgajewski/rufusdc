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
#ifndef KRUFUSDCHUB_H
#define KRUFUSDCHUB_H

#include <string>
#include <boost/shared_ptr.hpp>

#include <QObject>
#include <QString>
#include <QMap>
#include <QSet>
#include <QMutex>

#include "rufusdc/userinfo.h"

#include "userinfo.h"

namespace RufusDc
{
	class Hub;
}

namespace KRufusDc
{

using namespace boost;

class Client;

class ClientThreadHubAnchor;

/**
* @brief Qt wrapper aorund RufusDc::Hub.
* The wrapper lives in the same thread as Client, while actual Hub implementation lives in seperate thread.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Hub : public QObject
{
	Q_OBJECT
	friend class Client;

public:

	enum MessageType
	{
		System,  ///< System message
		Chat     ///< Chat message
	};

	~Hub();
	
	/// Connects hub
	void connect();
	
	/// Disconnects hub
	void disconnect();
	
	/// Parent Client
	Client* parent() const { return _pParent; }
	
	/// Hub implementation
	shared_ptr<RufusDc::Hub>& hub() { return _pHub; }
	
	// TODO I don't like it. IT shopuld be hidden
	ClientThreadHubAnchor* anchor() const  { return _thread; }

signals:

	/// Emitted when there is new chat message incoming
	void hubMessage( int type, const QString& msg );

private slots:

	void wtMessage( int type, const QString& msg );

private:

	/// Private constructor. Use Client to obtain instance.
	Hub( const shared_ptr<RufusDc::Hub>& pHub, Client *parent );
	
	/// Underlying RufusDc::Hub object
	shared_ptr<RufusDc::Hub> _pHub;
	
	/// Parent
	Client* _pParent;
	
	/// Cached hub address
	QString _address;
	
	ClientThreadHubAnchor* _thread; ///< Sub-object living in worker thread
};

/// Subobject living in worker thread
class ClientThreadHubAnchor : public QObject
{
Q_OBJECT

public:
	ClientThreadHubAnchor( Hub* parent, QObject* qparent = 0 );
	
	/// Sets encoding used to convert incoming messages to QString's unicode.
	void setHubEncoding( const QByteArray& name );
	
	/// Returns all users currently connected to hub
	/// @returns User list sorted by nick (case-insensitive)
	QList<UserInfo> getUsers();

	/// Gets changed user containers. Clears internal buffers
	void getChangedUsers
		( QMap< QString, UserInfo >&   added
		, QMap< QString, UserInfo >&   modified
		, QSet< QString>&               removed
		);
	
	// boost slots
	
	/// Boost slot - called from within worker thread
	void wtMessage( int type, const std::string& msg );
	
	void wtUserAdded( const RufusDc::UserInfo& info );
	void wtUserModified( const RufusDc::UserInfo& info );
	void wtUserRemoved( const std::string& nick );
	
signals:
	/// Boost slot - called from within worker thread
	void signalWtMessage( int type, const QString& msg );

private:

	QTextCodec* _pCodec; ///< Text coded used to decode incoming messages
	
	Hub* _pParent; ///< Parent
	
	// user change tracking
	QMap< QString, UserInfo > _addedUsers;
	QMap< QString, UserInfo > _modifiedUsers;
	QSet< QString> _removedUsers;
	
	QMutex _userMutex;
};


}

#endif // KRUFUSDCHUB_H

// EOF


