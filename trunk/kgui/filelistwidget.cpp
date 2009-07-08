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

// bzlib
#include <bzlib.h>

// qt
#include <QDomDocument>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>

// KDE
#include <KIcon>
#include <KLocalizedString>
#include <KMimeType>
#include <KIconLoader>
#include <KAction>
#include <KMenu>

// local
#include "utils.h"
#include "filelistwidget.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
FileListWidget::FileListWidget( QWidget* parent )
	: TabContent( parent )
{
	_pTree = new QTreeWidget( this );
	QVBoxLayout* pLayout = new QVBoxLayout( this );
	pLayout->addWidget( _pTree );
	
	// init columns
	_pTree->setHeaderLabels( QStringList() << i18n("Name") << i18n("Size") );
	
	// init visual tab properties
	setTabTitle( i18n("File list") );
	setTabIcon( KIcon("view-list-tree") );
}

// ============================================================================
// Destructor
FileListWidget::~FileListWidget()
{
}

// ============================================================================
// Load file list
void FileListWidget::loadFromFile( const QString& path )
{
	// open file
	QFile file( path );
	if ( ! file.open( QIODevice::ReadOnly ) )
	{
		qDebug("Can't open file list file: %s", qPrintable(path) );
		return; 
	}
	
	// read data
	QByteArray rawData = file.readAll();
	QByteArray data;
	
	// decompress if needed
	if ( QFileInfo( path ).suffix().toLower() == "bz2" )
	{
		try
		{
			data = decompressBZ2( rawData );
		}
		catch(...)
		{
			qDebug("Error decompressing bz2");
			return;
		}
	}
	else
	{
		// use raw data
		data = rawData;
	}
	
	// load XML
	try
	{
		loadFromXML( data );
	}
	catch(...)
	{
		qDebug("Error loading from XML");
	}
}

// ============================================================================
// Decompress
QByteArray FileListWidget::decompressBZ2( const QByteArray& compressed )
{
	unsigned int inSize = compressed.size();
	const char* inBuf = compressed.data();
	
	unsigned int outSize = inSize * 10; // rough estimate
	
	char* outBuf = new char[ outSize ];
	
	int res = BZ2_bzBuffToBuffDecompress
			( outBuf
			, &outSize
			, (char*)inBuf
			, inSize
			, 0 // no small, be fast!
			, 1 // some verbosity (0-4)
			);
	
	if ( res == BZ_OK )
	{
			//cerr << "file list decompressed from " << inSize << " to " << outSize << " bytes" << endl;
			QByteArray result( outBuf, outSize );
			delete[] outBuf;
			
			return result;
	}
	else
	{
			delete[] outBuf;
			throw std::logic_error( "Error decompresing filelist.xml.bz2." );
	}
}

// ============================================================================
// Load from XML
void FileListWidget::loadFromXML( const QByteArray& xml )
{

	// parse xml
	QDomDocument doc;
	
	if ( ! doc.setContent( xml, false ) )
	{
		throw std::logic_error("Can't parse XML");
	}
	
	QDomElement root = doc.documentElement();
	if ( root.tagName() != "FileListing" )
	{
		throw std::logic_error( qPrintable( QString("Can't understand XML, root element id %1").arg(root.tagName()) ) );
	}
	
	// populate
	_pTree->clear();
	
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
				_pTree->addTopLevelItem( createDirectoryItem( element, "/" ) );
			}
			else if ( element.tagName() == "File" )
			{
				_pTree->addTopLevelItem( createFileItem( element, "/" ) );
			}
			else
			{
				qWarning("Unkown file list element type: %s", qPrintable( element.tagName() ) );
			}
		}
	}
	
	// set first coulm with to some value
	_pTree->setColumnWidth( 0, 250 );
	
	// change title
	// TODO
	//QString title = QString( i18n("%1's file list")).arg( pFileList->nick().c_str() );
	//setTabTitle( title );
}

// ============================================================================
// Create directory
QTreeWidgetItem* FileListWidget::createDirectoryItem( const QDomElement& element, const QString& parentPath )
{
	QString name = element.attribute( "Name" );
	QString path = parentPath + name + "/";
	QTreeWidgetItem* pDir = new  QTreeWidgetItem();
	
	pDir->setText( ColumnName, name );
	pDir->setIcon( ColumnName, KIcon("inode-directory") );
	
	pDir->setData( 0, RoleType, Dir );
	pDir->setData( 0, RolePath, path );
	
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
				pDir->addChild( createDirectoryItem( child, path ) );
			}
			else if ( child.tagName() == "File" )
			{
				pDir->addChild( createFileItem( child, path ) );
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
QTreeWidgetItem* FileListWidget::createFileItem( const QDomElement& element, const QString& parentPath )
{
	// decode XML
	QString name = element.attribute( "Name" );
	QString size = element.attribute( "Size" );
	QString tth = element.attribute( "TTH" );
	QString path = parentPath + name;
	
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
	pFile->setData( 0, RoleType, File );
	pFile->setData( 0, RolePath, path );
	
	return pFile;
}

// ============================================================================
// Context
void FileListWidget::contextMenuEvent( QContextMenuEvent* pEvent )
{
	QTreeWidgetItem* pItem = _pTree->currentItem();
	
	if ( pItem && pItem->data( 0, RoleType ) == File )
	{
		
		// get selection
		int row = _pTree->selectionModel()->currentIndex().row();
			
		// init menu
		KAction actionDownloadFile( KIcon("go-down"), i18n("Download file"), this );
		KMenu menu( this );
		
		menu.addTitle( i18n("File") );
		menu.addAction( & actionDownloadFile );
		
		QAction* pRes = menu.exec( pEvent->globalPos() );
		
		// handle selection
		if ( pRes == & actionDownloadFile )
		{
			// extract data
			quint64 size = pItem->data( 0, RoleSize ).value<quint64>();
			QByteArray tth = pItem->data( 0, RoleTTH ).value<QByteArray>();
			QByteArray path = pItem->data( 0, RolePath ).value<QByteArray>();
		
			/* TODO
			_pClient->downloadFile
				( _pFileList->hub().c_str()
				, _pFileList->nick().c_str()
				, path
				, tth
				, size
				);
			*/
		}
	}
}

} // namespace
