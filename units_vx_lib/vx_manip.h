//----------------------------------------------------------------------------
// File : vx_manip.h
// Project : MacMapSuite
// Purpose : Header file : Geometry modification utils
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
// 16/08/2005 creation.
//----------------------------------------------------------------------------

#ifndef _vx_manip_
#define _vx_manip_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

void	ivs_rotate_anchor		(	ivertices* vxs, 
									double rad, 
									i2dvertex* o);
void	ivs_rotate				(	ivertices* vxs, 
									double rad);
void	ivs_scale_anchor		(	ivertices* vxs, 
									double cx, 
									double cy, 
									i2dvertex* o);
void	ivs_scale				(	ivertices* vxs, 
									double cx, 
									double cy);
void	ivs_move				(	ivertices* vxs, 
									int dx, 
									int dy);
void	dvs_move				(	dvertices* vxs, 
									double dx, 
									double dy);
void	ivs_shift				(	ivertices** vxs, 
									double d);
// get a "vx" on "vxs" at "coef" of dist from start 
void	ivs_line_pos			(	ivertices* vxs, 
									i2dvertex* vx, 
									double coef);
void	ivs_centro				(	ivertices* vxs, 
									i2dvertex* vx);
// get a "vx" on "vxs" at mid dist from start 
void	ivs_centro_line			(	ivertices* vxs, 
									i2dvertex* vx);
void	ivs_centro_surf			(	ivertices* vxs, 
									i2dvertex* vx);
void	ivs_reverse				(	ivertices* vxs);
void	ivs_simplify_with_angle	(	ivertices** vxs,	
									double a);			// a in radian
void	ivs_cut_miter_with_angle(	ivertices** vxs,	
									double a);			// a in radian

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
