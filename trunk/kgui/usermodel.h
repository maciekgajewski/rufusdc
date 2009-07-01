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
#ifndef KRUFUSDCUSERMODEL_H
#define KRUFUSDCUSERMODEL_H

// Qt
#include <QAbstractTableModel>
#include <QMap>
#include <QSet>

// local
#include "userinfo.h"

namespace KRufusDc
{

/**
* @brief Data model for user list control.
* Call update() to update bulk data.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class UserModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	UserModel( QObject *parent = 0 );
	~UserModel();
	
	/// intializes data
	void populate( const QList<UserInfo>& data );
	
	/// Updates data dynamically
	///@param added users added since last update
	///@param modified users modified since last update
	///@param removed users removed since last update
	void update 
		( const QMap< QString, UserInfo >&   added
		, const QMap< QString, UserInfo >&   modified
		, const QSet< QString>&              removed
		);
	
	/// Updates data dynamically
	///@param users added or modified since last update
	///@param removed users removed since last update
	void update 
		( const QMap< QString, UserInfo >&   updated
		, const QSet< QString>&              removed
		);
	/// direct access to data
	const UserInfo& getUserInfo( int row ) const { return _users[ row ]; }
	
	// model duties
	
	/// Item flags
	virtual Qt::ItemFlags flags ( const QModelIndex& index ) const;
	
	/// Item data
	virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
	
	/// Header data
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/// Row count
	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	
	/// Column count
	virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

private:

	/// Data
	QList< UserInfo > _users;
	
	QMap< QString, int > _index; ///< Index for fast lookup by nick.
	
	void rebuildIndex(); ///< Rebuild index
	
	// sub-updates
	
	/// Adds users to list
	void addUsers( const QMap< QString, UserInfo >& added );
	
	/// Modiefies users on the list
	void modifyUsers( const QMap< QString, UserInfo >& modified );
	
	/// Removes users from list
	void removeUsers( const QSet< QString>& removed );

};

}

#endif // KRUFUSDCUSERMODEL_H

// EOF


