//----------------------------------------------------------------------------
// File : vx.h
// Project : MacMapSuite
// Purpose : Header file : MacMap geometry defines
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
// 25/05/2001 creation.
//----------------------------------------------------------------------------

#ifndef _vx_
#define _vx_

//----------------------------------------------------------------------------
// Int h/v geometry
typedef	struct	ivx_rect{
	int		top;
	int		left;
	int		bottom;
	int		right;
}ivx_rect;

typedef	struct	ivx_cube{ 
	int		top;
	int		left;
	int		min;
	int		bottom;
	int		right;
	int		max;
}ivx_cube;

typedef struct i2dvertex{
	int		h;
	int		v;
}i2dvertex;

typedef struct i3dvertex{
	int		h;
	int		v;
	int		m;
}i3dvertex;

typedef struct ivertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	union{
	i2dvertex	vx2[1];
	i3dvertex	vx3[1];
	}vx;
}ivertices;

typedef struct i2dvertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	i2dvertex	vx[0];
}i2dvertices;

typedef struct i3dvertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	i3dvertex	vx[0];
}i3dvertices;
// OFFSETS :
// - No offset : no=0, offs=NULL;
// - Offset : offs=start indexes, offs[0]==0

//----------------------------------------------------------------------------
// Double x/y geometry
typedef	struct	dvx_rect{ 
	double	xmin;
	double	ymin;
	double	xmax;
	double	ymax;
}dvx_rect;

typedef	struct	dvx_cube{ 
	double	xmin;
	double	ymin;
	double	zmin;
	double	xmax;
	double	ymax;
	double	zmax;
}dvx_cube;

typedef struct d2dvertex{
	double	x;
	double	y;
}d2dvertex;

typedef struct d3dvertex{
	double	x;
	double	y;
	double	z;
}d3dvertex;

typedef struct dvertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	union{
	d2dvertex	vx2[1];
	d3dvertex	vx3[1];
	}vx;
}dvertices;

typedef struct d2dvertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	d2dvertex	vx[0];
}d2dvertices;

typedef struct d3dvertices{
	int			sign;
	int			nv;
	int			no;
	int*		offs;
	d3dvertex	vx[0];
}d3dvertices;

//----------------------------------------------------------------------------
// I/O utilities

typedef struct ivertices32{
    int			sign;
    int			nv;
    int			no;
    int         offs;
    union{
        i2dvertex	vx2[1];
        i3dvertex	vx3[1];
    }vx;
}ivertices32;

//----------------------------------------------------------------------------

#define	_2D_VX			2
#define	_3D_VX			3

#define __BOUNDS_MAX__	(INT_MAX/2)

#define __FUZZY_DIST__	(sqrt(2.0)/2.0)
#define __TEST_INT__	2398567903

enum{
	kVXNoKind	=0,
	kVXPoint	=1,
	kVXPolyline	=2,
	kVXPolygon	=4,
	
	kVXText		=3,
	kVXRaster	=5
};

#define kTagPolygon		"MULTIPOLYGON"
#define kTagLinestring	"MULTILINESTRING"
#define kTagPoint		"MULTIPOINT"
#define kTagText		"TEXT"
#define kTagRaster		"RASTER"

//----------------------------------------------------------------------------

#endif
