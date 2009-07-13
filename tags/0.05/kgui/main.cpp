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

#include <stdexcept>
#include <iostream>

// KDE
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <QTimer>
#include <QThreadPool>

#include "client.h"
#include "mainwindow.h"


// ============================================================================
// main
int main(int argc, char** argv )
{
	KAboutData aboutData
		( "KRufusDC"                  // app name
		, 0                           // translation dir
		, ki18n("KRufusDC")           // human-readable app name
		, "0.1"                       // version
		, ki18n("DC++ client")        // short description
		, KAboutData::License_GPL_V2  // License
		, ki18n("(c) 2008 Maciej Gajewski") // copyright statement
		);
	
	KCmdLineArgs::init( argc, argv, &aboutData );
	
	KApplication app;
	KRufusDc::Client client; // client implementation

	// stop thread from thread pool,
	// TODO this is possible workaround
	QThreadPool::globalInstance()->setMaxThreadCount( 0 );
	
	KRufusDc::MainWindow* pWin = new KRufusDc::MainWindow( &client );
	pWin->show();

	QTimer::singleShot( 0, &client, SLOT(start()) ); // start client from within event loop	

	// intercept app quit event
	QObject::connect( &app, SIGNAL(aboutToQuit()), &client, SLOT(stop()) );

	try
	{
		return app.exec();
	}
	catch( const std::exception& e )
	{
		std::cerr << "Runaway exception: " << e.what() << std::endl;
	}
}

// EOF

