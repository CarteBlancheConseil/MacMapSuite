//----------------------------------------------------------------------------
// File : wgs84.h
// Project : MacMapSuite
// Purpose : Header file : wgs84 proj class (Proj4 C++ wrapper)
// Author : Benoit Ogier, benoit.ogier@macmap.com
//
// Copyright (C) 1997-2015 Carte Blanche Conseil.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// See the COPYING.lesser file for more information.
//
//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// 10/02/2006 creation.
//----------------------------------------------------------------------------

#ifndef __wgs84__
#define __wgs84__

//----------------------------------------------------------------------------

#include <MacMapSuite/bStdProj.h>

//----------------------------------------------------------------------------

class wgs84 : public bStdProj{
	public:		
		wgs84				();
		virtual ~wgs84		();

	protected:
		
	private:

};

//----------------------------------------------------------------------------

#endif
