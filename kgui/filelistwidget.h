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

// boost
#include <boost/shared_ptr.hpp>

// Qt
#include <QTreeWidget>
class QDomElement;

namespace RufusDc
{
	class FileList;
}

namespace KRufusDc
{

/**
* Tree widget displaying remote user's file list.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileListWidget : public QTreeWidget
{
	Q_OBJECT

public:
	FileListWidget( QWidget* parent );
	virtual ~FileListWidget();
	
	/**
	 * @brief Set's file list.
	 * Parses XML data contained within the file list
	 * @param  pFileList file list pointer
	 * @exception std::logic_error if can't parse xml
	 */
	void setFileList( boost::shared_ptr< RufusDc::FileList > pFileList );

private:

	/// Columns
	enum Columns
	{
		ColumnName = 0,  ///< File name
		ColumnSize       ///< File size
	};
	
	/// Roles for auxiliary data
	enum Roles
	{
		RoleSize = Qt::UserRole + 2611, ///< File size 
		RoleTTH                         ///< TTH
	};

	/// Creates directory item from XML element
	QTreeWidgetItem* createDirectoryItem( const QDomElement& element );
	
	/// Createsfile item from XML element
	QTreeWidgetItem* createFileItem( const QDomElement& element );
	
	/// The file list
	boost::shared_ptr< RufusDc::FileList > _pFileList;
};

}

#endif // KRUFUSDCFILELISTWIDGET_H

// EOF


