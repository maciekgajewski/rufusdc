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
#ifndef RUFUSDCFILEDOWNLOAD_H
#define RUFUSDCFILEDOWNLOAD_H

// stl
#include <fstream>
#include <list>

// boost
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>

// local
#include "downloadmap.h"
#include "download.h"
#include "directconnection.h"
#include "hub.h"

namespace RufusDc
{

using namespace std;

class Error;

/**
* File download
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileDownload : public Download
{
	friend class DownloadManager;
public:
	
	/// pointer type
	typedef boost::shared_ptr<FileDownload> ptr;

	/// Destructor
	virtual ~FileDownload();

	/**
	 * @brief Sets file path
	 * @param path file path
	 */
	void setPath( const string& path ){ _path = path;	}
	
	/// Returns path
	string path() const { return _path;	}

	/// Sets file TTH
	void setTth( const string& value ){	_tth = value;}
	
	/// Returns file TTH
	string tth() const { return _tth; }
	
	/// starts download
	virtual void start();

	/// Sets expected file size
	void setSize( const uint64_t& value ) { _size = value; }
	
	/// Returns expected file size
	uint64_t size() const{ return _size; }
	
private:

	/// Constructor
	/// Accessibl;e only to manager
	FileDownload();
	
	/// File source information
	struct Source
	{
		typedef boost::shared_ptr< Source > ptr;
		
		string hub;
		string nick;
		string path;
		
		boost::posix_time::ptime successfullConnection;
		DirectConnection::ptr pConnection;
	};
	
	/// Initializes temporary file
	void createTemporaryFile();
	
	/// Initializes temporary file with information
	void createInfoFile();
	
	/// Dumps state into info file
	void dumpInfo();
	
	/// Escapes string before writing into config file
	string escape( const string& str );
	
	/// Attempts to connect to source
	void connectToSource( const shared_ptr<Source>& pSrc );
	
	/// Finds what source shoudl download after connection
	void findWhatToDownload( Source* pSrc, uint64_t& start, uint64_t& len );
	
	/// Complete download
	void completeDownload();

	/// Original file path
	string _path;
	
	/// File's TTH
	string _tth;
	
	/// Expected file size
	uint64_t _size;
	
	/// Source hub
	///@todo need more of these for mutli-download
	Hub::ptr _pHub;
	
	/// Temporary file
	fstream _tmpFile;
	
	/// Temporary file name
	string _tmpFileName;
	
	/// Temporary file for download information name
	string _infoFileName;
	
	// state
	
	/// Received bytes. Received is continous block begining at the start of the file
	///@todo replece by chain of received regions.
	uint64_t _receivedBytes; 

	/// List of sources
	list< Source::ptr > _sources;
	
	/// Download map
	DownloadMap _map;
	
	/// Timer connecting periodically to sources
	boost::asio::deadline_timer _timer;
	

	// slots
	
	void onConnected( const shared_ptr<Source>& pSrc, const Error& err, const shared_ptr<DirectConnection>& pConn );
	void onDataIncoming( vector<char>& data, uint64_t offset );
	void onDownloadCompleted( const shared_ptr<Source>& pSrc, const Error& err );
};

}

#endif // RUFUSDCFILEDOWNLOAD_H

// EOF


