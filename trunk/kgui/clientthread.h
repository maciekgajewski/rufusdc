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
#ifndef KRUFUSDCCLIENTTHREAD_H
#define KRUFUSDCCLIENTTHREAD_H

#include <boost/asio/deadline_timer.hpp>

#include <QThread>

namespace RufusDc
{
	class Client;
	class Hub;
}


namespace KRufusDc
{

/**
* @brief Thread runing RufusDc::Client
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ClientThread : public QThread
{
	Q_OBJECT

public:

	// Hub message type
	enum MessageType
	{
		System, ///< System message
		Chat    ///< Chat message
	};

	ClientThread( QObject *parent = 0 );
	virtual ~ClientThread();
	
	/**
	 * @brief Access to RufusDc::Client
	 * @return reference to underlying client object
	 */
	RufusDc::Client& client() { return *_pClient; }
	
public slots:

	// cross-thread slots
	void slotConnectHub( const QString& addr );
	void slotDisconnectHub( const QString& addr );
	
	/// Runs client form within the message loop
	void runClient();
	
	/// Stops client ant hte entire thread
	void stopClient();

protected:

	/// Thread's routine
	virtual void run();
	
private slots:

private:

	/// Called by ASIO timer
	void onTimer();
	
private:

	RufusDc::Client* _pClient;
	
	boost::asio::deadline_timer* _pTimer;
	
	bool _stopped; ///< STOP flag

};

} // namespcae

#endif // KRUFUSDCCLIENTTHREAD_H

// EOF


