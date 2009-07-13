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
#ifndef RUFUSDCDOWNLOADMANAGER_H
#define RUFUSDCDOWNLOADMANAGER_H

// std
#include <list>

// boost
#include <boost/signals.hpp>
#include <boost/thread/mutex.hpp>

// local
#include "download.h"
#include "filelist.h"

#include "singleton.h"

namespace RufusDc
{

/**
* Manages ongoing downloads
*/
class DownloadManager : public Singleton<DownloadManager>
{
public: // signals
	
	
	/// File list received
	boost::signal< void (const FileList::ptr&) > signalIncomingFileList;

public: // methods

	/// Constructor
    DownloadManager();

	/// Destructor
    virtual ~DownloadManager();

	/**
	 * @brief Starts file list download
	 * Creates file list download manager and places it in download list.
	 * @param hub  hub to which user is connected
	 * @param nick user's nick
	 */
	void downloadFileList( const string& hub, const string& nick );
	
	/**
	 * @brief Starts file download
	 * Creates file download manager and places it in download list.
	 * @param hub  hub to which owner of file is connected
	 * @param nick user's nick
	 * @param path file path
	 * @param tth  file hash
	 * @param size expected file size
	 */
	void downloadFile
		( const string& hub
		, const string& nick
		, const string& path
		, const string& tth
		, uint64_t size
		);
	
	/**
	 * @brief Called by FileListDownload
	 * States that file list was succesfully downloaded
	 * @param pList received file list
	 * @internal
	 */
	void fileListReceived( const FileList::ptr& pList );
	
private:

	/// Downloads list
	std::list< Download::ptr > _downloads;
	
	/// Mutex guarding downloads
	boost::mutex _downloadsMutex;
	
};

}

#endif
