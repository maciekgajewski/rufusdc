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
		ClientThreadHubAnchor( QObject* parent = 0 ) : QObject(parent) {}
		
		/// Boost slot - called from within worker thread
		void wtMessage( int type, const std::string& msg );
	signals:
		/// Boost slot - called from within worker thread
		void signalWtMessage( int type, const QString& msg );
};

}

#endif // KRUFUSDCHUB_H

// EOF


