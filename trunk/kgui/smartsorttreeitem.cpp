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

// Qt
#include <QHeaderView>

// local
#include "smartsorttreeitem.h"

namespace KRufusDc
{

static const int SORT_ROLE = 118273; ///< some random number used as sort role

// ============================================================================
// Constructors
SmartSortTreeItem::SmartSortTreeItem(QTreeWidgetItem* parent, QTreeWidgetItem* preceding, int type): QTreeWidgetItem(parent, preceding, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(QTreeWidgetItem* parent, const QStringList& strings, int type): QTreeWidgetItem(parent, strings, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(QTreeWidgetItem* parent, int type): QTreeWidgetItem(parent, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(QTreeWidget* view, QTreeWidgetItem* after, int type): QTreeWidgetItem(view, after, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(QTreeWidget* view, const QStringList& strings, int type): QTreeWidgetItem(view, strings, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(QTreeWidget* view, int type): QTreeWidgetItem(view, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(const QStringList& strings, int type): QTreeWidgetItem(strings, type)
{
}


SmartSortTreeItem::SmartSortTreeItem(int type): QTreeWidgetItem(type)
{
}


SmartSortTreeItem::~SmartSortTreeItem()
{
}

// ============================================================================
// set valus
void SmartSortTreeItem::setValue( int column, const QVariant& data )
{
	setData( column, SORT_ROLE, data );
}

// ============================================================================
// Comparision
bool SmartSortTreeItem::operator< ( const QTreeWidgetItem& other ) const
{
	int column = treeWidget()->header()->sortIndicatorSection();
	
	QVariant valueThis = data( column, SORT_ROLE );
	QVariant valueThat = other.data( column, SORT_ROLE );
	
	// if any of value is null, fallback to original implementation
	if ( valueThis.isNull() || valueThat.isNull() )
	{
		return QTreeWidgetItem::operator<( other );
	}
	
	// try string comparision
	if ( valueThis.type() == QMetaType::QString && valueThat.type() == QMetaType::QString )
	{
		QString stringThis = valueThis.toString();
		QString stringThat = valueThat.toString();
		
		return stringThis < stringThat;
	}
	
	// try numerical comparision
	if ( valueThis.canConvert<double>() && valueThat.canConvert<double>() )
	{
		double dblThis = valueThis.toDouble();
		double dblThat = valueThat.toDouble();
		
		return dblThis < dblThat;
	}

	// fall back to base implementation
	return QTreeWidgetItem::operator<( other );
}

} // namespace

// EOF

