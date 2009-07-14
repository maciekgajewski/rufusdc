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

// kde
#include <KLocalizedString>
#include <KIcon>

// local
#include "clientthread.h"
#include "searchquery.h"
#include "searchwidget.h"

namespace KRufusDc
{

// ============================================================================
// constructor
SearchWidget::SearchWidget(QWidget* parent): TabContent(parent)
{
	setupUi( this );
	
	setTabTitle( i18n("Search") );
	
	// some extra UI setup
	pSearchButton->setIcon( KIcon("edit-find") );
	
	// setup timer
	_timeToSearchTimer.setInterval( 500 ); //; to get smooth, 1-second updates, one shuld use 0.5 second timeout
	connect( &_timeToSearchTimer, SIGNAL(timeout()), SLOT(onTimeToSearchTimer()) );
	
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
	SearchQuery query;
	// TODO
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
	/*
	std::string token = pResult->getToken();
	std::string file =  pResult->getFile();
	std::string tth = pResult->getTTH().toBase32();
	
	qDebug("Search result: file=%s, token=%s, tth=%s", file.c_str(), token.c_str(), tth.c_str() );
	*/
	// TODO make it useful
}

} // ns

// EOF

