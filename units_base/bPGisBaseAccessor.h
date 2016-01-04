//----------------------------------------------------------------------------
// File : bPGisBaseAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Postgis database base class accessor version 3.0.0
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
// 04/05/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bPGisBaseAccessor__
#define __bPGisBaseAccessor__

//----------------------------------------------------------------------------

#include "bv300BaseAccessor.h"

//----------------------------------------------------------------------------

class bPGisBaseAccessor : public bv300BaseAccessor{
	public:
		bPGisBaseAccessor 					(	const char* hpath, 
												int* status);
		bPGisBaseAccessor 					(	const char* hpath, 
												int* status,
												int kind,
												int srid,
												double reso,
												double u2m,
												const char* name,												
												const char* data);
		virtual ~bPGisBaseAccessor			(	);
		virtual int signature				(	);
		
	protected:
		
	private:
		virtual void make_data				(	char* data);
		virtual void make_data				(	char* data, 
												const char* path);
};

//----------------------------------------------------------------------------

#endif


