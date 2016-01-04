//----------------------------------------------------------------------------
// File : vx_measure.h
// Project : MacMapSuite
// Purpose : Header file : Geometry measurement utils
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

#ifndef _vx_measure_
#define _vx_measure_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

typedef enum dst_end_kind{
	kDistEndKindAC,
	kDistEndKindAD,
	kDistEndKindBC,
	kDistEndKindBD
}dst_end_kind;
 
//----------------------------------------------------------------------------

double	ivx2_surf		(	i2dvertex* vx,
							int n);
double	ivx3_surf		(	i3dvertex* vx,
							int n);
int		ivx2_in_surf	(	i2dvertex* vx, 
							i2dvertex* vs, 
							int n, 
							int limit);
// returns	: 1 if clockwise,
//			: -1 if anticlockwise
//			: 0 if error
int		ivx2_clockwise	(	i2dvertex* vx,
							int n);
int		ivx3_clockwise	(	i3dvertex* vx,
							int n);
double	ivs_length		(	ivertices*	vs);
double	ivx2_length		(	i2dvertex*	vs,
							int nv);
double	ivs_surf		(	ivertices*	vs);
int		ivx_in_ivs		(	i2dvertex*	vx,
							ivertices*	vs,
							int	limit);
double	ivx2_dist		(	i2dvertex*	vxa,
							i2dvertex*	vxb);
double	ivx3_dist		(	i3dvertex*	vxa,
							i3dvertex*	vxb);
double	vx2vec_dist		(	i2dvertex*	vx,
							i2dvertex*	vxa,
							i2dvertex*	vxb);
double	ivx2ivs_dist	(	i2dvertex*	vx,
							ivertices*	vs);

// Minimal distance
// vsa	<- a vertices
// vsb	<- b vertices
// returns min dist
double	vxs_dist		(	ivertices*	vsa,
							ivertices*	vsb);

// Minimal distance at ends 
// vsa	<- a vertices
// vsb	<- b vertices
// k	-> dist kind
// returns min dist
double	vxs_dist_end	(	ivertices*	vsa,
							ivertices*	vsb,
							dst_end_kind* k);
							
int		ivs_multigon	(	ivertices*	vs);

// result in radians
// equal to xy_vx_angle
double	vx_angle		(	i2dvertex*	vxa,
							i2dvertex*	vxb);
// result in radians
double	xy_vx_angle		(	i2dvertex*	vxa,
							i2dvertex*	vxb);
// result in radians
double	hv_vx_angle		(	i2dvertex*	vxa,
							i2dvertex*	vxb);
// result in radians
double	vx_theta		(	i2dvertex*	vxa,
							i2dvertex*	vxb);
int		ivr_sect		(	ivx_rect*	vr1,
							ivx_rect*	vr2,
							ivx_rect*	vr);
void	ivr_union		(	ivx_rect*	vr1,
							ivx_rect*	vr2,
							ivx_rect*	vr);
void	ivr_inset		(	ivx_rect*	vr,
							int			h,
							int			v);
int		ivr_in_ivr		(	ivx_rect*	vr1,
							ivx_rect*	vr2);
int		eq_ivx2			(	i2dvertex*	vx1,
							i2dvertex*	vx2);
int		eq_ivx3			(	i3dvertex*	vx1,
							i3dvertex*	vx2);
int		eq_dvx2			(	d2dvertex*	vx1,
							d2dvertex*	vx2);
int		eq_dvx3			(	d3dvertex*	vx1,
							d3dvertex*	vx2);
int		eq_ivr			(	ivx_rect*	vr1,
							ivx_rect*	vr2);

int		ivx2_closed		(	i2dvertex* ring, 
							int n);
int		ivs_closed		(	ivertices*	vxs);
// Returns bounds of vertices
void	ivs_bounds		(	ivertices*	vxs,
							ivx_rect* vr);
// Returns midle point of vr
void	ivr_mid			(	ivx_rect* vr,
							i2dvertex*	vx);
// Returns 1 if vr is not empty, 0 otherwise
int		ivr_ok			(	ivx_rect* vr);

int		ivx2_sect		(	i2dvertex* aa,
							i2dvertex* ab,
							i2dvertex* ba,
							i2dvertex* bb);

void	ivx2_get_sect	(	i2dvertex* aa,
							i2dvertex* ab,
							i2dvertex* ba,
							i2dvertex* bb,
							i2dvertex* r);
					
int		ivx2_dir		(	i2dvertex* va,
							i2dvertex* vb);
int		ivx2_in_ivr		(	ivx_rect*	vr,	
							i2dvertex*	vx);
	
//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
