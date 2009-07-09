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
#ifndef KRUFUSDCSMARTSORTTREEITEM_H
#define KRUFUSDCSMARTSORTTREEITEM_H

// Qt
#include <QTreeWidgetItem>

namespace KRufusDc {

/**
* QTreeWidgetITem that holds additional data for each column, which is used
* to determine sort order
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class SmartSortTreeItem : public QTreeWidgetItem
{
public:

	// constructors 
	SmartSortTreeItem(QTreeWidgetItem* parent, QTreeWidgetItem* preceding, int type = Type );
	SmartSortTreeItem(QTreeWidgetItem* parent, const QStringList& strings, int type = Type);
	SmartSortTreeItem(QTreeWidgetItem* parent, int type = Type);
	SmartSortTreeItem(QTreeWidget* view, QTreeWidgetItem* after, int type = Type);
	SmartSortTreeItem(QTreeWidget* view, const QStringList& strings, int type = Type);
	SmartSortTreeItem(QTreeWidget* view, int type = Type);
	SmartSortTreeItem(const QStringList& strings, int type = Type);
	SmartSortTreeItem(int type = Type);
	virtual ~SmartSortTreeItem();

	/// Sets sort value for column
	///@param coulmn coulm number
	///@param data
	void setValue( int column, const QVariant& data );

	/// Coparision operator, overloaded
	virtual bool operator< ( const QTreeWidgetItem & other ) const;
};

}

#endif
