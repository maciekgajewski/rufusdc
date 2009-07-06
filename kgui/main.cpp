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
#include <signal.h>

// KDE
#include <KUniqueApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <QTimer>
#include <QThreadPool>

// dcpp
#include "dcpp/stdinc.h"
#include "dcpp/DCPlusPlus.h"
#include "dcpp/TimerManager.h"

// local
#include "userinfo.h"
#include "mainwindow.h"
#include "clientthread.h"
#include "transferinfo.h"
#include "downloadinfo.h"

// TODO use another facility
void callBack(void* , const std::string& a)
{
	std::cerr << "Loading: " << a << std::endl;
}

// ============================================================================
// main
int main(int argc, char** argv )
{
	// meta-types registartion
	qRegisterMetaType<KRufusDc::UserInfo> ("UserInfo");
	qRegisterMetaType<KRufusDc::TransferInfo> ("TransferInfo");
	qRegisterMetaType<KRufusDc::DownloadInfo> ("DownloadInfo");
	
	// initialize KDE app
	KAboutData aboutData
		( "KRufusDC"                  // app name
		, 0                           // translation dir
		, ki18n("KRufusDC")           // human-readable app name
		, "0.1"                       // version
		, ki18n("DC++ client")        // short description
		, KAboutData::License_GPL_V2  // License
		, ki18n("(c) 2008 Maciej Gajewski") // copyright statement
		, ki18n("RufusDC is a KDE4 DC++ client")
		, "http://code.google.com/p/rufusdc/"
		);
	
	KCmdLineArgs::init( argc, argv, &aboutData );
	
	// create app
	KUniqueApplication app;

	// load icon
	QIcon appIcon;
	appIcon.addFile(":/resources/logo128.png", QSize(128, 128 ) );
	appIcon.addFile(":/resources/logo32.png", QSize(32, 32 ) );
	appIcon.addFile(":/resources/logo16.png", QSize(16, 16 ) );
	app.setWindowIcon( appIcon );

	KRufusDc::MainWindow* pWin = new KRufusDc::MainWindow();
	
	KRufusDc::ClientThread clientThread( pWin );
	
	pWin->show();

	try
	{
		// Start the DC++ client core
		dcpp::startup(callBack, NULL);
		dcpp::TimerManager::getInstance()->start();
		::signal(SIGPIPE, SIG_IGN);
		
		clientThread.start();
		
		KRufusDc::ClientThread::invoke("startListening");
		KRufusDc::ClientThread::invoke("autoConnect");
		
		int result = app.exec();
		std::cerr << "Shutting down..." << std::endl;
		
		clientThread.stop();
		dcpp::shutdown();
		
		return result;
	}
	catch( const std::exception& e )
	{
		std::cerr << "Runaway exception: " << e.what() << std::endl;
	}
	
	return -1;
}

// EOF

