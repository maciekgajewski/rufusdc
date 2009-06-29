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
#ifndef KRUFUSDCMAINWINDOW_H
#define KRUFUSDCMAINWINDOW_H

// boost
#include <boost/shared_ptr.hpp>

// Qt
#include <QPointer>

// KDE
#include <KMainWindow>


namespace KRufusDc
{

	class ConnectDialog;
	class TabWidget;

/**
* @brief Apps main window
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class MainWindow : public KMainWindow
{
	Q_OBJECT
public:
	MainWindow( QWidget* pParent = NULL );
	virtual ~MainWindow();
	
private Q_SLOTS:

	// action handlers
	
	void onActionConnect();   ///< Connect action
	
	// other slots
	
	void connectToHub( const QString& str ); ///< Connect to hub requested
	
	// messages from client
	
	/// File list was received
	void fileListReceived( /*const boost::shared_ptr<RufusDc::FileList>& pFileList*/ );

private:
	
	// initializers
	
	/// Initializes actions
	void initActions();
	
	/// Initalizes GUI elements
	void initGui();

	
	// dialogs
	
	QPointer<ConnectDialog> _pDialogConnect;  ///< "Connect to hub" dialog
	
	// widgets
	
	TabWidget* _pTabs;
	
	
};

}

#endif // KRUFUSDCMAINWINDOW_H

// EOF


