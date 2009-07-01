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
#include "tabcontent.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
TabContent::TabContent( QWidget *parent )
		: QWidget( parent )
{
}

// ============================================================================
// Destructor
TabContent::~TabContent()
{
}

// ============================================================================
// Set tab title
void TabContent::setTabTitle( const QString& title )
{
	if ( title != _tabTitle )
	{
		_tabTitle = title;
		Q_EMIT titleChanged( title );
	}
}

// ============================================================================
// Set icon
void TabContent::setTabIcon( const QIcon& icon )
{
	_tabIcon = icon;
	Q_EMIT iconChanged( icon );
}

// ============================================================================
// Close tab
void TabContent::closeTab()
{
	deleteLater();
}

void TabContent::invoke
	( const char* method
	, QGenericArgument val0
	, QGenericArgument val1
	, QGenericArgument val2
	, QGenericArgument val3
	, QGenericArgument val4
	)
{
	QMetaObject::invokeMethod
		( this
		, method
		, Qt::QueuedConnection
		, val0, val1, val2, val3, val4
		);
}

}
