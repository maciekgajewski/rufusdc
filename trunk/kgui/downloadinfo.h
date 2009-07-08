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
#ifndef KRUFUSDCDOWNLOADINFO_H
#define KRUFUSDCDOWNLOADINFO_H

// Qt
#include <QMetaType>

// dcpp
namespace dcpp
{
	class QueueItem;
}

namespace KRufusDc
{

/**
* @brief Structure descvribinng download status
* Describes multi-source download
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DownloadInfo
{
public:
    DownloadInfo();
    ~DownloadInfo();

	enum State
	{
		RUNNING,   ///< Downloading
		WAITING,   ///< No sources
		FINISHED,  ///< Finished
	};
	
	/// Fills the structure from QueueItem
	void fromQueueItem( dcpp::QueueItem* pItem );
	
	/// Returns state
	State state() const { return _state; }

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

	bool isFileList() const
	{
		return _isFileList;
	}
	

private:
	State _state;            ///< Transfer state (download specific)
	
	QString _TTH;            ///< TTH (base32) (the key)
	int     _onlineUsers;    ///< Online users for the transfer (download specific)
	QString _path;           ///< File path
	int64_t _size;           ///< File size
	int64_t _transferred;    ///< Transferred bytes
	bool    _isFileList;     ///< If tis is file list transfer

};

} // ns

Q_DECLARE_METATYPE(KRufusDc::DownloadInfo);

#endif // KRUFUSDCDOWNLOADINFO_H

// EOF

