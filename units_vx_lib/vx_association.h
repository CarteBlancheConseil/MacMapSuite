//----------------------------------------------------------------------------
// File : vx_association.h
// Project : MacMapSuite
// Purpose : Header file : Geometry association utils
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
// 16/12/2005 creation.
//----------------------------------------------------------------------------

#ifndef _vx_association_
#define _vx_association_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

ivertices*	ivs_associate_as_line	(	ivertices* vsa, 
										ivertices* vsb, 
										double dmax);
ivertices*	ivs_associate_as_surf	(	ivertices* vsa, 
										ivertices* vsb, 
										double dmax);

ivertices*	ivs_intersect_as_line	(	ivertices* vsa, 
										ivertices* vsb);
ivertices*	ivs_intersect_as_surf	(	ivertices* vsa, 
										ivertices* vsb);
ivertices*  ivs_clip_line_in_surf   (   ivertices* lin,
                                        ivertices* gon);
ivertices*  ivs_clip_line_out_surf  (   ivertices* lin,
                                        ivertices* gon);

ivertices*	ivs_convexify			(	i2dvertex*	vx,
										int n);
ivertices*	ivs_polygonize			(	i2dvertex* vx, 
										int n);
										
void		ivs_split_surf			(	ivertices* srf, 
										ivertices* lin,
										ivertices** ra,
										ivertices** rb);
										
ivertices*	ivs_bufferize_surf		(	ivertices* vsa, 
										double d);
ivertices*	ivs_bufferize_line		(	ivertices* vsa, 
										double d);
ivertices*	ivs_bufferize_point		(	ivertices* vsa, 
										double d);

ivertices*	ivs_split_surf_with_line(	ivertices* gon,
										ivertices* lin);

int         ivs_build_point_offsets (   ivertices* vxs);
	
//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
