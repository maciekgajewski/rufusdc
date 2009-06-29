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
#ifndef KRUFUSDCCONNECTDIALOG_H
#define KRUFUSDCCONNECTDIALOG_H

#include <QDialog>

#include "ui_connectdialog.h"

namespace KRufusDc
{

/**
* @brief "Connect to hub" dialog.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class ConnectDialog : public QDialog, private Ui::ConnectDialog
{
	Q_OBJECT

public:
	ConnectDialog( QWidget* pParent = NULL , Qt::WindowFlags f = 0 );
	virtual ~ConnectDialog();
	
	virtual void accept();
	
Q_SIGNALS:
	
	/// Called when dialog is accepted with address.
	void addressAccepted( const QString& str );
	
private Q_SLOTS:

	/// Handles text change. validates and enabled OK button if valid
	void addressChanged( const QString& addr );

private:

	/// Validates address
	bool validate( const QString& addr );
	
};

}

#endif // KRUFUSDCCONNECTDIALOG_H

// EOF


