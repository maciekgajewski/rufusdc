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
#ifndef KRUFUSDCFILELISTWIDGET_H
#define KRUFUSDCFILELISTWIDGET_H

// Qt
#include <QTreeWidget>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/ClientManager.h>
#include <dcpp/DirectoryListing.h>

// local
#include "tabcontent.h"


namespace KRufusDc
{

class Client;

/**
* Tree widget displaying remote user's file list.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileListWidget : public TabContent
{
	Q_OBJECT

public:

	/// Constructor
	///@param parent parent widget
	FileListWidget( QWidget* parent );
	virtual ~FileListWidget();
	
	/**
	 * @brief Loads file list file
	 * @param  path file path
	 * @param cid user id
	 */
	void loadFromFile( const QString& path, const QString& cid );

	/// Loads from internal listing object (_pListing)
	void loadFromListing();
	
private:

	/// Columns
	enum Columns
	{
		ColumnName = 0,  ///< File name
		ColumnSize,      ///< File size
		ColumnTTH,       ///< File hash
	};
	
	/// Roles for auxiliary data
	enum Roles
	{
		RoleSize = Qt::UserRole + 2611, ///< File size 
		RoleTTH,                        ///< TTH
		RoleType,                       ///< Type (Dir or File );
		RolePath,                       ///< Path to file
		RolePointer,                    ///< Pointer to DirectoryListing obj
	};
	
	/// Item type
	enum Type
	{
		File, ///< Plain file
		Dir   ///< Directory
	};

	/// Inside tree widget
	QTreeWidget* _pTree;

	/// Creates directory item from DCPP dir description
	QTreeWidgetItem* createDirectoryItem( dcpp::DirectoryListing::Directory* pDir, const QString& parentPath );
	
	/// Createsfile item from DCPP file description
	QTreeWidgetItem* createFileItem( dcpp::DirectoryListing::File* pFile, const QString& parentPath );
	
	/// Opens context menu
	virtual void contextMenuEvent( QContextMenuEvent* pEvent );
	
	/// Dir list representation
	dcpp::DirectoryListing* _pListing;
};

}

#endif // KRUFUSDCFILELISTWIDGET_H

// EOF


