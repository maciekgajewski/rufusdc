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
#ifndef KRUFUSDCCLIENT_H
#define KRUFUSDCCLIENT_H

#include <QObject>

#include "clientthread.h"

namespace KRufusDc
{

class Hub;

/**
* @brief Qt wrapper around RufusDc::Client.
* Runs the RufusDc::Client in seperate thread, and comunicates with it using cross-thread
* messages.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Client : public QObject
{
	Q_OBJECT

public:
	Client( QObject *parent = 0 );
	virtual ~Client();

	// interface
	
	/**
	 * @brief Clreates Hub wrapper.
	 * @param addr hub address
	 * @return created Hub wrapper.
	 */
	Hub* createHub( const QString addr );
	
	/// Connects hub
	void connectHub( const QString& addr );
	
	/// Disconnects hub
	void disconnectHub( const QString& addr );
	
	/// Returns pointer to client worker thread
	ClientThread& clientThread() { return _thread; }

public Q_SLOTS:
	/**
	 * @brief Starts the client.
	 * Start this before doing anything else, bu from within apps event loop
	 */
	void start();
	
	
	/**
	 * @brief Stops the client.
	 * It stops undelrying thread. Call this before destroying client.
	 */
	void stop();
	
Q_SIGNALS:

	// cross-threads signals

	void signalConnectHub( const QString& addr );
	void signalDisconnectHub( const QString& addr );
	void startThread();
	void stopThread();

private:

	ClientThread _thread;
};

}

#endif // KRUFUSDCCLIENT_H

// EOF


