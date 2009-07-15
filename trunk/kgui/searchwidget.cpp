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
#include <QFileInfo>

// kde
#include <KLocalizedString>
#include <KIcon>
#include <KIconLoader>
#include <KMimeType>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/SearchManager.h>
#include <dcpp/SearchResult.h>

// local
#include "clientthread.h"
#include "searchquery.h"
#include "transferinfo.h"
#include "smartsorttreeitem.h"
#include "utils.h"

#include "searchwidget.h"

namespace KRufusDc
{

static const int ACTIVE_SEARCH_TIME = 30*1000; // time when searchresult are received [ms]

// ============================================================================
// constructor
SearchWidget::SearchWidget(QWidget* parent): TabContent(parent)
{
	setupUi( this );
	_searchActive = false;
	
	setTabTitle( i18n("Search") );
	
	// some extra UI setup
	pSearchButton->setIcon( KIcon("edit-find") );
	pResults->setColumnWidth( COLUMN_NAME, 250 );
	
	// setup timer
	_timeToSearchTimer.setInterval( 500 ); //; to get smooth, 1-second updates, one shuld use 0.5 second timeout
	connect( &_timeToSearchTimer, SIGNAL(timeout()), SLOT(onTimeToSearchTimer()) );
	_timeToSearchTimer.start();
	
	
	
	dcpp::SearchManager::getInstance()->addListener(this);
}

// ============================================================================
// destructor
SearchWidget::~SearchWidget()
{
	dcpp::SearchManager::getInstance()->removeListener(this);
}

// ============================================================================
// on search
void SearchWidget::onSearch()
{
	if ( ! dcpp::SearchManager::getInstance()->okToSearch() )
	{
		return; // do nothing
	}
	
	// clear results
	pResults->clear();
	
	// search
	SearchQuery query;

	query.setText( pSearchLine->text() );
	query.setType( pTypeCombo->currentIndex() );
	query.setToken( "halo" ); // TODO use properly. My hub does ot support it, have to find oneter one
	query.setSizeMethod( pSizeCombo->currentIndex() );
	
	double size = pSizeInput->value();
	for ( int i = 0; i < pUnitsCombo->currentIndex(); i++ )
	{
		size = size * 1024;
	}
	query.setSize( int64_t( size ) );
	
	ClientThread::invoke("search", Q_ARG(SearchQuery, query ) );
	
	// active for some time
	_searchActive = true;
	QTimer::singleShot( ACTIVE_SEARCH_TIME, this, SLOT(deactivateSearch()) );
	
	// force update
	onTimeToSearchTimer();
}

// ============================================================================
// Deactivate search
void SearchWidget::deactivateSearch()
{
	_searchActive = false;
}

// ============================================================================
// On size method changed
void SearchWidget::onSizeMethodChanged( int method )
{
	pSizeInput->setEnabled( method != 0 );
	pUnitsCombo->setEnabled( method != 0 );
}

// ============================================================================
// Time to search
void SearchWidget::onTimeToSearchTimer()
{
	QString baseText = i18n("Search");
	
	if ( dcpp::SearchManager::getInstance()->okToSearch() )
	{
		pSearchButton->setText( baseText );
		pSearchButton->setEnabled( true );
		pSearchButton->setToolTip( i18n("Start searching") );
	}
	else
	{
		QString text = i18n( "%1 (%2)" )
			.arg( baseText )
			.arg( dcpp::SearchManager::getInstance()->timeToSearch() );
		
		pSearchButton->setText( text );
		pSearchButton->setEnabled( false );
		pSearchButton->setToolTip( i18n("Wait until search is available") );
		
	}
}

// ============================================================================
// On search result
void SearchWidget::on(dcpp::SearchManagerListener::SR, const dcpp::SearchResultPtr& pResult) throw()
{
	/* TODO remove
	std::string token = pResult->getToken();
	std::string file =  pResult->getFile();
	std::string tth = pResult->getTTH().toBase32();
	int64_t size = pResult->getSize();
	
	qDebug("Search result: file=%s, token=%s, tth=%s, size=%lld"
		, file.c_str(), token.c_str(), tth.c_str(), size );
	*/
	TransferInfo info;
	info.fromSearchResult( pResult.get() );
	invoke("onResult", Q_ARG( TransferInfo, info ) );
}

// ============================================================================
// on result
void SearchWidget::onResult( const TransferInfo& info )
{
	// ignore if searchdiabled
	if ( ! _searchActive )
	{
		return;
	}
	
	SmartSortTreeItem* pItem = new SmartSortTreeItem( pResults );
	
	// data
	pItem->setData( 0, ROLE_DATA, QVariant::fromValue( info ) );
	
	// name
	pItem->setText( COLUMN_NAME, QFileInfo( info.path() ).fileName() );
	// TODO icon
	
	// size
	pItem->setText( COLUMN_SIZE, sizeToString( info.size() ) );
	pItem->setValue( COLUMN_SIZE, double( info.size() ) );
	
	// slots
	pItem->setText( COLUMN_SLOTS, info.slotString() );
	
	// tth
	pItem->setText( COLUMN_TTH, info.TTH() );
	
	// user hub
	QString uh = QString("%1@%2").arg( info.userNick() ).arg( info.userHub() );
	pItem->setText( COLUMN_USER, uh );
	
	// icon
	if ( info.isDirectory() )
	{
		pItem->setIcon( COLUMN_NAME, KIcon("inode-directory") );
	}
	else
	{
		QString iconName = KMimeType::iconNameForUrl( info.path() );
		QPixmap icon = KIconLoader::global()->loadMimeTypeIcon( iconName, KIconLoader::Small );
		pItem->setIcon( COLUMN_NAME, icon );
	}
}

} // ns

// EOF

