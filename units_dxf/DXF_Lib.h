//----------------------------------------------------------------------------
// File : DXF_Lib.h
// Project : MacMapSuite
// Purpose : Header file : Text file parser
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
// 01/01/2000 creation.
// 04/12/2007 Portage OS X et MacMapSuite.framework.
//----------------------------------------------------------------------------

#ifndef _DXF_LIB_
#define _DXF_LIB_

//----------------------------------------------------------------------------

#include	<limits.h>
#include	<math.h>
#include	<stdio.h>

#include	<MacMapSuite/bStdTable.h>

//----------------------------------------------------------------------------

// INCONNU
#define	_UNKNOWN		0
#define	UNKNOWN			"UNKNOWN"

// EN-TETES
#define	_HEADER			1
#define	_CLASSES		2
#define	_TABLES			3
#define	_BLOCKS			4
#define	_ENTITIES		5
#define	_OBJECTS		6
#define	_THUMBNAILIMAGE	7

#define	HEADER			"HEADER"
#define	CLASSES			"CLASSES"
#define	TABLES			"TABLES"
#define	BLOCKS			"BLOCKS"
#define	ENTITIES		"ENTITIES"
#define	OBJECTS			"OBJECTS"
#define	THUMBNAILIMAGE	"THUMBNAILIMAGE"


// GROUP CODES
#define	___0			0
#define	___1			1
#define	___2			2
#define	___3			3
#define	___5			5
#define	___6			6
#define	___7			7
#define	___8			8
#define	___9			9
#define	__10			10
#define	__11			11
#define	__12			12
#define	__13			13
#define	__20			20
#define	__21			21
#define	__22			22
#define	__23			23
#define	__30			30
#define	__31			31
#define	__32			32
#define	__33			33
#define	__38			38
#define	__39			39
#define	__40			40
#define	__41			41
#define	__42			42
#define	__43			43
#define	__50			50
#define	__51			51
#define	__53			53
#define	__62			62
#define	__66			66
#define	__70			70
#define	__71			71
#define	__72			72
#define	__73			73
#define	__74			74
#define	__100			100
#define	__210			210
#define	__220			220
#define	__230			230
#define	__999			999

// DICTIONNAIRE
#define	_SECTION		-2000
#define	_ENDSEC			-2001
#define	__EOF			-2002
#define	_CONTINUOUS		-2003
#define	_STANDARD		-2004

#define	SECTION			"SECTION"
#define	ENDSEC			"ENDSEC"
#define	_EOF			"EOF"
#define	CONTINUOUS		"CONTINUOUS"
#define	STANDARD		"STANDARD"

// VERSIONS
#define	_AC1006			10
#define	_AC1009			11
#define	_AC1012			13
#define	_AC1014			14
#define	_AC1500			2000

#define	AC1006			"AC1006"
#define	AC1009			"AC1009"
#define	AC1012			"AC1012"
#define	AC1014			"AC1014"
#define	AC1500			"AC1500"


// HEADER
#define	_ACADVER		-1000
#define	_EXTMIN			-1001
#define	_EXTMAX			-1002
#define	_ANGDIR			-1003

#define	ACADVER			"$ACADVER"
#define	EXTMIN			"$EXTMIN"
#define	EXTMAX			"$EXTMAX"
#define	ANGDIR			"$ANGDIR"

// BLOCKS
#define	_BLOCK			-4000
#define	_ENDBLK			-4001

#define	BLOCK			"BLOCK"
#define	ENDBLK			"ENDBLK"


// ENTITIES
#define _3DFACE			-3000//
#define _3DSOLID		-3001
#define _ACADPROXENTITY	-3002
#define _ARC			-3003//
#define _ARCALIGNEDTEXT	-3004//
#define _ATTDEF			-3005//
#define _ATTRIB			-3006//
#define _BODY			-3007
#define _CIRCLE			-3008//
#define _DIMENSION		-3009//
#define _ELLIPSE		-3010//
#define _HATCH			-3011
#define _IMAGE			-3012
#define _INSERT			-3013//
#define _LEADER			-3014
#define _LINE			-3015//
#define _LWPOLYLINE		-3016
#define _MLINE			-3017
#define _MTEXT			-3018//
#define _OLEFRAME		-3019
#define _OLE2FRAME		-3020
#define _POINT			-3021//
#define _POLYLINE		-3022//
#define _RAY			-3023//
#define _REGION			-3024
#define _RTEXT			-3025//
#define _SEQEND			-3026//
#define _SHAPE			-3027
#define _SOLID			-3028//
#define _SPLINE			-3029
#define _TEXT			-3030//
#define _TOLERANCE		-3031
#define _TRACE			-3032//
#define _VERTEX			-3033//
#define _VIEWPORT		-3034
#define _WIPEOUT		-3035
#define _XLINE			-3036//


#define _DFACE			"3DFACE"
#define _DSOLID			"3DSOLID"
#define ACADPROXENTITY	"ACAD PROXY ENTITY"
#define ARC				"ARC"
#define ARCALIGNEDTEXT	"ARCALIGNEDTEXT"
#define ATTDEF			"ATTDEF"
#define ATTRIB			"ATTRIB"
#define BODY			"BODY"
#define CIRCLE			"CIRCLE"
#define DIMENSION		"DIMENSION"
#define ELLIPSE			"ELLIPSE"
#define HATCH			"HATCH"
#define IMAGE			"IMAGE"
#define INSERT			"INSERT"
#define LEADER			"LEADER"
#define LINE			"LINE"
#define LWPOLYLINE		"LWPOLYLINE"
#define MLINE			"MLINE"
#define MTEXT			"MTEXT"
#define OLEFRAME		"OLEFRAME"
#define OLE2FRAME		"OLE2FRAME"
#define POINT			"POINT"
#define POLYLINE		"POLYLINE"
#define RAY				"RAY"
#define REGION			"REGION"
#define RTEXT			"RTEXT"
#define SEQEND			"SEQEND"
#define SHAPE			"SHAPE"
#define SOLID			"SOLID"
#define SPLINE			"SPLINE"
#define TEXT			"TEXT"
#define TOLERANCE		"TOLERANCE"
#define TRACE			"TRACE"
#define VERTEX			"VERTEX"
#define VIEWPORT		"VIEWPORT"
#define WIPEOUT			"WIPEOUT"
#define XLINE			"XLINE"

// SUBCLASS MARKERS
#define	ACDBPOINT		"AcDbPoint"
#define	ACDBTEXT		"AcDbText"
#define	ACDBVERTEX		"AcDbVertex"
#define	ACDB2DVERTEX	"AcDb2dVertex"
#define	ACDB2DPOLYLINE	"AcDb2dPolyline"
#define	ACDBENTITY		"AcDbEntity"

#define	DXF_NOERR		0

// VERTEX
#define FIRST			0
#define SECUND			1
#define THIRD			2
#define FOURTH			3
#define EXTRUSION		200

// STRUCTURES
typedef struct VERTEX_Rec{
	double	 	x;
	double	 	y;
	double	 	z;
}VERTEX_Rec;

typedef struct DXFObject_Rec{
	int			clss;
	char		layer[256];
	char		subclass[256];
	char		name[256];
	int			handle;
	int			color;
	int			flag;
	double		size;
	double		scale;
	double		rotation;
	double		user[6];
	VERTEX_Rec	extrusion;
	int			nvertices;
	VERTEX_Rec	vertices[1];
}DXFObject_Rec;
typedef DXFObject_Rec *DXFObject;

typedef struct SEC_Rec{
	int			err;
	int			begin;
	int			end;
}SEC_Rec;

typedef struct HDR_Rec{
	SEC_Rec			sec;
	int				acadver;
	double			angdir;
	VERTEX_Rec		extmax;
	VERTEX_Rec		extmin;
}HDR_Rec;

typedef struct BLOCK_Rec{
	int			handle;
	char		layer[256];
	char		bn1[256];
	char		bn2[256];
	char		path[256];
	VERTEX_Rec	base;
	int			nshapes;
	DXFObject*	shapes;
}BLOCK_Rec;
typedef BLOCK_Rec *BLOCK_Handle;

typedef struct BLK_Rec{
	SEC_Rec			sec;
	int				nblks;
	BLOCK_Handle*	blks;
}BLK_Rec;

typedef struct TAG_Rec{
	int			pos;
	int			err;
	int			last;
	int			cod;
	char		cnt[256];
}TAG_Rec;

typedef struct DXF_Rec{
	bStdTable*		tbl;
	int				curs;
	HDR_Rec			header;
	SEC_Rec			classes;
	SEC_Rec			tables;
	BLK_Rec			blocks;
	SEC_Rec			entities;
	SEC_Rec			objects;
	SEC_Rec			thumbnail;
	TAG_Rec			tag;
}DXF_Rec;
typedef DXF_Rec *DXF_Handle;

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

void		DXF_swapd			(	double *a, 
									double *b);

void		DXF_EntityTagName	(	int clss,
									char* s);
int			DXF_EqualVertex		(	VERTEX_Rec *v1, 
									VERTEX_Rec *v2);

DXF_Handle	DXF_Create			(	char* name,	
									char* path,	
									int world);
DXF_Handle	DXF_Open			(	char* name,	
									char* path);
DXF_Handle	DXF_fCreate			(	char* name,	
									char* path,	
									int world,
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid);
DXF_Handle	DXF_fOpen			(	char* name,	
									char* path,
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid);

int 		DXF_Close			(	DXF_Handle	DXF);

int			DXF_WriteHeader		(	DXF_Handle DXF);

int			DXF_SetSection		(	DXF_Handle	DXF,
									int which);
int			DXF_OpenSection		(	DXF_Handle	DXF,
									int which);
int			DXF_CloseSection	(	DXF_Handle	DXF);
int			DXF_CloseDoc		(	DXF_Handle	DXF);
int			DXF_WriteComment	(	DXF_Handle	DXF,
									char* s);
DXFObject	DXF_ReadNext		(	DXF_Handle DXF);
int			DXF_WriteNext		(	DXF_Handle DXF,	
									DXFObject shape);
int			DXF_Insert2Shapes	(	DXF_Handle DXF,	
									DXFObject insert,
									DXFObject **shapes,	
									int *count);

DXFObject	DXF_NewObject		(	int DXFType,
									int nvertices,	
									VERTEX_Rec *vertices);
void		DXF_DestroyObject	(	DXFObject shape);

//----------------------------------------------------------------------------

VERTEX_Rec*	DXF_VXS2VERTEX		(	DXF_Handle DXF,
									int closed,
									ivertices* vxs);
ivertices* DXF_VERTEX2VXS		(	DXF_Handle DXF,
									DXFObject shape);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
