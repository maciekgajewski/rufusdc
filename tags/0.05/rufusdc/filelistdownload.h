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
#ifndef RUFUSDCFILELISTDOWNLOAD_H
#define RUFUSDCFILELISTDOWNLOAD_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "download.h"

namespace RufusDc
{

using namespace std;
using namespace boost;

class DirectConnection;
class Error;

/**
* File list download
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class FileListDownload : public Download
{

public:

	/// shared ptr type
	typedef boost::shared_ptr< FileListDownload > ptr;

	/// Constructor
	FileListDownload();
	
	/// Destructor
	virtual ~FileListDownload();

	/// Starts downloading
	virtual void start();

private:

	/// Hndler. Called when connection is established (or failes)
	void onConnected( const Error& err, const shared_ptr<DirectConnection>& pConnection );
	
	/// Handler. Called when requested data comes in
	void onIncomingData( vector<char>& data, uint64_t offset );

	/// Handler - called when download is completed
	void downloadCompleted( const Error& err );
	
	/// Connection to source
	shared_ptr<DirectConnection> _pConnection;
	
	/// Buffer for incoming data
	vector<char> _inBuffer;
};

}

#endif // RUFUSDCFILELISTDOWNLOAD_H

// EOF


