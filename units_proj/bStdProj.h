//----------------------------------------------------------------------------
// File : bStdProj.h
// Project : MacMapSuite
// Purpose : Header file : Proj base class (Proj4 C++ wrapper)
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

#ifndef __bStdProj__
#define __bStdProj__

//----------------------------------------------------------------------------

#include <MacMapSuite/vx.h>

//----------------------------------------------------------------------------

class bStdProj{
public:		
	bStdProj				(	const char* projdef);
	bStdProj				(	int srid, 
								bool *init);
	virtual ~bStdProj		(	);
	virtual bool transform	(	bStdProj& src, 
								dvertices* vxs);
	virtual bool is_latlong	(	);
	
protected:
	void* _pj;
		
private:
};

//----------------------------------------------------------------------------

class bStdProjExt : public bStdProj{
public:		
	bStdProjExt				(	int srid, 
								bool *init);
	virtual ~bStdProjExt	(	);
	
	virtual const char* name(	){return(_name);};

protected:
	char* _name;
	
private:
};

//----------------------------------------------------------------------------

#endif
