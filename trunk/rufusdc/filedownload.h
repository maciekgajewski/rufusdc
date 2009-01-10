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

// local
#include "download.h"

namespace RufusDc
{

using namespace std;

class Hub;

/**
* File download
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileDownload : public Download
{
	friend class Client;
public:
	
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
	FileDownload( Client* pParent );
	
	/// Initializes temporary file
	void createTemporaryFile();
	
	/// Original file path
	string _path;
	
	/// File's TTH
	string _tth;
	
	/// Expected file size
	uint64_t _size;
	
	/// Source hub
	///@todo need more of these for mutli-download
	shared_ptr<Hub> _pHub;
	
	/// Temporary file
	fstream _tmpFile;
	
	/// Temporary file name
	string _tmpFileName;
	
	// state
	
	/// Received bytes. Received is continous block begining at the start of the file
	///@todo replece by chain of received regions.
	uint64_t _receivedBytes; 

	// slots
	
};

}

#endif // RUFUSDCFILEDOWNLOAD_H

// EOF


