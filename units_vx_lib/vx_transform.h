//----------------------------------------------------------------------------
// File : vx_transform.h
// Project : MacMapSuite
// Purpose : Header file : Geometry transformation
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
// 04/08/2010 creation.
//----------------------------------------------------------------------------

#ifndef _vx_transform_
#define _vx_transform_

//----------------------------------------------------------------------------

#include "vx.h"

//----------------------------------------------------------------------------

typedef struct SIMILPrm{
	double		Tx;
	double		Ty;
	double		R[6+1];
	double		EchelleX;
	double		EchelleY;
	double		ResiduX;
	double		ResiduY;
}SIMILPrm;

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif
	
//----------------------------------------------------------------------------
	
void SIMIL_Init		(	SIMILPrm* prm, 
						dvertices* fich, 
						dvertices* click);

void SIMIL_Transform(	d2dvertex* fich, 
						d2dvertex* click, 
						SIMILPrm* prm);
	
//----------------------------------------------------------------------------
	
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
