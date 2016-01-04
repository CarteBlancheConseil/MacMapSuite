//----------------------------------------------------------------------------
// File : vx_conv_geos.h
// Project : MacMapSuite
// Purpose : Header file : vx <-> GEOS conversion
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
// 11/04/2007 creation.
//----------------------------------------------------------------------------

#ifndef _vx_conv_geos_
#define _vx_conv_geos_

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

#include "vx.h"
#include "geos_c.h"


//----------------------------------------------------------------------------

GEOSGeom	vxs2geosxgon	(	ivertices* vxs);
ivertices*	geosxgon2vxs	(	GEOSGeom xgon);

GEOSGeom	vxs2geosxlin	(	ivertices* vxs);
ivertices*	geosxlin2vxs	(	GEOSGeom xlin);

GEOSGeom	vxs2geosxpoint	(	ivertices* vxs);
ivertices*	geosxpoint2vxs	(	GEOSGeom xlin);

ivertices*	geoscollect2vxs	(	GEOSGeom collect);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
