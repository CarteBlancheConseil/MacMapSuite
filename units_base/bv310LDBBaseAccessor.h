//----------------------------------------------------------------------------
// File : bv310LDBBaseAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Local database base class accessor version 3.1.0 
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
// 04/07/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bv310LDBBaseAccessor__
#define __bv310LDBBaseAccessor__

//----------------------------------------------------------------------------

#include "bv310BaseAccessor.h"

//----------------------------------------------------------------------------

class bv310LDBBaseAccessor : public bv310BaseAccessor{
	public:
		bv310LDBBaseAccessor 				(	const char* hpath, 
												int wanted_srid,
												double wanted_reso,
												int* status);
		bv310LDBBaseAccessor 				(	const char* hpath, 
												int* status,
												int kind,
												int srid,
												double reso,
												double u2m,
												const char* name,												
												const char* data);
		virtual ~bv310LDBBaseAccessor		(	);

		virtual int signature				(	);
		
	protected:
		virtual int lnk_open				(	);
		virtual void lnk_close				(	);
	
	private:
		
};

//----------------------------------------------------------------------------

#endif


