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

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/QueueManager.h>

namespace KRufusDc
{

	class ConnectDialog;
	class TabWidget;
	class TransferWidget;

/**
* @brief Apps main window
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class MainWindow : public KMainWindow
	, public dcpp::QueueManagerListener
{
	Q_OBJECT
public:
	MainWindow( QWidget* pParent = NULL );
	virtual ~MainWindow();

	/// Calls method from worker thread. Method must be a slot
	///@param method method name, w/o signature.
	void invoke
		( const char* method
		, QGenericArgument val0 = QGenericArgument( 0 )
		, QGenericArgument val1 = QGenericArgument()
		, QGenericArgument val2 = QGenericArgument()
		, QGenericArgument val3 = QGenericArgument()
		, QGenericArgument val4 = QGenericArgument()
		);

private Q_SLOTS:

	// action handlers
	
	void onActionConnect();   	///< Connect action
	void onActionTransfers();	///< Show transfers action
	
	// other slots
	
	/// Connect to hub requested
	void connectToHub( const QString& str, const QString& encoding );

	/// File list downloaded
	///@param path path to file list file
	void fileListDownloaded( const QString& path );

protected: // event handlers

	/// Close event handlers - hides window instead of closing it
	//virtual void closeEvent( QCloseEvent * event ); // TODO trying queryClose instead
	
protected: // KMainWindow

	/// Asks wheter window can be closed
	virtual bool queryClose();

protected: // DC++ event handlers

	// QueueManager
	virtual void on(dcpp::QueueManagerListener::Finished, dcpp::QueueItem* qi, const std::string&, int64_t size) throw();

private:
	
	// initializers
	
	/// Initializes actions
	void initActions();
	
	/// Initalizes GUI elements
	void initGui();

	
	// dialogs
	
	QPointer<ConnectDialog> _pDialogConnect;  ///< "Connect to hub" dialog
	
	// widgets
	
	TabWidget* _pTabs;	///< Main tab widget
	
	// fixed tabs
	
	QPointer<TransferWidget> _pTransfer;	///< Transfers
	
	
};

}

#endif // KRUFUSDCMAINWINDOW_H

// EOF


