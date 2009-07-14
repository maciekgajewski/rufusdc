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
#ifndef KRUFUSDCSEARCHQUERY_H
#define KRUFUSDCSEARCHQUERY_H

// Qt
#include <QMetaType>
#include <QString>

namespace KRufusDc {

/**
* Simple structure used to transmit search query across the threads.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class SearchQuery
{
public:
    SearchQuery();
    ~SearchQuery();

	void setText ( const QString& theValue )
	{
		_text = theValue;
	}
	

	QString text() const
	{
		return _text;
	}

	void setType ( int theValue )
	{
		_type = theValue;
	}
	

	int type() const
	{
		return _type;
	}

	void setSizeMethod ( int theValue )
	{
		_sizeMethod = theValue;
	}
	

	int sizeMethod() const
	{
		return _sizeMethod;
	}

	void setSize ( const int64_t& theValue )
	{
		_size = theValue;
	}
	

	int64_t size() const
	{
		return _size;
	}

	void setToken ( const QString& theValue )
	{
		_token = theValue;
	}
	

	QString token() const
	{
		return _token;
	}

private:

	QString	_text;		///< Searched text - name ot TTH
	int _type;			///< File type (SearchManager::TypeModes)
	int _sizeMethod;	///< Size comparision method (SearchManager::SizeModes)
	int64_t	_size;		///< Size threshold
	QString _token;		///< Search token 
	
};

}

Q_DECLARE_METATYPE(KRufusDc::SearchQuery);

#endif // KRUFUSDCSEARCHQUERY_H

// EOF


