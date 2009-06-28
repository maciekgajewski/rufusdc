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
#ifndef RUFUSDCSINGLETON_H
#define RUFUSDCSINGLETON_H

// std
#include <cassert>

namespace RufusDc {

/**
* Basic singleton.
* @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
template< typename T>
class Singleton
{
public:
    
    Singleton()
    {
    	assert( !_pInstance );
    	_pInstance = reinterpret_cast<T*>( this );
    }
    
    virtual ~Singleton()
    {
    	_pInstance = NULL;
    }
    
    /// Returns reference to singleton.
    static T& ref()
    {
    	assert( _pInstance );
    	return *_pInstance; 
    }
    
    /// Returns pointer to singleton
    static T* pointer()
    {
    	assert( _pInstance );
    	return _pInstance; 
    }

private:

	/// Static instance pointer
	static T* _pInstance;
};

/// Instance variable
template< typename T>
T* Singleton<T>::_pInstance = NULL;

}

#endif
