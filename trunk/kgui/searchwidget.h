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
#ifndef KRUFUSDCSEARCHWIDGET_H
#define KRUFUSDCSEARCHWIDGET_H

// Qt
#include <QTimer>

// dcpp
#include <dcpp/stdinc.h>
#include <dcpp/DCPlusPlus.h>
#include <dcpp/SearchManager.h>

// local
#include "tabcontent.h"
#include "ui_searchwidget.h"

namespace KRufusDc {

/**
* Search tab
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class SearchWidget
	: public TabContent
	, private Ui::SearchWidget
	, public dcpp::SearchManagerListener
{
	Q_OBJECT
public:
	SearchWidget(QWidget* parent);
	virtual ~SearchWidget();
	
protected Q_SLOTS:

	/// Starts search
	void onSearch();
	
	/// Called when combo box with size method changes
	///@param method index of selected method
	void onSizeMethodChanged( int method );
	
	/// Timer handler. Updates 'time to search' info.
	void onTimeToSearchTimer();
	
protected: // DC++ event handlers

	// SearchManager
	virtual void on(dcpp::SearchManagerListener::SR, const dcpp::SearchResultPtr& pResult) throw();

private: // data

	QTimer _timeToSearchTimer;	///< Timer used to dsplay 'time to search'

};

}

#endif // KRUFUSDCSEARCHWIDGET_H

// EOF


