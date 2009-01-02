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

#include <QPushButton>

#include "connectdialog.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
ConnectDialog::ConnectDialog( QWidget* parent, Qt::WindowFlags f )
	: QDialog( parent, f )
	, Ui::ConnectDialog()
{
	setupUi( this );
	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
	address->setHistoryItems( QStringList() << "hubdc.lanet.net.pl:4012" ); // for debugging
}

// ============================================================================
// Destructor
ConnectDialog::~ConnectDialog()
{
}

// ============================================================================
// Accept
void ConnectDialog::accept()
{
	QString addr = address->currentText();
	if ( validate( addr ) )
	{
		emit addressAccepted( addr );
		
		QDialog::accept();
		
		// TODO move this to some method called befor showing by MainWindow.
		// TODO this doesn't resolve 'Cancel' stealing focus from combo.
		address->clearEditText();
		address->setFocus();
	}
}

// ============================================================================
// Address changed
void ConnectDialog::addressChanged( const QString& addr )
{
	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( validate( addr ) );
}

// ============================================================================
// Validate
bool ConnectDialog::validate( const QString& addr )
{
	// stupid simple :)
	return ! addr.isEmpty();
}

}
