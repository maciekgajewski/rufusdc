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

#include "filedownload.h"
#include "filelistdownload.h"

#include "downloadmanager.h"

namespace RufusDc {

// ============================================================================
// Constructor
DownloadManager::DownloadManager()
	: Singleton<DownloadManager>()
{
}

// ============================================================================
// Destructor
DownloadManager::~DownloadManager()
{
}

// ============================================================================
// Download file list
void DownloadManager::downloadFileList( const string& hub, const string& nick )
{
	FileListDownload::ptr pDownload( new FileListDownload() );
	
	pDownload->setSource( hub, nick );
	pDownload->start();
	
	_downloads.push_back( shared_ptr<Download>( pDownload ) );
}

// ============================================================================
// Download file
void DownloadManager::downloadFile
	( const string& hub
	, const string& nick
	, const string& path
	, const string& tth
	, uint64_t size
	)
{
	// TODO check for duplicates

	FileDownload::ptr pDownload( new FileDownload() );
	pDownload->setSource( hub, nick );
	pDownload->setPath( path );
	pDownload->setTth( tth );
	pDownload->setSize( size );
	
	pDownload->start();
	
	_downloads.push_back( shared_ptr<Download>( pDownload ) );
}

// ============================================================================
// File list received
void DownloadManager::fileListReceived( const shared_ptr<FileList>& pList )
{
	// just emit signal
	signalIncomingFileList( pList );
}

}
