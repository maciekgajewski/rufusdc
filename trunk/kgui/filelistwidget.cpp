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

// qt
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QFileInfo>

// KDE
#include <KIcon>
#include <KLocalizedString>
#include <KMimeType>
#include <KIconLoader>
#include <KAction>
#include <KMenu>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/ClientManager.h>
#include <dcpp/DirectoryListing.h>
#include <dcpp/SettingsManager.h>

// local
#include "utils.h"
#include "smartsorttreeitem.h"
#include "filelistwidget.h"

namespace KRufusDc
{

// ============================================================================
// Constructor
FileListWidget::FileListWidget( QWidget* parent )
	: TabContent( parent )
	, _pListing( NULL )
{
	_pTree = new QTreeWidget( this );
	QVBoxLayout* pLayout = new QVBoxLayout( this );
	pLayout->addWidget( _pTree );
	
	// init columns
	_pTree->setHeaderLabels( QStringList() << i18n("Name") << i18n("Size") << i18n("TTH") );
	_pTree->setSortingEnabled( true );
	_pTree->setAlternatingRowColors( true );
	
	// init visual tab properties
	setTabTitle( i18n("File list") );
	setTabIcon( KIcon("view-list-tree") );
}

// ============================================================================
// Destructor
FileListWidget::~FileListWidget()
{
	if ( _pListing ) delete _pListing;
}

// ============================================================================
// Load file list
void FileListWidget::loadFromFile( const QString& path, const QString& cid )
{
	// Find user
	dcpp::UserPtr pUser = dcpp::ClientManager::getInstance()->findUser( dcpp::CID( cid.toStdString() ) );
	if ( ! pUser )
	{
		qDebug("File list - can't find user");
		return;
	}
	
	//create dir listing
	Q_ASSERT( ! _pListing );
	_pListing = new dcpp::DirectoryListing( pUser );
	
	// load file
	try
	{
		_pListing->loadFile( path.toUtf8().data() );
	}
	catch(...)
	{
		qDebug("Error loading from file");
		return;
	}
	
	// set title from CID
	std::string nickss = dcpp::ClientManager::getInstance()->getNicks( dcpp::CID( cid.toStdString() ) ).front();
	QString nick = QString::fromUtf8( nickss.c_str() );
	setTabTitle( i18n("File list: %1").arg( nick ) );
	
	// build tree
	loadFromListing();
}


// ============================================================================
// Load from listing
void FileListWidget::loadFromListing()
{
	// populate
	_pTree->clear();
	dcpp::DirectoryListing::Directory* pRoot = _pListing->getRoot();
	
	// add dirs
	dcpp::DirectoryListing::Directory::Iter dirIt;
	for ( dirIt = pRoot->directories.begin(); dirIt != pRoot->directories.end(); ++dirIt )
	{
		_pTree->addTopLevelItem( createDirectoryItem( *dirIt, PATH_SEPARATOR_STR ) );
	}
	// add files
	dcpp::DirectoryListing::File::Iter fileIt;
	for ( fileIt = pRoot->files.begin(); fileIt != pRoot->files.end(); ++fileIt )
	{
		_pTree->addTopLevelItem( createFileItem( *fileIt, PATH_SEPARATOR_STR ) );
	}
	
	// set first coulm width to some value (do not use 'fit t content', as the content is looooong)
	_pTree->setColumnWidth( 0, 250 );
	_pTree->sortItems( ColumnName,  Qt::AscendingOrder );
}


// ============================================================================
// Create directory
QTreeWidgetItem* FileListWidget::createDirectoryItem( dcpp::DirectoryListing::Directory* pDir, const QString& parentPath )
{
	QString name = QString::fromUtf8( pDir->getName().c_str() );
	QString path = parentPath + name + PATH_SEPARATOR_STR;
	
	SmartSortTreeItem* pItem = new  SmartSortTreeItem();
	
	// text / icon
	pItem->setText( ColumnName, name );
	pItem->setIcon( ColumnName, KIcon("inode-directory") );
	
	pItem->setData( 0, RoleType, Dir );
	pItem->setData( 0, RolePath, path );
	pItem->setData( 0, RolePointer, QVariant::fromValue<void*>( pDir ) );
	
	// size
	int64_t sizeValue = pDir->getTotalSize();
	QString size = sizeToString( sizeValue );
	
	pItem->setText( ColumnSize, size );
	pItem->setValue( ColumnSize, double(sizeValue) );
	pItem->setTextAlignment( ColumnSize, Qt::AlignRight );
	
	
	// add subdirs
	dcpp::DirectoryListing::Directory::Iter dirIt;
	for ( dirIt = pDir->directories.begin(); dirIt != pDir->directories.end(); ++dirIt )
	{
		pItem->addChild( createDirectoryItem( *dirIt, path ) );
	}
	// add files
	dcpp::DirectoryListing::File::Iter fileIt;
	for ( fileIt = pDir->files.begin(); fileIt != pDir->files.end(); ++fileIt )
	{
		pItem->addChild( createFileItem( *fileIt, path ) );
	}
	
	return pItem;
}

// ============================================================================
// Create file
QTreeWidgetItem* FileListWidget::createFileItem( dcpp::DirectoryListing::File* pFile, const QString& parentPath )
{
	QString name = QString::fromUtf8( pFile->getName().c_str() );
	int64_t sizeValue = pFile->getSize();
	QString tth = QString::fromStdString( pFile->getTTH().toBase32() );
	QString path = parentPath + name;
	
	QString sizeText = sizeToString( sizeValue );
	
	SmartSortTreeItem* pItem = new  SmartSortTreeItem();
	
	// name
	pItem->setText( ColumnName, name );
	
	// size
	pItem->setText( ColumnSize, sizeText );
	pItem->setValue( ColumnSize, double(sizeValue) );
	pItem->setTextAlignment( ColumnSize, Qt::AlignRight );
	
	// tth
	pItem->setText( ColumnTTH, tth );
	
	// find icon
	QString iconName = KMimeType::iconNameForUrl( name );
	QPixmap icon = KIconLoader::global()->loadMimeTypeIcon( iconName, KIconLoader::Small );
	pItem->setIcon( ColumnName, icon );

	pItem->setData( 0, RoleSize, qlonglong(sizeValue) );
	pItem->setData( 0, RoleTTH, tth );
	pItem->setData( 0, RoleType, File );
	pItem->setData( 0, RolePath, path );
	pItem->setData( 0, RolePointer, QVariant::fromValue<void*>( pFile ) );
	
	return pItem;
}


// ============================================================================
// Context
void FileListWidget::contextMenuEvent( QContextMenuEvent* pEvent )
{
	Q_ASSERT( _pListing );
	
	QTreeWidgetItem* pItem = _pTree->currentItem();
	
	if ( ! pItem )
	{
		return;
	}
	
	// file
	if ( pItem->data( 0, RoleType ) == File )
	{
			
		// init menu
		KAction actionDownloadFile( KIcon("go-down"), i18n("Download file"), this );
		KMenu menu( this );
		
		menu.addTitle( i18n("File") );
		menu.addAction( & actionDownloadFile );
		
		QAction* pRes = menu.exec( pEvent->globalPos() );
		
		dcpp::DirectoryListing::File* pFile 
			= (dcpp::DirectoryListing::File*)pItem->data( 0, RolePointer ).value<void*>();
		
		// create target path
		std::string path = SETTING(DOWNLOAD_DIRECTORY) + pFile->getName();
		
		// handle selection
		if ( pRes == & actionDownloadFile )
		{
			qDebug("Downloading file: %s", path.c_str() );
			try
			{
				_pListing->download( pFile, path, false, false ); // false = no view, no high-prio
			}
			catch( const std::exception& e )
			{
				qDebug("Error downloading file: %s", e.what() );
			}
		}
	}
	// file
	else if ( pItem->data( 0, RoleType ) == Dir )
	{
			
		// init menu
		KAction actionDownload( KIcon("go-down"), i18n("Download directory"), this );
		KMenu menu( this );
		
		menu.addTitle( i18n("Directory") );
		menu.addAction( & actionDownload );
		
		QAction* pRes = menu.exec( pEvent->globalPos() );
		
		dcpp::DirectoryListing::Directory* pDir 
			= (dcpp::DirectoryListing::Directory*)pItem->data( 0, RolePointer ).value<void*>();
		
		// create target path
		std::string path = SETTING(DOWNLOAD_DIRECTORY);
		
		// handle selection
		if ( pRes == & actionDownload )
		{
			qDebug("Downloading dir: %s", path.c_str() );
			try
			{
				_pListing->download( pDir, path, false ); // false = no high-prio
			}
			catch( const std::exception& e )
			{
				qDebug("Error downloading dir: %s", e.what() );
			}
		}
	}
}

} // namespace
