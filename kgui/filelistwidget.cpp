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

// std
#include <stdexcept>

// rufusdc
#include "rufusdc/filelist.h"

// qt
#include <QDomDocument>

// KDE
#include <KIcon>
#include <KLocalizedString>
#include <KMimeType>
#include <KIconLoader>

// local
#include "utils.h"
#include "filelistwidget.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
FileListWidget::FileListWidget( QWidget* parent ): QTreeWidget( parent )
{
	// init columns
	setHeaderLabels( QStringList() << i18n("Name") << i18n("Size") );
	
}

// ============================================================================
// Destructor
FileListWidget::~FileListWidget()
{
}

// ============================================================================
// Set file list
void FileListWidget::setFileList( boost::shared_ptr< RufusDc::FileList > pFileList )
{
	_pFileList = pFileList;
	
	// parse xml
	QDomDocument doc;
	
	QByteArray data( pFileList->xml().c_str() );
	
	if ( ! doc.setContent( data, false ) )
	{
		throw std::logic_error("Can't parse XML");
	}
	
	QDomElement root = doc.documentElement();
	if ( root.tagName() != "FileListing" )
	{
		throw std::logic_error( qPrintable( QString("Can't understand XML, root element id %1").arg(root.tagName()) ) );
	}
	
	// populate
	clear();
	
	QDomNodeList topLevelElements = root.childNodes();
	for( int i = 0; i < topLevelElements.size(); i++ )
	{
		QDomNode node = topLevelElements.at( i );
		if ( node.isElement() )
		{
			QDomElement element = node.toElement();
			
			/// directory
			if ( element.tagName() == "Directory" )
			{
				addTopLevelItem( createDirectoryItem( element ) );
			}
			else if ( element.tagName() == "File" )
			{
				addTopLevelItem( createFileItem( element ) );
			}
			else
			{
				qWarning("Unkown file list element type: %s", qPrintable( element.tagName() ) );
			}
		}
	}
	
	resizeColumnToContents( 0 );
}

// ============================================================================
// Create directory
QTreeWidgetItem* FileListWidget::createDirectoryItem( const QDomElement& element )
{
	QString name = element.attribute( "Name" );
	QTreeWidgetItem* pDir = new  QTreeWidgetItem();
	
	pDir->setText( ColumnName, name );
	pDir->setIcon( ColumnName, KIcon("inode-directory") );
	
	// add children
	QDomNodeList children = element.childNodes();
	for( int i = 0; i < children.size(); i++ )
	{
		QDomNode node = children.at( i );
		if ( node.isElement() )
		{
			QDomElement child = node.toElement();
			
			/// directory
			if ( child.tagName() == "Directory" )
			{
				pDir->addChild( createDirectoryItem( child ) );
			}
			else if ( child.tagName() == "File" )
			{
				pDir->addChild( createFileItem( child ) );
			}
			else
			{
				qWarning("Unkown file list element type: %s", qPrintable( element.tagName() ) );
			}
		}
	}
	
	return pDir;
}

// ============================================================================
// Create file
QTreeWidgetItem* FileListWidget::createFileItem( const QDomElement& element )
{
	// decode XML
	QString name = element.attribute( "Name" );
	QString size = element.attribute( "Size" );
	QString tth = element.attribute( "TTH" );
	
	quint64 sizeValue = size.toLongLong();
	QString sizeText = sizeToString( sizeValue );
	
	QTreeWidgetItem* pFile = new  QTreeWidgetItem();
	
	pFile->setText( ColumnName, name );
	pFile->setText( ColumnSize, sizeText );
	
	// find icon
	QString iconName = KMimeType::iconNameForUrl( name );
	QPixmap icon = KIconLoader::global()->loadMimeTypeIcon( iconName, KIconLoader::Small );
	pFile->setIcon( ColumnName, icon );

	pFile->setData( 0, RoleSize, sizeValue );
	pFile->setData( 0, RoleTTH, tth );
	
	return pFile;
}

} // namespace
