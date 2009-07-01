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
#ifndef KRUFUSDCTABCONTENT_H
#define KRUFUSDCTABCONTENT_H

#include <QIcon>
#include <QWidget>

namespace KRufusDc
{

/**
* @brief Widged displayed inside TabWidget's tab.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class TabContent : public QWidget
{
	Q_OBJECT

public:
	TabContent( QWidget *parent = 0 );
	~TabContent();

	/// Returns tab title
	QString tabTitle() const { return _tabTitle; }
	
	/// Returns tab icon
	QIcon tabIcon() const { return _tabIcon; }

public Q_SLOTS:

	/// Called when tab is closed
	virtual void closeTab();

Q_SIGNALS:
	
	/// Tab title changed
	void titleChanged( const QString& title );
	
	/// Tab iconchanged
	void iconChanged( const QIcon& icon );

protected Q_SLOTS: // slots

	/// Sets tab title
	void setTabTitle( const QString& title );
	
	/// Sets tab icon
	void setTabIcon( const QIcon& icon );
	
protected: // methods
	
	/// Calls method from worker thread. Method must be a slot
	///@param method method name, w/o signature.
	void invoke
		( const char* method
		, QGenericArgument val0 = QGenericArgument( 0 )
		, QGenericArgument val1 = QGenericArgument()
		, QGenericArgument val2 = QGenericArgument()
		, QGenericArgument val3 = QGenericArgument()
		, QGenericArgument val4 = QGenericArgument()
		);

private: // data

	/// Current tab title
	QString _tabTitle;
	
	/// Current tab icon
	QIcon _tabIcon;
};

}

#endif // KRUFUSDCTABCONTENT_H

// EOF


