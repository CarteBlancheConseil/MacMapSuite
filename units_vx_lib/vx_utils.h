//----------------------------------------------------------------------------
// File : vx_utils.h
// Project : MacMapSuite
// Purpose : Header file : Geometry utils
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

#ifndef _vx_utils_
#define _vx_utils_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

#define		i2d_vx2seg(a)	((i2dseg*)a)

//----------------------------------------------------------------------------

// Calculate new pos of "vx" projected on "vxa" to "vxb" segment
// Returns minimal distance between vx and "vxa" to "vxb" segment
//  -> vx : initial vertex
//  -> vxa : from vertex
//  -> vxb : to vertex
// <-  vr : projected vertex
// <-  r : if <=0 or >=1, vr exists in vs
double		ivx_projonseg	(	i2dvertex* vx,
								i2dvertex* vxa,
								i2dvertex* vxb,
								i2dvertex* vr,
								double* r);
						
void		ivrset			(	ivx_rect*	vr, 
								int	left,	
								int	top,	
								int right,	
								int	bottom);
								
// Testing geometry : vertices number, offsets and signature
int			ivs_good		(	ivertices* vxs);
// Testing geometry : bounding box in __BOUNDS_MAX__
int			ivs_goodgeo		(	ivertices* vxs);
// Assumes that vxs is a polygon
void		ivs_closegeom	(	ivertices* vxs);

int         ivs_integrity   (   int kind,
                                ivertices* vxs);

void		ivx_angleproj	(	i2dvertex*	vx,	
								i2dvertex* vx1, 
								i2dvertex*	vx2, 
								double rad);

// Calculate new pos of "vx" projected on "vs" geometry
// Returns minimal distance between vx and "vxa" to "vxb" segment
//  -> vx : initial vertex
//  -> vs : reference geometry
// <-  vr : projected vertex
// <-  idx : in [0..n-1]
// <-  r : if <=0 or >=1, vr exists in vs
double		ivx_proj		(	i2dvertex*	vx,
								ivertices*	vs,
								i2dvertex*	vr,
								int*		idx,	
								double*		r);
// Calculate new pos of "vx" projected at "d" distance for "rad" angle
//  -> vx : initial vertex
// <-  vr : projected vertex
//  -> d : distance in database resolution and unit
//  -> rad : angle in radian
// WARNING : rad for h/v geometry (v from top to bottom)
void		ivx_proj_at_dist(	i2dvertex*	vx,
								i2dvertex*	vr,
								double		d,
								double		rad);
/*												
i2dvertex*	ivx2_sub		(	ivertices* vxs, 
								int idx, 
								int* count);
i3dvertex*	ivx3_sub		(	ivertices* vxs, 
								int idx, 
								int* count);
*/

ivertices*	ivx2_add		(	ivertices* vxs, 
								i2dvertex* vx);
ivertices*	ivx3_add		(	ivertices* vxs, 
								i3dvertex* vx);
								
dvertices*	dvx2_add		(	dvertices* vxs, 
								d2dvertex* vx);
dvertices*	dvx3_add		(	dvertices* vxs, 
								d3dvertex* vx);
								
ivertices*	ivx2_insert		(	ivertices* vxs, 
								i2dvertex* vx, 
								int idx);		//	idx in [0..n-1]
ivertices*	ivx3_insert		(	ivertices* vxs, 
								i3dvertex* vx, 
								int idx);		//	idx in [0..n-1]

ivertices*	ivx_remove		(	ivertices* vxs, 
								int idx);		//	idx in [0..n-1]

// split 'vxs' in two parts at point 'idx'
void		ivs_split		(	ivertices* vxs, 
								int idx,		//	idx in [0..n-1]
								ivertices** vsa,
								ivertices** vsb);

// add 'vsb' to 'vsa', strip offsets
// returns realloc of 'vsa'
ivertices*	ivs_concat		(	ivertices* vsa, 
								ivertices* vsb);
								
// add 'vsb' to 'vsa', create offsets if needed
// returns realloc of 'vsa'
ivertices*	ivs_group		(	ivertices* vsa, 
								ivertices* vsb);
// add 'vsb' to 'vsa', create offsets if needed
// returns realloc of 'vsa'
dvertices*	dvs_group		(	dvertices* vsa, 
								dvertices* vsb);
								
// strip offsets of 'vxs'
void		ivs_strip_offs	(	ivertices* vs);



int			ivs_n_vxs		(	ivertices* vxs);
int			ivs_n_parts		(	ivertices* vxs);
// Return part number (in [0..n-1]) for vertex number "idx" (in [0..n-1])
int			ivs_part_num	(	ivertices* vxs,
								int idx);
i2dvertex*	ivs2_part		(	ivertices* vxs, 
								int idx,		//	idx in [0..n-1]
								int* nvx);
i3dvertex*	ivs3_part		(	ivertices* vxs, 
								int idx,		//	idx in [0..n-1]
								int* nvx);


int			dvs_n_vxs		(	dvertices* vxs);
int			dvs_n_parts		(	dvertices* vxs);
// Return part number (in [0..n-1]) for vertex number "idx" (in [0..n-1])
int			dvs_part_num	(	dvertices* vxs,
								int idx);
d2dvertex*	dvs2_part		(	dvertices* vxs, 
								int idx,		//	idx in [0..n-1]
								int* nvx);
d3dvertex*	dvs3_part		(	dvertices* vxs, 
								int idx,		//	idx in [0..n-1]
								int* nvx);


ivertices*	ivs_pack		(	ivertices* vxs);
ivertices*  ivs_unpack32    (   ivertices32* vxs32);

ivertices*	ivs_unpack		(	ivertices* vxs);
dvertices*	dvs_pack		(	dvertices* vxs);
dvertices*	dvs_unpack		(	dvertices* vxs);

double		deg2rad			(	double deg);
double		rad2deg			(	double rad);

void		vx_i2d			(	d2dvertex* dvx, 
								i2dvertex* ivx, 
								double reso);
void		vx_d2i			(	i2dvertex* ivx, 
								d2dvertex* dvx, 
								double reso);
void		vxs_i2d			(	dvertices** dvxs, 
								ivertices* ivxs, 
								double reso);
void		vxs_d2i			(	ivertices** ivxs, 
								dvertices* dvxs,
								double reso);

// returns 0 if vx not in vxs
// returns [1..n] if exists
// 3D : N, PARTS : Y
int			ivx2_exists		(	ivertices* vxs, 
								i2dvertex* vx);
// 3D : N, PARTS : N
void		ivs_sect_inserts(	ivertices** vsa, 
								ivertices** vsb);
// 3D : N, PARTS : N
void		ivs_shift_origin(	ivertices* vxs, 
								int o);

ivertices*	text2ivs		(	int* kind, 
								double reso,
								char* txt);
char*		ivs2text		(	int kind, 
								double reso,
								ivertices* vxs);

dvertices*	text2dvs		(	int* kind, 
								char* txt);
char*		dvs2text		(	int kind, 
								dvertices* vxs);
																
//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
