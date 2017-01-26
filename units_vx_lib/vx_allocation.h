//----------------------------------------------------------------------------
// File : vx_allocation.h
// Project : MacMapSuite
// Purpose : Header file : Geometry allocation / deallocation utils
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
// 20/08/2002 creation.
//----------------------------------------------------------------------------

#ifndef _vx_allocation_
#define _vx_allocation_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

ivertices*	ivs_new		(	int kind,
							int nv,
							int no);
void		ivs_free	(	ivertices* vxs);
dvertices*	dvs_new		(	int kind,
							int nv,
							int no);
void		dvs_free	(	dvertices* vxs);

int			sizeofivs2	(	int	nv,
							int	no);
int			sizeofivs3	(	int	nv,	
							int	no);
int			ivssize		(	ivertices* vxs, 
							int* hSz, 
							int* vSz, 
							int* oSz);
int			sizeofdvs2	(	int nv,	
							int no);
int			sizeofdvs3	(	int nv,	
							int no);
int			dvssize		(	dvertices* vxs,	
							int* hSz,
							int* vSz,
							int* oSz);

int			ivs2ivs		(	ivertices* vin,	
							ivertices**	vxs);
int			dvs2dvs		(	dvertices* vin,	
							dvertices**	vxs);
int			ivr2ivs		(	ivx_rect* vr,
							ivertices**	vxs);
int			ivspart2ivs	(	ivertices* vin,
							int part,			// in [0..n-1]
							ivertices**	vxs);
int			dvspart2dvs	(	dvertices* vin,
							int part,			// in [0..n-1]
							dvertices**	vxs);
ivertices*  nth_ivs		(	ivertices* vxs,
                            long part);
	
//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
