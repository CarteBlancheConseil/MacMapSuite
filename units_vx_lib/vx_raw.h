//----------------------------------------------------------------------------
// File : vx_raw.h
// Project : MacMapSuite
// Purpose : Header file : Geometry raw utils
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
// 27/10/2006 creation.
//----------------------------------------------------------------------------

#ifndef _vx_raw_
#define _vx_raw_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

#define beetween(a,b,c)	(((a<=b)&&(b<=c))||((a>=b)&&(b>=c)))

//----------------------------------------------------------------------------

int		vx_quad			(	int dh, 
							int	dv);
int		vx_cquad		(	int oq, 
							int q, 
							int* lq, 
							double* oh, 
							double* ov, 
							int dh, 
							int dv);

void	vx_theta_qsort	(	i2dvertex* r, 
							i2dvertex* t, 
							int n);

int		vx_siam			(	const i2dvertex *p0, 
							const i2dvertex *p1, 
							const i2dvertex *p2);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
