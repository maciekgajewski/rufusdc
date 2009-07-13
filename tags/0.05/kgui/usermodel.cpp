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

// KDE
#include <KLocalizedString>
#include <KIcon>

// local
#include "utils.h"
#include "usermodel.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
UserModel::UserModel( QObject *parent )
		: QAbstractTableModel( parent )
{
}

// ============================================================================
// Destructor
UserModel::~UserModel()
{
}

// ============================================================================
// Flags
Qt::ItemFlags UserModel::flags ( const QModelIndex& /*index*/ ) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

// ============================================================================
// item data
QVariant UserModel::data ( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
{
	// say no to non-top-level items
	if ( index.parent().isValid() )
	{
		return QVariant();
	}

	// text
	if ( role == Qt::DisplayRole )
	{
		const UserInfo& info = _users[ index.row() ];
		
		switch( index.column() )
		{
			case 0: return info.nick();
			case 1: return sizeToString( info.sharesize() );
			case 2: return info.connection();
			case 3: return info.description();
		}
	}
	// icon
	if ( role == Qt::DecorationRole && index.column() == 0 )
	{
		return KIcon("user-online");
	}
	
	// unknown
	return QVariant();
}

// ============================================================================
// Header data
QVariant UserModel::headerData ( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const
{
	if ( role == Qt::DisplayRole )
	{
		switch( section )
		{
			case 0: return i18n("Nick");
			case 1: return i18n("shared");
			case 2: return i18n("connection");
			case 3: return i18n("description");
		}
	}
	
	return QVariant();
}

// ============================================================================
// Number of rows
int UserModel::rowCount ( const QModelIndex & parent /*= QModelIndex()*/ ) const
{
	if ( parent.isValid() )
	{
		return 0;
	}
	
	return _users.size();
}

// ============================================================================
// Number of columns
int UserModel::columnCount( const QModelIndex & parent /*= QModelIndex()*/ ) const
{
	if ( parent.isValid() )
	{
		return 4; // TODO use defined costant here
	}
	
	return _users.size();
}

// ============================================================================
// Update data
void UserModel::update 
		( const QMap< QString, UserInfo >&  added
		, const QMap< QString, UserInfo >&  modified
		, const QSet< QString>&             removed
		)
{
	
	// first - modify!
	modifyUsers( modified );
	
	// remove removed
	removeUsers( removed );
	// now index is worthless crap
	
	// finally - add
	addUsers( added );
	
	// rebuild index
	rebuildIndex();	
}

// ============================================================================
// Rewbuilds index
void UserModel::rebuildIndex()
{
	_index.clear();
	for( int i = 0 ; i < _users.size(); i++ )
	{
		_index[ _users[i].nick().toLower() ] = i;
	}
}

// ============================================================================
// Populates data
void UserModel::populate( const QList<UserInfo>& data )
{
	_users = data;
	
	rebuildIndex();
}

// ============================================================================
// add users
void UserModel::addUsers( const QMap< QString, UserInfo >& added )
{
	// NOTE: don';t use index here, it's invalid now
	
	int firstRow = 0;
	int addedRows = 0;
	
	QMap< QString, UserInfo >::const_iterator firstInserted = added.begin();
	
	// insert before existing elements
	QList< UserInfo >::iterator existingit = _users.begin();
	
	while( existingit != _users.end() && firstInserted != added.end() )
	{
		QString key = (*existingit).nick().toLower();
		
		if ( key < firstInserted.key() )
		{
			//qDebug("adding: %s can't be added before %s(%d)"
			//	, qPrintable( firstInserted.key() ), qPrintable( key ), firstRow );
			++existingit;
			++firstRow;
			continue;
		}
		else
		{
			//qDebug("adding: %s can be added before %s (%d)"
			//	, qPrintable( firstInserted.key() ), qPrintable( key ), firstRow );
			// find last inserted now
			QMap< QString, UserInfo >::const_iterator lastInserted = firstInserted + 1;
			int lastRow = firstRow;
			
			while( lastInserted != added.end() && key > lastInserted.key() )
			{
				//qDebug("adding: %s also can be added before %s, as %d"
				//	, qPrintable( lastInserted.key() ), qPrintable( key ), lastRow+1 );
				++lastInserted;
				++lastRow;
			}
			lastInserted;
			
			// now insert
			//qDebug("Inserting %d rows before %s", lastRow-firstRow + 1, qPrintable( key ) );
			beginInsertRows( QModelIndex(), firstRow, lastRow );
				while( firstInserted != lastInserted )
				{
					existingit = _users.insert( existingit, firstInserted.value() );
					++existingit;
					++firstInserted;
					++firstRow;
					++addedRows;
				}
			endInsertRows();
		}
	}
	
	// append all remaining items at the end
	if ( firstInserted != added.end() )
	{
		int firstAppended = _users.size();
		int lastAppended = _users.size() + added.size() - addedRows - 1;
		
		//qDebug("adding: %d records remaining, append at the end as rows %d-%d", added.size() - addedRows,
		//	firstAppended, lastAppended );
		beginInsertRows( QModelIndex(), firstAppended, lastAppended );
		while( firstInserted != added.end() )
		{
			_users.append( firstInserted.value() );
			++firstInserted;
		}
		endInsertRows();
	}
	
	
	
	/* TODO old. new implementation needed
	QMap< QString, UserInfo >::const_iterator addedit;
	QList< UserInfo >::iterator existingit = _users.begin();
	int row = 0;
	for( addedit = added.begin(); addedit != added.end(); ++addedit )
	{
		if ( existingit != _users.end() )
		{
			while( existingit != _users.end() && (*existingit).nick().toLower() < addedit.key() )
			{
				++row;
				++existingit;
			}
		}
			
		if (  existingit != _users.end() )
		{
			// insert before
			//qDebug("User %s inserted at row %d, before user %s", qPrintable(addedit.key())
			//	, row, qPrintable( (*existingit).nick() ) );
				
			beginInsertRows( QModelIndex(), row, row );
				existingit = _users.insert( existingit, addedit.value() );
				++row;
				++existingit;
			endInsertRows();
		}
		else
		{
			//qDebug("User %s appended to list", qPrintable(addedit.key()) );
			
			beginInsertRows( QModelIndex(), _users.size(), _users.size() );
				_users.append( addedit.value() );
			endInsertRows();
			++addedit;
		}
	}
	*/
	
}

// ============================================================================
// modify users
void UserModel::modifyUsers( const QMap< QString, UserInfo >& modified )
{
	//Q_FOREACH( QString nick, modified.keys() ) // TODO suboptimal ?
	for( QMap< QString, UserInfo >::const_iterator it = modified.begin(); it != modified.end(); ++it )
	{
		// find by index
		QMap< QString, int >::iterator indexit = _index.find( it.key() );
		if ( indexit == _index.end() )
		{
			qWarning("Unknown user %s mark as modified", qPrintable(it.key()) );
			continue;
		}
		int idx = indexit.value();
		
		_users[ idx ] = it.value(); // change!
		
		// signalize change
		QModelIndex topLeft = index( idx, 0 );
		QModelIndex bottomRight = index( idx, columnCount() );
		
		Q_EMIT dataChanged( topLeft, bottomRight );
	}
}

// ============================================================================
// remove users
void UserModel::removeUsers( const QSet< QString>& removed )
{
	int numRemoved = 0;
	Q_FOREACH( QString nick, removed )
	{
		// find by index
		QMap< QString, int >::iterator indexit = _index.find( nick );
		if ( indexit == _index.end() )
		{
			qWarning("Unknown user %s mark as removed", qPrintable(nick) );
			continue;
		}
		int idx = indexit.value() - numRemoved; // simple correction to index
		
		//qDebug("User %s removed, row: %d", qPrintable(nick), idx );
		beginRemoveRows( QModelIndex(), idx, idx );
			_users.removeAt( idx );
		endRemoveRows();
		
		numRemoved++;
	}
}

}
