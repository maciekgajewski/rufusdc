// Copyright (C) 2008 Maciek Gajewski <maciej.gajewski0@gmail.com>
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
#ifndef RUFUSDCOPERATION_H
#define RUFUSDCOPERATION_H

#include <string>
#include <boost/signals.hpp>

using namespace std;

namespace RufusDc
{

/**
* @brief DC client operation.
* This class contains description of any operation executed by DC client. Operation has its name, progress,
* and current status.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Operation
{

	public:
		
		/// Operation status types
		enum Status
		{
			Running,       ///< running
			Completed,     ///< completed successfully
			Failed         ///< failed
		};
		
		Operation( const string& description );
		virtual ~Operation();
		
		// signals
		boost::signal< void ()> signalChanged; ///< Operation status changed
		
		// getters
		
		/// Returns operation description
		string description() const { return _description; }
	
		/// Returns operation progress [%]
		int progress() const { return _progress; }
		
		/// Returns operation status
		Status status() const { return _status; }
		
		/// Returns status description
		string statusDescription() const { return _statusDescription; }
		
		// setters
		
		void setStatus( Status s );
		void setStatusDescription( const string& sd  );
		void setProgress( int p );
		
		// bulk change support
		/**
		 * @brief Begins bulk change.
		 * No signal will be mitted until endChange is called.
		 */
		void beginChange();
		
		/**
		 * @brief Ends bulk change.
		 * Will emit signal if operation state changed since last beginChange()
		 */
		void endChange();
	
	private:
	
		void changed();             ///< Notifies about change
	
		string _description;        ///< Operation description
		string _statusDescription;  ///< Status description
		int    _progress;           ///< Progress
		Status _status;             ///< Status
		int    _bulkChange;         ///< Counter of 'bulk change' calls
		bool   _changed;            ///< Flag used to dekjet change between begin/end change calls

};

}

#endif // RUFUSDCOPERATION_H

// EOF


