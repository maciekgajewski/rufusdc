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
#ifndef KRUFUSDCTABWIDGET_H
#define KRUFUSDCTABWIDGET_H

#include <KTabWidget>

namespace KRufusDc
{

class TabContent;

/**
* Tab widget holding TabContent widgets.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class TabWidget : public KTabWidget
{
	Q_OBJECT

public:
	TabWidget( QWidget* parent = NULL, Qt::WindowFlags f = 0 );
	virtual ~TabWidget();
	
	/// Adds tab.
	///@param pTab tab widget
	///@return tab index
	int addTab( TabContent* pTab );
	
private Q_SLOTS:

	/// Tab close request
	void tabCloseRequest( QWidget* pTab );
	
	/// Handles tab title change
	void tabTitleChanged( const QString& title );
	
	/// Handles tab icon change
	void tabIconChanged( const QIcon& icon );

};

}

#endif // KRUFUSDCTABWIDGET_H

// EOF


