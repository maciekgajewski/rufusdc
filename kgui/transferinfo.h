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
#ifndef KRUFUSDCTRANSFERINFO_H
#define KRUFUSDCTRANSFERINFO_H

// Qt
#include <QMetaType>

// dcpp
namespace dcpp
{
	class QueueItem;
	class Upload;
}

namespace KRufusDc
{

/**
* @brief Structure containing transfer information.
* Used to transfer the information across threads
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class TransferInfo
{
public:
	/// Transfer type
	enum Type
	{
		DOWNLOAD,
		UPLOAD,
	};
	
	TransferInfo();
	~TransferInfo();
	
	
	/// Fills the structure with data from cpp::Upload
	void fromDcppUpload( dcpp::Upload* pUpload );
	
	/// Fills the structure from QueueItem
	void fromQueueItem( dcpp::QueueItem* pItem );
	
public: // accessors

	/// Returns transfer type
	Type type() const { return _type; }

	QString TTH() const
	{
		return _TTH;
	}

	int onlineUsers() const
	{
		return _onlineUsers;
	}

	QString path() const
	{
		return _path;
	}

	int64_t size() const
	{
		return _size;
	}

	int64_t transferred() const
	{
		return _transferred;
	}

	double averageSpeed() const
	{
		return _averageSpeed;
	}

private: // data

	Type _type;
	
	QString _TTH;            ///< TTH (base32) (the key)
	int     _onlineUsers;    ///< Online users for the transfer (download specific)
	QString _path;           ///< File path
	int64_t _size;           ///< File size
	int64_t _transferred;    ///< Transferred bytes
	double	_averageSpeed;   ///< Average transfer speed
	QString _CID;            ///< User ID (base32) (upload specific)
	
	/* TODO this belongs to connection
	int64_t _secondsLeft;    ///< Estimated time remaining
	QString _userNick;       ///< User nick
	QString _userHub;        ///< User hub
	*/

};

}

Q_DECLARE_METATYPE(KRufusDc::TransferInfo);

#endif // KRUFUSDCTRANSFERINFO_H

// EOF


