//----------------------------------------------------------------------------
// File : PGis_Utils.h
// Project : MacMapSuite
// Purpose : Header file : Postgis utils
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
// 29/06/2005 creation.
//----------------------------------------------------------------------------

#ifndef __PGis_utils__
#define __PGis_utils__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

#define	POINTTYPE			1
#define	LINETYPE			2
#define	POLYGONTYPE			3
#define	MULTIPOINTTYPE		4
#define	MULTILINETYPE		5
#define	MULTIPOLYGONTYPE	6
#define	COLLECTIONTYPE		7

#define WKBZOFFSET			0x80000000
#define WKBMOFFSET			0x40000000
#define WKBSRIDFLAG			0x20000000
#define ZMFLAG(x)			(((x)&((WKBZOFFSET)+(WKBMOFFSET)))>>30)

//----------------------------------------------------------------------------

typedef struct gPoint{
	double x; 
	double y; 
	double z;
}gPoint;

typedef struct Ring{
	gPoint*			list;	// list of points
	struct Ring*	next;	// next rings
	int				n;		// number of points in list
	unsigned int	linked;	// number of next rings
}Ring;

typedef unsigned char byte;
//typedef unsigned long int uint32;

//----------------------------------------------------------------------------

extern char outchr[];

//----------------------------------------------------------------------------

int		PIP				(	gPoint P, 
							gPoint* V, 
							int n);
char	getEndianByte	(	void);
byte	getbyte			(	byte* c);
void	skipbyte		(	byte** c);
byte	popbyte			(	byte** c);
UInt32	popint			(	byte** c,
							char swap);
UInt32	getint			(	byte* c);
void	skipint			(	byte** c);
double	popdouble		(	byte** c,
							char swap);
void	skipdouble		(	byte** c);
void	dump_wkb		(	byte* wkb);
byte*	HexDecode		(	byte* hex);
int		reverse_points	(	int num_points,
							double* x, 
							double* y, 
							double* z, 
							double* m);
int		is_clockwise	(	int num_points,
							double* x,
							double* y);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
