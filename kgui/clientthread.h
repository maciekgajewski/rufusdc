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
#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

// Qt
#include <QThread>

namespace KRufusDc
{

class MainWindow;

/**
* Client thread. Performs client-related actions in separate thread.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ClientThread : public QObject
{
Q_OBJECT
public:
    
	ClientThread( MainWindow* pMainWindow );
	virtual ~ClientThread();

	/// Starts client thread
	void start();
	
	/// Stops client thread
	void stop();
	
	/// invokes slot in client thread
	/// Calls method in client thread. Method must be a slot
	///@param method method name, w/o signature.
	static void invoke
		( const char* method
		, QGenericArgument val0 = QGenericArgument( 0 )
		, QGenericArgument val1 = QGenericArgument()
		, QGenericArgument val2 = QGenericArgument()
		, QGenericArgument val3 = QGenericArgument()
		, QGenericArgument val4 = QGenericArgument()
		);
	
private Q_SLOTS: // call these via 'invoke'

	/// Starts active client
	void startListening();
	
	/// Auto-connects to favorite hubs
	void autoConnect();

private:

	/// Pointer to apps main window
	MainWindow* _pMainWindow;
	
	/// Actual thread
	QThread	_thread;
	
	/// Static instance pointer
	static ClientThread* _pInstance;
};

} // namespace

#endif // CLIENTTHREAD_H
