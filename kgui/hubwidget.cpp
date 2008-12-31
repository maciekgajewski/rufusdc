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
#include <QThread>

#include "hub.h"

#include "hubwidget.h"

namespace KRufusDc
{


// ============================================================================
// Constructor
HubWidget::HubWidget( Hub* pHub, QWidget* parent, Qt::WindowFlags f )
	: QWidget( parent, f )
	, Ui::HubWidget()
	, _pHub( pHub )
{
	Q_ASSERT( pHub );
	
	setupUi( this );
	
	connect( pHub, SIGNAL( hubMessage(int,QString)), SLOT( onHubMessage(int,QString) ) );
}

// ============================================================================
// Destructor
HubWidget::~HubWidget()
{
}

// ============================================================================
// incoming message
void HubWidget::onHubMessage( int type, const QString& msg )
{
	//qDebug("msg: tid:%d type:%d %s", int(QThread::currentThreadId()), type, qPrintable( msg ) );
	
	if ( type == Hub::System )
	{
		pChat->setTextColor( Qt::darkRed );
	}
	else
	{
		pChat->setTextColor( Qt::black ); // TODO use palette here
	}
	pChat->append( msg );
}


}
