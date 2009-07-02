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

#include "tabwidget.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
TabWidget::TabWidget( QWidget* parent, Qt::WindowFlags f )
	: KTabWidget( parent, f )
{
	setCloseButtonEnabled( true );
	setTabReorderingEnabled( true );
	
	connect( this, SIGNAL(closeRequest(QWidget*)), SLOT(tabCloseRequest( QWidget*) ) );
}

// ============================================================================
// Destructor
TabWidget::~TabWidget()
{
}

// ============================================================================
// Add tab
int TabWidget::addTab( TabContent* pTab )
{
	Q_ASSERT( pTab );
	
	connect( pTab, SIGNAL(titleChanged(const QString&)), SLOT(tabTitleChanged(const QString&)) );
	connect( pTab, SIGNAL(iconChanged(const QIcon&)), SLOT(tabIconChanged(const QIcon&)) );
	
	return QTabWidget::addTab( static_cast<QWidget*>( pTab ), pTab->tabIcon(), pTab->tabTitle() );
}

// ============================================================================
// tab close
void TabWidget::tabCloseRequest( QWidget* pTab )
{
	Q_ASSERT( pTab );
	
	TabContent* pTabContent = qobject_cast<TabContent*>( pTab );
	
	if ( pTabContent )
	{
		pTabContent->closeTab();
		
		// remove tab
		int idx = indexOf( pTabContent );
		if ( idx >= 0  )
		{
			removeTab( idx );
		}
	}
	else
	{
		// unknown type
		pTab->deleteLater();
	}
}

// ============================================================================
// tab text changed
void TabWidget::tabTitleChanged( const QString& title )
{
	QWidget* pTab = qobject_cast<QWidget*>( sender() );
	
	if ( pTab )
	{
		int index = indexOf( pTab );
		if ( index >= 0 )
		{
			setTabText( index, title );
		}
	}
}

// ============================================================================
// tab icon changed
void TabWidget::tabIconChanged( const QIcon& icon )
{
	QWidget* pTab = qobject_cast<QWidget*>( sender() );
	
	if ( pTab )
	{
		int index = indexOf( pTab );
		if ( index >= 0 )
		{
			setTabIcon( index, icon );
		}
	}
}


}
