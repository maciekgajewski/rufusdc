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

class ClientThreadHub;

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
	
	// TODO I don't like it. IT should be hidden
	ClientThreadHub* anchor() const  { return _thread; }
	
	/// Returns hub address
	QString address() const { return _address; }
	
	/// Return hub name
	QString name() const { return _name; }
	
	/// Returns hub topic
	QString topic() const { return _topic; }
	
	/**
	 * @brief Requestst file list for user.
	 * @param nick user's nick
	 */
	void requestFileList( const QString& nick );
	
	/**
	 * @brief Sends chat message to hub.
	 * @param msg message text, w/o any header.
	 */
	void sendChatMessage( const QString& msg );

Q_SIGNALS:

	// public signals

	/// Emitted when there is new chat message incoming
	void hubMessage( int type, const QString& msg );
	
	/// Emitted when hub name changes
	void hubNameChanged( const QString& name );
	
	/// Emitted when hub topic changes
	void hubTopicChanged( const QString& topic );
	
	// signals sent to anchor
	
	void wtRequestFileList( const QString& nick );
	
	void wtSendChatMessage( const QString& msg );

private Q_SLOTS:

	// x-thread slots, connected to anchor
	
	void wtMessage( int type, const QString& msg );
	
	void wtNameChanged( const QString& name );
	void wtTopicChanged( const QString& topic );

private:

	/// Private constructor. Use Client to obtain instance.
	Hub( const QString& addr, Client *parent );
	
	/// Parent
	Client* _pParent;
	
	/// Cached hub address
	QString _address;
	
	/// Cached hub name, as sent by hub
	QString _name;
	
	/// Cached hub topic
	QString _topic;
	
	ClientThreadHub* _thread; ///< Sub-object living in worker thread
};

}

#endif // KRUFUSDCHUB_H

// EOF


