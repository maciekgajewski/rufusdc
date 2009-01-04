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
#ifndef KRUFUSDCCLIENTTHREADHUB_H
#define KRUFUSDCCLIENTTHREADHUB_H

// std & boost
#include <string>
#include <boost/shared_ptr.hpp>

// rufusdc
#include "rufusdc/hub.h"

// Qt
#include <QObject>
#include <QMap>
#include <QSet>
#include <QMutex>
class QTextCodec;

// local
#include "userinfo.h"

namespace KRufusDc
{

using namespace boost;

class Hub;

/**
* @brief Hub wrapper part operating in cleint's thread.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ClientThreadHub : public QObject
{
	Q_OBJECT

public:
	
	ClientThreadHub( Client* _pParent, const shared_ptr<RufusDc::Hub>& pHub, QObject* qparent = 0 );
	
	~ClientThreadHub();

	
	/// Sets encoding used to convert incoming messages to QString's unicode.
	void setHubEncoding( const QByteArray& name );
	
	/// Returns all users currently connected to hub
	/// @returns User list sorted by nick (case-insensitive)
	QList<UserInfo> getUsers();

	/// Gets changed user containers. Clears internal buffers
	void getChangedUsers
		( QMap< QString, UserInfo >&   added
		, QMap< QString, UserInfo >&   modified
		, QSet< QString>&               removed
		);
	
	// boost slots
	
	/// Boost slot - called from within worker thread
	void wtMessage( int type, const std::string& msg );
	
	void wtUserAdded( const RufusDc::UserInfo& info );
	void wtUserModified( const RufusDc::UserInfo& info );
	void wtUserRemoved( const std::string& nick );
	
Q_SIGNALS:
	/// Boost slot - called from within worker thread
	void signalWtMessage( int type, const QString& msg );

public Q_SLOTS:

	// slots from UI thread
	
	void utRequestFileList( const QString& nick );

private:

	QTextCodec* _pCodec; ///< Text coded used to decode incoming messages
	
	// TODO remove if not nneded
	//Hub* _pParent; ///< Parent
	Client* _pParent;
	
	/// Underlying RufusDc::Hub object
	shared_ptr<RufusDc::Hub> _pHub;
	
	// user change tracking
	QMap< QString, UserInfo > _addedUsers;
	QMap< QString, UserInfo > _modifiedUsers;
	QSet< QString> _removedUsers;
	
	QMutex _userMutex;
};

}

#endif // KRUFUSDCCLIENTTHREADHUB_H

// EOF


