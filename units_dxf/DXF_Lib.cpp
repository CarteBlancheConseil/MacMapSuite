//----------------------------------------------------------------------------
// File : DXF_Lib.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Text file parser
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
#include "DXF_Lib.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <MacMapSuite/C_Utils.h>
#include <MacMapSuite/MacMapSuiteC.h>
#include "bTextTable.h"

#include <MacMapSuite/bTrace.h>


// ---------------------------------------------------------------------------
// OK
// ------------
void DXF_swapd(double *a, double *b){
double	c;
	
	c=(*a);
	(*a)=(*b);
	(*b)=c;
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void	DXF_Double2Str(const double *d, int decs, char* s){
	sprintf(s,"%.*f",decs,*d);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_GetGroupCode(char*	s){	
char	*cc;
int		n=strtol(s,&cc,10);
	//if(*cc!='\0'){
	if(*cc!=0){
		return(-1);
	}
	return(n);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void	DXF_GetGroupStr(int	n, char* s){	
//#error gabarit à v√©rifier
	sprintf(s,"%3d",n);
/*	while(strlen(s)<3){
		strinsert(s," ",0);
	}*/
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsBegGrpCode(char* s){
	return(DXF_GetGroupCode(s)==___0);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsBegSec(char* s){
//fprintf(stderr,"DXF_IsBegSec::testing %s\n",s);
	if(!strcmp(s,SECTION)){
		return(_SECTION);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsEndSec(char* s){
	if(!strcmp(s,ENDSEC)){
		return(_ENDSEC);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsSeqEnd(char* s){
	if(!strcmp(s,SEQEND)){
		return(_SEQEND);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsSection(char*	s){
//fprintf(stderr,"%s\n",s);
	if(!strcmp(s,HEADER))
		return(_HEADER);
	else if(!strcmp(s,CLASSES))
		return(_CLASSES);
	else if(!strcmp(s,TABLES))
		return(_TABLES);
	else if(!strcmp(s,BLOCKS))
		return(_BLOCKS);
	else if(!strcmp(s,ENTITIES))
		return(_ENTITIES);
	else if(!strcmp(s,OBJECTS))
		return(_OBJECTS);
	else if(!strcmp(s,CLASSES))
		return(_CLASSES);
	else if(!strcmp(s,THUMBNAILIMAGE))
		return(_THUMBNAILIMAGE);
	return(_UNKNOWN);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsHeaderTag(char* s){
	if(!strcmp(s,ACADVER))
		return(_ACADVER);
	else if(!strcmp(s,EXTMIN))
		return(_EXTMIN);
	else if(!strcmp(s,EXTMAX))
		return(_EXTMAX);
	else if(!strcmp(s,ANGDIR))
		return(_ANGDIR);
	return(_UNKNOWN);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsEntityTag(char* s){
	if(!strcmp(s,_DFACE))
		return(_3DFACE);
	else if(!strcmp(s,_DSOLID))
		return(_3DSOLID);
	else if(!strcmp(s,ACADPROXENTITY))
		return(_ACADPROXENTITY);
	else if(!strcmp(s,ARC))
		return(_ARC);
	else if(!strcmp(s,ARCALIGNEDTEXT))
		return(_ARCALIGNEDTEXT);
	else if(!strcmp(s,ATTDEF))
		return(_ATTDEF);
	else if(!strcmp(s,ATTRIB))
		return(_ATTRIB);
	else if(!strcmp(s,BODY))
		return(_BODY);
	else if(!strcmp(s,CIRCLE))
		return(_CIRCLE);
	else if(!strcmp(s,DIMENSION))
		return(_DIMENSION);
	else if(!strcmp(s,HATCH))
		return(_HATCH);
	else if(!strcmp(s,IMAGE))
		return(_IMAGE);
	else if(!strcmp(s,INSERT))
		return(_INSERT);
	else if(!strcmp(s,LEADER))
		return(_LEADER);
	else if(!strcmp(s,LINE))
		return(_LINE);
	else if(!strcmp(s,LWPOLYLINE))
		return(_LWPOLYLINE);
	else if(!strcmp(s,MLINE))
		return(_MLINE);
	else if(!strcmp(s,MTEXT))
		return(_MTEXT);
	else if(!strcmp(s,OLEFRAME))
		return(_OLEFRAME);
	else if(!strcmp(s,OLE2FRAME))
		return(_OLE2FRAME);
	else if(!strcmp(s,POINT))
		return(_POINT);
	else if(!strcmp(s,POLYLINE))
		return(_POLYLINE);
	else if(!strcmp(s,RAY))
		return(_RAY);
	else if(!strcmp(s,REGION))
		return(_REGION);
	else if(!strcmp(s,RTEXT))
		return(_RTEXT);
	else if(!strcmp(s,SEQEND))
		return(_SEQEND);
	else if(!strcmp(s,SHAPE))
		return(_SHAPE);
	else if(!strcmp(s,SOLID))
		return(_SOLID);
	else if(!strcmp(s,SPLINE))
		return(_SPLINE);
	else if(!strcmp(s,TEXT))
		return(_TEXT);
	else if(!strcmp(s,TOLERANCE))
		return(_TOLERANCE);
	else if(!strcmp(s,TRACE))
		return(_TRACE);
	else if(!strcmp(s,VERTEX))
		return(_VERTEX);
	else if(!strcmp(s,VIEWPORT))
		return(_VIEWPORT);
	else if(!strcmp(s,WIPEOUT))
		return(_WIPEOUT);
	else if(!strcmp(s,XLINE))
		return(_XLINE);
	else
		return(_UNKNOWN);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_IsBlockTag(char* s){	
	if(!strcmp(s,BLOCK))
		return(_BLOCK);
	else if(!strcmp(s,ENDBLK))
		return(_ENDBLK);
	return(DXF_IsEntityTag(s));
}

// ---------------------------------------------------------------------------
// OK
// ------------
void DXF_EntityTagName(int clss, char* s){
	switch(clss){
		case (_3DFACE):
			strcpy(s,_DFACE);
			break;
		case (_3DSOLID):
			strcpy(s,_DSOLID);
			break;
		case (_ACADPROXENTITY):
			strcpy(s,ACADPROXENTITY);
			break;
		case (_ARC):
			strcpy(s,ARC);
			break;
		case (_ARCALIGNEDTEXT):
			strcpy(s,ARCALIGNEDTEXT);
			break;
		case (_ATTDEF):
			strcpy(s,ATTDEF);
			break;
		case (_ATTRIB):
			strcpy(s,ATTRIB);
			break;
		case (_BODY):
			strcpy(s,BODY);
			break;
		case (_CIRCLE):
			strcpy(s,CIRCLE);
			break;
		case (_DIMENSION):
			strcpy(s,DIMENSION);
			break;
		case (_HATCH):
			strcpy(s,HATCH);
			break;
		case (_IMAGE):
			strcpy(s,IMAGE);
			break;
		case (_INSERT):
			strcpy(s,INSERT);
			break;
		case (_LEADER):
			strcpy(s,LEADER);
			break;
		case (_LINE):
			strcpy(s,LINE);
			break;
		case (_LWPOLYLINE):
			strcpy(s,LWPOLYLINE);
			break;
		case (_MLINE):
			strcpy(s,MLINE);
			break;
		case (_MTEXT):
			strcpy(s,MTEXT);
			break;
		case (_OLEFRAME):
			strcpy(s,OLEFRAME);
			break;
		case (_OLE2FRAME):
			strcpy(s,OLE2FRAME);
			break;
		case (_POINT):
			strcpy(s,POINT);
			break;
		case (_POLYLINE):
			strcpy(s,POLYLINE);
			break;
		case (_RAY):
			strcpy(s,RAY);
			break;
		case (_REGION):
			strcpy(s,REGION);
			break;
		case (_RTEXT):
			strcpy(s,RTEXT);
			break;
		case (_SEQEND):
			strcpy(s,SEQEND);
			break;
		case (_SHAPE):
			strcpy(s,SHAPE);
			break;
		case (_SOLID):
			strcpy(s,SOLID);
			break;
		case (_SPLINE):
			strcpy(s,SPLINE);
			break;
		case (_TEXT):
			strcpy(s,TEXT);
			break;
		case (_TOLERANCE):
			strcpy(s,TOLERANCE);
			break;
		case (_TRACE):
			strcpy(s,TRACE);
			break;
		case (_VERTEX):
			strcpy(s,VERTEX);
			break;
		case (_VIEWPORT):
			strcpy(s,VIEWPORT);
			break;
		case (_WIPEOUT):
			strcpy(s,WIPEOUT);
			break;
		case (_XLINE):
			strcpy(s,XLINE);
			break;
		default:
			strcpy(s,UNKNOWN);
			break;
	}
}

// ---------------------------------------------------------------------------
// OK
// ------------
int	DXF_EqualVertex(VERTEX_Rec *v1, VERTEX_Rec *v2){
	return((v1->x==v2->x)&&(v1->y==v2->y)&&(v1->z==v2->z));
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void	DXF_ReadTag(DXF_Handle	DXF){
char	s[256];

	DXF->tag.pos=DXF->curs;
	DXF->tag.err=DXF->tbl->ReadVal(DXF->curs,1,s);
	if(DXF->tag.err){
fprintf(stderr,"DXF_ReadTag::DXF->tbl->ReadVal error %d (tag)\n",DXF->tag.err);
		return;
	}
	DXF->curs++;
	DXF->tag.cod=DXF_GetGroupCode(s);
	DXF->tag.err=DXF->tbl->ReadVal(DXF->curs,1,DXF->tag.cnt);
	if(DXF->tag.err){
fprintf(stderr,"DXF_ReadTag::DXF->tbl->ReadVal error %d (content)\n",DXF->tag.err);
		DXF->curs++;
		return;
	}
	DXF->curs++;

//fprintf(stderr,"%d->%s\n",DXF->tag.cod,DXF->tag.cnt);
}

// ---------------------------------------------------------------------------
// 
// ------------
static void	DXF_WriteTag(DXF_Handle	DXF, int cod, const char* cnt){
//_bTrace_("DXF_WriteTag",false);
char	s[256];
	
	DXF->tag.cod=cod;
	strcpy(DXF->tag.cnt,cnt);
	
	DXF_GetGroupStr(DXF->tag.cod,s);
	DXF->tag.pos=DXF->curs+1;
	DXF->tag.err=DXF->tbl->WriteVal(DXF->curs+1,1,s);
//_tm_("cod="+cod+"->"+s);
	if(DXF->tag.err){
//_te_("WriteVal for tag :"+s);
		return;
	}
	DXF->curs++;
	DXF->tag.err=DXF->tbl->WriteVal(DXF->curs+1,1,DXF->tag.cnt);
	if(DXF->tag.err){
//_te_("WriteVal for content :"+s);
		return;
	}
	DXF->curs++;
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadVers(DXF_Handle	DXF){	
	DXF_ReadTag(DXF);
	if(!strcmp(DXF->tag.cnt,AC1006))
		DXF->header.acadver=_AC1006;
	else if(!strcmp(DXF->tag.cnt,AC1009))
		DXF->header.acadver=_AC1009;
	else if(!strcmp(DXF->tag.cnt,AC1012))
		DXF->header.acadver=_AC1012;
	else if(!strcmp(DXF->tag.cnt,AC1014))
		DXF->header.acadver=_AC1014;
	else if(!strcmp(DXF->tag.cnt,AC1500))
		DXF->header.acadver=_AC1500;
	else
		DXF->header.acadver=_UNKNOWN;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_WriteVers(DXF_Handle DXF){
	DXF_WriteTag(DXF,___9,ACADVER);
	switch(DXF->header.acadver){
		case _AC1006:
			DXF_WriteTag(DXF,___1,AC1006);
			break;
		case _AC1009:
			DXF_WriteTag(DXF,___1,AC1009);
			break;
		case _AC1012:
			DXF_WriteTag(DXF,___1,AC1012);
			break;
		case _AC1014:
			DXF_WriteTag(DXF,___1,AC1014);
			break;
		case _AC1500:
			DXF_WriteTag(DXF,___1,AC1500);
			break;
	}
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// 
// ------------
static int	DXF_ReadBounds(DXF_Handle	DXF, VERTEX_Rec	*xyz){
int		pos;
	
	DXF_ReadTag(DXF);
	if((DXF->tag.err)||(DXF->tag.cod!=__10))
		return(-1);
	xyz->x=strtod(DXF->tag.cnt,NULL);
	DXF_ReadTag(DXF);
	if((DXF->tag.err)||(DXF->tag.cod!=__20))
		return(-1);
	xyz->y=strtod(DXF->tag.cnt,NULL);
	pos=DXF->curs;
	DXF_ReadTag(DXF);
	if((DXF->tag.err)||(DXF->tag.cod!=__30)){
		DXF->curs=pos;
		xyz->z=0;
	}
	else{
		xyz->z=strtod(DXF->tag.cnt,NULL);
	}
	return(DXF_NOERR);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static DXFObject DXF_EmptyObject(int nvertices){
int			i;
DXFObject	shape=NULL;
	
	shape=(DXFObject)calloc((sizeof(DXFObject_Rec)+((nvertices-1)*sizeof(VERTEX_Rec))),1);
	if(!shape){
		return(NULL);
	}
	shape->size=0;
	shape->scale=0;
	shape->rotation=0;
	shape->user[0]=0;
	shape->user[1]=0;
	shape->user[2]=0;
	shape->user[3]=0;
	shape->user[4]=0;
	shape->user[5]=0;
	shape->extrusion.x=0;
	shape->extrusion.y=0;
	shape->extrusion.z=0;
	shape->nvertices=nvertices;
	for(i=0;i<nvertices;i++){
		shape->vertices[i].x=0;
		shape->vertices[i].y=0;
		shape->vertices[i].z=0;
	}
	return(shape);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadPoint(DXF_Handle DXF, DXFObject *shape){
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_POINT;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:	
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __39:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __50:
					(*shape)->rotation=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadPolyline(DXF_Handle DXF, DXFObject *shape){
int		flg=0,sqe=0,sequend=0;
	(*shape)=DXF_EmptyObject(0);
	if(!shape)
		return(-1);
	(*shape)->clss=_POLYLINE;
	(*shape)->nvertices=0;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					if(!sqe){
						if(DXF_IsSeqEnd(DXF->tag.cnt))
							sqe=1;
						else if(!flg)
							flg=1;
					}
					else
						sequend=1;
					break;
				case ___5:
					if(!flg){
						(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					}
					break;
				case ___8:
					if(!flg)
						strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					if(flg){
						(*shape)->nvertices++;
						(*shape)=(DXFObject)realloc((*shape),(sizeof(DXFObject_Rec)+(((*shape)->nvertices-1)*sizeof(VERTEX_Rec))));
						if(!(*shape))
							return(-1);
						(*shape)->vertices[(*shape)->nvertices-1].x=0;
						(*shape)->vertices[(*shape)->nvertices-1].y=0;
						(*shape)->vertices[(*shape)->nvertices-1].z=0;
						(*shape)->vertices[(*shape)->nvertices-1].x=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __20:
					if(flg){
						(*shape)->vertices[(*shape)->nvertices-1].y=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __30:
					if(flg){
						(*shape)->vertices[(*shape)->nvertices-1].z=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __39:
					if(flg){
						(*shape)->size=strtol(DXF->tag.cnt,NULL,10);
					}
					break;
				case __62:
					if(!flg){
						(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					}
					break;
				case __70:
					if(!flg){
						(*shape)->flag=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __100:
					if(!flg){
						strcpy((*shape)->subclass,DXF->tag.cnt);
					}
					break;
				case __210:
					if(!flg){
						(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __220:
					if(!flg){
						(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __230:
					if(!flg){
						(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					}
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(!sequend)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadLWPolyline(DXF_Handle DXF, DXFObject *shape){			
	(*shape)=DXF_EmptyObject(0);
	if(!shape)
		return(-1);
	(*shape)->clss=_LWPOLYLINE;
	(*shape)->nvertices=0;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->nvertices++;
					(*shape)=(DXFObject)realloc((*shape),(sizeof(DXFObject_Rec)+(((*shape)->nvertices-1L)*sizeof(VERTEX_Rec))));
					if(!(*shape))
						return(-1);
					(*shape)->vertices[(*shape)->nvertices-1].x=0;
					(*shape)->vertices[(*shape)->nvertices-1].y=0;
					(*shape)->vertices[(*shape)->nvertices-1].z=0;
					(*shape)->vertices[(*shape)->nvertices-1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[(*shape)->nvertices-1].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[(*shape)->nvertices-1].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __38:
					(*shape)->user[0]=strtol(DXF->tag.cnt,NULL,10);
				case __39:
					(*shape)->size=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __40:
					(*shape)->user[1]=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __41:
					(*shape)->user[2]=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __42:
					(*shape)->user[3]=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __70:
					(*shape)->flag=strtod(DXF->tag.cnt,NULL);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadLine(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(2);
	if(!shape)
		return(-1);
	(*shape)->clss=_LINE;
	(*shape)->nvertices=2;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __11:
					(*shape)->vertices[1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __21:
					(*shape)->vertices[1].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __31:
					(*shape)->vertices[1].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __39:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadArc(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_ARC;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __39:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __40:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __50:
					(*shape)->user[1]=strtod(DXF->tag.cnt,NULL);
					break;
				case __51:
					(*shape)->user[2]=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadCircle(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_CIRCLE;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __39:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __40:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadEllipse(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(2);
	if(!shape)
		return(-1);
	(*shape)->clss=_ELLIPSE;
	(*shape)->nvertices=2;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __11:
					(*shape)->vertices[1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __21:
					(*shape)->vertices[1].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __31:
					(*shape)->vertices[1].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __40:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __41:
					(*shape)->user[1]=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __42:
					(*shape)->user[2]=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadTrace(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(4);
	if(!shape)
		return(-1);
	(*shape)->clss=_TRACE;
	(*shape)->nvertices=4;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __11:
					(*shape)->vertices[1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __21:
					(*shape)->vertices[1].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __31:
					(*shape)->vertices[1].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __12:
					(*shape)->vertices[2].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __22:
					(*shape)->vertices[2].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __32:
					(*shape)->vertices[2].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __13:
					(*shape)->vertices[3].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __23:
					(*shape)->vertices[3].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __33:
					(*shape)->vertices[3].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __39:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadText(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_TEXT;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___1:
					strcpy((*shape)->name,DXF->tag.cnt);
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:	
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __11:
					(*shape)->nvertices++;
					(*shape)=(DXFObject)realloc((*shape),(sizeof(DXFObject_Rec)+(((*shape)->nvertices-1)*sizeof(VERTEX_Rec))));
					if(!(*shape))
						return(-1);
					(*shape)->vertices[(*shape)->nvertices-1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __21:
					if((*shape)->nvertices>1){
							(*shape)->vertices[(*shape)->nvertices-1].y=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __31:
					if((*shape)->nvertices>1){
							(*shape)->vertices[(*shape)->nvertices-1].z=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __40:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __41:
					(*shape)->scale=strtod(DXF->tag.cnt,NULL);
					break;
				case __50:
					(*shape)->rotation=strtod(DXF->tag.cnt,NULL);
					break;
				case __51:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __71:
					(*shape)->user[1]=strtod(DXF->tag.cnt,NULL);
					break;
				case __72:
					(*shape)->user[2]=strtod(DXF->tag.cnt,NULL);
					break;
				case __73:
					(*shape)->user[3]=strtod(DXF->tag.cnt,NULL);
					break;
				case __74:
					(*shape)->user[4]=strtod(DXF->tag.cnt,NULL);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadDimension(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_DIMENSION;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___1:
					strcpy((*shape)->name,DXF->tag.cnt);
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __11:
					(*shape)->nvertices++;
					(*shape)=(DXFObject)realloc((*shape),(sizeof(DXFObject_Rec)+(((*shape)->nvertices-1)*sizeof(VERTEX_Rec))));
					if(!(*shape))
						return(-1);
					(*shape)->vertices[(*shape)->nvertices-1].x=strtod(DXF->tag.cnt,NULL);
					break;
				case __21:
					if((*shape)->nvertices>1){
							(*shape)->vertices[(*shape)->nvertices-1].y=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __31:
					if((*shape)->nvertices>1){
							(*shape)->vertices[(*shape)->nvertices-1].z=strtod(DXF->tag.cnt,NULL);
					}
					break;
				case __53:
					(*shape)->rotation=strtod(DXF->tag.cnt,NULL);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadArcAlignedText(DXF_Handle DXF, DXFObject *shape){
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_ARCALIGNEDTEXT;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___1:
					strcpy((*shape)->name,DXF->tag.cnt);
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __62:
					(*shape)->color=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __40:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __41:
					(*shape)->scale=strtod(DXF->tag.cnt,NULL);
					break;
				case __42:
					(*shape)->size=strtod(DXF->tag.cnt,NULL);
					break;
				case __50:
					(*shape)->user[1]=strtod(DXF->tag.cnt,NULL);
					break;
				case __51:
					(*shape)->user[2]=strtod(DXF->tag.cnt,NULL);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default :
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadInsert(DXF_Handle DXF, DXFObject *shape){	
	(*shape)=DXF_EmptyObject(1);
	if(!shape)
		return(-1);
	(*shape)->clss=_INSERT;
	(*shape)->nvertices=1;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				case ___2:
					strcpy((*shape)->name,DXF->tag.cnt);
					break;
				case ___5:
					(*shape)->handle=strtol(DXF->tag.cnt,NULL,16);
					break;
				case ___8:
					strcpy((*shape)->layer,DXF->tag.cnt);
					break;
				case __10:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __20:
					(*shape)->vertices[0].y=strtod(DXF->tag.cnt,NULL);
					break;
				case __30:
					(*shape)->vertices[0].z=strtod(DXF->tag.cnt,NULL);
					break;
				case __41:
					(*shape)->user[0]=strtod(DXF->tag.cnt,NULL);
					break;
				case __42:
					(*shape)->user[1]=strtod(DXF->tag.cnt,NULL);
					break;
				case __43:
					(*shape)->user[2]=strtod(DXF->tag.cnt,NULL);
					break;
				case __50:
					(*shape)->rotation=strtod(DXF->tag.cnt,NULL);
					break;
				case __66:
					(*shape)->flag=strtol(DXF->tag.cnt,NULL,10);
					break;
				case __100:
					strcpy((*shape)->subclass,DXF->tag.cnt);
					break;
				case __210:
					(*shape)->extrusion.x=strtod(DXF->tag.cnt,NULL);
					break;
				case __220:
					(*shape)->extrusion.y=strtod(DXF->tag.cnt,NULL);
					break;
				case __230:
					(*shape)->extrusion.z=strtod(DXF->tag.cnt,NULL);
					break;
				default:
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadNotShape(DXF_Handle DXF, DXFObject *shape){
	(*shape)=NULL;
	do{
		DXF_ReadTag(DXF);
		if(!DXF->tag.err){
			switch(DXF->tag.cod){
				case ___0:
					break;
				default:
					break;
			}
		}
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	if(DXF->tag.err==-1)
		DXF->tag.err=DXF_NOERR;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadXLine(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadLine(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_XLINE;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadRay(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadLine(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_RAY;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadAttdef(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadText(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_ATTDEF;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadAttrib(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadText(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_ATTRIB;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadMText(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadText(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_MTEXT;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadRText(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadText(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_RTEXT;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_ReadSolid(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadTrace(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_SOLID;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_Read3DFace(DXF_Handle DXF, DXFObject *shape){	
	(void)DXF_ReadTrace(DXF,shape);
	if(!shape)
		return(-1);
	(*shape)->clss=_3DFACE;
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteSubClass(DXF_Handle DXF, char* subclass){
	DXF_WriteTag(DXF,__100,subclass);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteLayer(DXF_Handle DXF, char* layer){
	if(layer[0]==0)
		strcpy(layer,"1");
	DXF_WriteTag(DXF,___8,layer);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteName(DXF_Handle DXF, char* name){
	if(name[0]==0)
		strcpy(name,"Default");
	DXF_WriteTag(DXF,___1,name);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteColor(DXF_Handle DXF, int color){
char	s[256];
	if(color<=0){
		color=1;
	}
	sprintf(s,"%6d",color);
	DXF_WriteTag(DXF,__62,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteFlag(DXF_Handle DXF, int flag){
char	s[256];
	sprintf(s,"%6d",flag);
	DXF_WriteTag(DXF,__70,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteLong(DXF_Handle DXF, int l, int cod){
char	s[256];
	sprintf(s,"%6d",l);
	DXF_WriteTag(DXF,cod,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteHandle(DXF_Handle DXF, int handle){
char	s[256];
	sprintf(s,"%d",handle);
	DXF_WriteTag(DXF,___5,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteThickness(DXF_Handle DXF, double *thickness){
char	s[256];
double	x=*thickness;
	if(x==0){
		x=1;
	}
	DXF_Double2Str(&x,6,s);	
	DXF_WriteTag(DXF,__39,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteRotation(DXF_Handle DXF, double *rotation){
char	s[256];
	
	DXF_Double2Str(rotation,6,s);
	DXF_WriteTag(DXF,__50,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteSize(DXF_Handle DXF, double *size){
char	s[256];
double	x=*size;
	if(x==0){
		x=1;
	}
	DXF_Double2Str(&x,6,s);
	DXF_WriteTag(DXF,__40,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteScale(DXF_Handle DXF, double *scale){
char	s[256];	
double	x=*scale;
	if(x==0){
		x=1;
	}
	DXF_Double2Str(&x,6,s);
	DXF_WriteTag(DXF,__41,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteDouble(DXF_Handle DXF, double *d, int cod){
char	s[256];
	
	DXF_Double2Str(d,6,s);
	DXF_WriteTag(DXF,cod,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteVertex(DXF_Handle DXF, VERTEX_Rec *vertex, int count){
char	s[256];
	
	DXF_Double2Str(&vertex->x,6,s);
	DXF_WriteTag(DXF,__10+count,s);
	if(DXF->tag.err)
		return(DXF->tag.err);
	DXF_Double2Str(&vertex->y,6,s);
	DXF_WriteTag(DXF,__20+count,s);
	if(DXF->tag.err)
		return(DXF->tag.err);
	DXF_Double2Str(&vertex->z,6,s);
	DXF_WriteTag(DXF,__30+count,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
/*static int DXF_WriteVertices(DXF_Handle DXF, DXFObject shape)
{
	int	i,err;
	
	for (i=0;i<shape->nvertices;i++)
	{
		DXF_WriteTag(DXF,___0,VERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
			
		err=DXF_WriteLayer(DXF,shape->layer);
		if(err)
			return(err);

		err=DXF_WriteColor(DXF,shape->color);
		if(err)
			return(err);

		err=DXF_WriteVertex(DXF,&shape->vertices[i],FIRST);
	}
	DXF_WriteTag(DXF,___0,SEQEND);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteLayer(DXF,"\p0");
	if(err)
		return(err);
	
	return(DXF->tag.err);
}*/

/*static int DXF_WriteVertices(DXF_Handle DXF, DXFObject shape){
	int	i,err;
	
	for (i=0;i<shape->nvertices;i++){
		DXF_WriteTag(DXF,___0,VERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		
		//if(shape->handle){
			err=DXF_WriteHandle(DXF,shape->handle+i+1);
			if(err)
				return(err);
		//}
		
		err=DXF_WriteSubClass(DXF,ACDBENTITY);
		if(err)
			return(err);
			
		err=DXF_WriteLayer(DXF,shape->layer);
		if(err)
			return(err);
			
		DXF_WriteTag(DXF,__100,ACDBVERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		DXF_WriteTag(DXF,__100,ACDB2DVERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		
		err=DXF_WriteVertex(DXF,&shape->vertices[i],FIRST);
	}
	
	DXF_WriteTag(DXF,___0,SEQEND);
	if(DXF->tag.err)
		return(DXF->tag.err);
	
	//if(shape->handle){
		err=DXF_WriteHandle(DXF,shape->handle+i+1);
		if(err)
			return(err);
	//}
	
	err=DXF_WriteSubClass(DXF,ACDBENTITY);
		if(err)
			return(err);
	
	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);
		
	return(DXF->tag.err);
}*/

static int DXF_WriteVertices(DXF_Handle DXF, DXFObject shape){
int	i,err;
	
	for(i=0;i<shape->nvertices;i++){
		DXF_WriteTag(DXF,___0,VERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		DXF_WriteTag(DXF,__100,ACDBVERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		DXF_WriteTag(DXF,__100,ACDB2DVERTEX);
		if(DXF->tag.err)
			return(DXF->tag.err);
		err=DXF_WriteLayer(DXF,shape->layer);
		if(err)
			return(err);
		DXF_WriteTag(DXF,___6,CONTINUOUS);
		if(DXF->tag.err)
			return(DXF->tag.err);
		err=DXF_WriteColor(DXF,shape->color);
		if(err)
			return(err);
		if(shape->handle){
			err=DXF_WriteHandle(DXF,shape->handle+i+1);
			if(err)
				return(err);
		}
		err=DXF_WriteVertex(DXF,&shape->vertices[i],FIRST);
	}
	DXF_WriteTag(DXF,___0,SEQEND);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);
	DXF_WriteTag(DXF,___6,CONTINUOUS);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteColor(DXF,shape->color);
	if(err)
		return(err);
	if(shape->handle){
		err=DXF_WriteHandle(DXF,shape->handle+i+1);
		if(err)
			return(err);
	}
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WritePoint(DXF_Handle DXF, DXFObject shape){
int	err;
	
	if(!shape)
		return(-1);
	DXF_WriteTag(DXF,___0,POINT);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteSubClass(DXF,shape->subclass);
	if(err)
		return(err);
	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);
	DXF_WriteTag(DXF,___6,CONTINUOUS);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteColor(DXF,shape->color);
	if(err)
		return(err);
	err=DXF_WriteHandle(DXF,shape->handle);
	if(err)
		return(err);
	err=DXF_WriteVertex(DXF,&shape->vertices[0],FIRST);
	if(err)
		return(err);
	err=DXF_WriteThickness(DXF,&shape->size);
	if(err)
		return(err);
	err=DXF_WriteVertex(DXF,&shape->extrusion,EXTRUSION);
	if(err)
		return(err);
	err=DXF_WriteRotation(DXF,&shape->rotation);
	if(err)
		return(err);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int DXF_WriteText(DXF_Handle DXF, DXFObject shape){
int	err;
	
	if(!shape)
		return(-1);
	DXF_WriteTag(DXF,___0,TEXT);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteSubClass(DXF,shape->subclass);
	if(err)
		return(err);
	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);
	err=DXF_WriteColor(DXF,shape->color);
	if(err)
		return(err);
	err=DXF_WriteHandle(DXF,shape->handle);
	if(err)
		return(err);
	/*err=DXF_WriteThickness(DXF,&shape->size);
	if(err)
		return(err);*/
	// PB Thickness=Size
	err=DXF_WriteVertex(DXF,&shape->vertices[0],FIRST);
	if(err)
		return(err);
	err=DXF_WriteSize(DXF,&shape->size);
	if(err)
		return(err);
	err=DXF_WriteName(DXF,shape->name);
	if(err)
		return(err);
	err=DXF_WriteRotation(DXF,&shape->rotation);
	if(err)
		return(err);
	err=DXF_WriteScale(DXF,&shape->scale);
	if(err)
		return(err);
	DXF_WriteTag(DXF,__51,"0");
	if(DXF->tag.err)
		return(DXF->tag.err);
	DXF_WriteTag(DXF,___7,STANDARD);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteDouble(DXF,&shape->user[1],__71);
	if(err)
		return(err);
	err=DXF_WriteDouble(DXF,&shape->user[2],__72);
	if(err)
		return(err);
	if(shape->nvertices>1){
		err=DXF_WriteVertex(DXF,&shape->vertices[1],SECUND);
		if(err)
			return(err);
	}
	err=DXF_WriteVertex(DXF,&shape->extrusion,EXTRUSION);
	if(err)
		return(err);
	err=DXF_WriteDouble(DXF,&shape->user[3],__73);
	if(err)
		return(err);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
/*static int DXF_WritePolyline(DXF_Handle DXF, DXFObject shape)
{
	int		err;
	VERTEX_Rec	NullVertex;
	
	NullVertex.x=0;
	NullVertex.y=0;
	NullVertex.z=0;
	if(!shape)
		return(-1);
	DXF_WriteTag(DXF,___0,POLYLINE);
	if(DXF->tag.err)
		return(DXF->tag.err);

	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);

	err=DXF_WriteColor(DXF,shape->color);
	if(err)
		return(err);	
		
	err=DXF_WriteLong(DXF,1,__66);
	if(err)
		return(err);
	
	err=DXF_WriteFlag(DXF,shape->flag);
	if(err)
		return(err);
		
	err=DXF_WriteVertices(DXF,shape);
	if(err)
		return(err);
		
	return(DXF->tag.err);
}*/

static int DXF_WritePolyline(DXF_Handle DXF, DXFObject shape){
//_bTrace_("DXF_WritePolyline",false);
int		err;
VERTEX_Rec	NullVertex;
	
	NullVertex.x=0;
	NullVertex.y=0;
	NullVertex.z=0;
	if(!shape)
		return(-1);
	DXF_WriteTag(DXF,___0,POLYLINE);
	if(DXF->tag.err)
		return(DXF->tag.err);
	err=DXF_WriteSubClass(DXF,shape->subclass);
	if(err)
		return(err);
	err=DXF_WriteLayer(DXF,shape->layer);
	if(err)
		return(err);
	DXF_WriteTag(DXF,___6,CONTINUOUS);
	if(DXF->tag.err)
		return(DXF->tag.err);	
	err=DXF_WriteColor(DXF,shape->color);
	if(err)
		return(err);		
	err=DXF_WriteHandle(DXF,shape->handle);
	if(err)
		return(err);
	err=DXF_WriteLong(DXF,1,__66);
	if(err)
		return(err);
	err=DXF_WriteVertex(DXF,&NullVertex,FIRST);
	if(err)
		return(err);
	err=DXF_WriteSize(DXF,&shape->size);
	if(err)
		return(err);
	err=DXF_WriteFlag(DXF,shape->flag);
	if(err)
		return(err);
	err=DXF_WriteVertex(DXF,&shape->extrusion,EXTRUSION);
	if(err)
		return(err);
//_tm_("DXF_WriteVertices");
	err=DXF_WriteVertices(DXF,shape);
	if(err)
		return(err);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadEndBlock(DXF_Handle DXF){
	do{
		DXF_ReadTag(DXF);
	}
	while ((!DXF->tag.err)&&(DXF->tag.cod!=___0)&&(!DXF->tag.last));
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadBlock(DXF_Handle DXF){
char			s[256];
int				err,which=_UNKNOWN;
DXFObject		shape;
BLOCK_Handle	blk;
	
	blk=(BLOCK_Handle)malloc(sizeof(BLOCK_Rec));
	if(!blk)
		return(-1);
	blk->handle=0;
	blk->layer[0]=0;
	blk->bn1[0]=0;
	blk->bn2[0]=0;
	blk->path[0]=0;
	blk->base.x=0;
	blk->base.y=0;
	blk->base.z=0;
	blk->nshapes=0;
	blk->shapes=(DXFObject*)malloc(sizeof(DXFObject));
	do{
		err=DXF->tag.err;
		if(!err){
			which=-1;
			switch(DXF->tag.cod){
				case ___0:
					shape=NULL;
					which=DXF_IsBlockTag(DXF->tag.cnt);
					switch(which){
						case _ENDBLK:
							err=DXF_ReadEndBlock(DXF);
							break;
						case _3DFACE:
							err=DXF_Read3DFace(DXF,&shape);
							break;
						case _ARC:
							err=DXF_ReadArc(DXF,&shape);
							break;
						case _ARCALIGNEDTEXT:
							err=DXF_ReadArcAlignedText(DXF,&shape);
							break;
						case _ATTDEF:
							err=DXF_ReadAttdef(DXF,&shape);
							break;
						case _ATTRIB:
							err=DXF_ReadAttrib(DXF,&shape);
							break;
						case _CIRCLE:
							err=DXF_ReadCircle(DXF,&shape);
							break;
						case _DIMENSION:
							err=DXF_ReadDimension(DXF,&shape);
							break;
						case _ELLIPSE:
							err=DXF_ReadEllipse(DXF,&shape);
							break;
						case _INSERT:
							err=DXF_ReadInsert(DXF,&shape);
							break;
						case _LINE:
							err=DXF_ReadLine(DXF,&shape);
							break;
						case _LWPOLYLINE:
							err=DXF_ReadLWPolyline(DXF,&shape);
							break;
						case _MTEXT:
							err=DXF_ReadMText(DXF,&shape);
							break;
						case _POINT:
							err=DXF_ReadPoint(DXF,&shape);
							break;
						case _POLYLINE:
							err=DXF_ReadPolyline(DXF,&shape);
							break;
						case _RAY:
							err=DXF_ReadRay(DXF,&shape);
							break;
						case _RTEXT:
							err=DXF_ReadRText(DXF,&shape);
							break;
						case _SOLID:
							err=DXF_ReadSolid(DXF,&shape);
							break;
						case _TEXT:
							err=DXF_ReadText(DXF,&shape);
							break;
						case _TRACE:
							err=DXF_ReadTrace(DXF,&shape);
							break;
						case _XLINE:
							err=DXF_ReadXLine(DXF,&shape);
							break;
						case _3DSOLID:
						case _ACADPROXENTITY:
						case _BODY:
						case _HATCH:
						case _IMAGE:
						case _LEADER:
						case _MLINE:
						case _SHAPE:
						case _SPLINE:
						case _TOLERANCE:
						case _VERTEX:
						case _VIEWPORT:
						case _WIPEOUT:
							err=DXF_ReadNotShape(DXF,&shape);
							break;
						default :
							DXF_ReadTag(DXF);
							err=DXF->tag.err;
							break;
					}
					if(shape){
						blk->nshapes++;
						blk->shapes=(DXFObject*)realloc(blk->shapes,(blk->nshapes*sizeof(DXFObject)));
						if(blk->shapes)
							blk->shapes[blk->nshapes-1]=shape;
						else
							err=-1;
					}
					break;
				case ___1:
					strcpy(blk->path,DXF->tag.cnt);
					DXF_ReadTag(DXF);
					break;
				case ___2:
					strcpy(blk->bn1,DXF->tag.cnt);
					DXF_ReadTag(DXF);
					break;
				case ___3:
					strcpy(blk->bn2,DXF->tag.cnt);
					DXF_ReadTag(DXF);
					break;
				case ___5:
					blk->handle=strtol(s,NULL,16);
					DXF_ReadTag(DXF);
					break;
				case ___8:
					strcpy(blk->layer,DXF->tag.cnt);
					DXF_ReadTag(DXF);
					break;
				case __10:
					blk->base.x=strtod(DXF->tag.cnt,NULL);
					DXF_ReadTag(DXF);
					break;
				case __20:
					blk->base.y=strtod(DXF->tag.cnt,NULL);
					DXF_ReadTag(DXF);
					break;
				case __30:
					blk->base.z=strtod(DXF->tag.cnt,NULL);
					DXF_ReadTag(DXF);
					break;
				default :
					err=DXF->tag.err;
					DXF_ReadTag(DXF);
					break;
			}
		}
		
	}
	while ((!err)&&(!DXF->tag.last)&&(which!=_ENDBLK));
	if(!err)
		DXF->blocks.blks[DXF->blocks.nblks-1]=blk;
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadHeader(DXF_Handle DXF, int pos){
char	s[256];
int	err,which=_UNKNOWN;
	
	DXF->header.sec.begin=pos;
	do{
		DXF_ReadTag(DXF);
		err=DXF->tag.err;
		if(!err){
			switch(DXF->tag.cod){
				case ___0:
					which=DXF_IsEndSec(DXF->tag.cnt);
					break;
				case ___9:
					which=DXF_IsHeaderTag(DXF->tag.cnt);
					switch(which){
						case _ACADVER:
							err=DXF_ReadVers(DXF);
							break;
						case _ANGDIR:
							DXF_ReadTag(DXF);
									DXF->header.angdir=(strtol(s,NULL,10))?0:1;
							break;
						case _EXTMIN:
							err=DXF_ReadBounds(DXF,&DXF->header.extmin);
							break;
						case _EXTMAX:
							err=DXF_ReadBounds(DXF,&DXF->header.extmax);
							break;
					}
					break;
				default :
					break;
			}
		}
	}
	while ((!err)&&(!DXF->tag.last)&&(which!=_ENDSEC));
	DXF->header.sec.end=DXF->curs;
	DXF->header.sec.err=err;
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
/*int	DXF_WriteHeader(DXF_Handle DXF){
	int	err,which=_UNKNOWN;
	
	DXF_WriteTag(DXF,___9,EXTMIN);
	err=DXF_WriteVertex(DXF,&DXF->header.extmin,0);
	if(err){
		return(err);
	}
	DXF_WriteTag(DXF,___9,EXTMAX);
	err=DXF_WriteVertex(DXF,&DXF->header.extmax,0);
	if(err){
		return(err);
	}
	return(DXF_NOERR);
}*/

int	DXF_WriteHeader(DXF_Handle DXF){
int	err;
	DXF_WriteTag(DXF,___9,EXTMIN);
	err=DXF_WriteVertex(DXF,&DXF->header.extmin,0);
	if(err){
		return(err);
	}
	DXF_WriteTag(DXF,___9,EXTMAX);
	err=DXF_WriteVertex(DXF,&DXF->header.extmax,0);
	if(err){
		return(err);
	}
	DXF_WriteTag(DXF,___9,ANGDIR);
	err=DXF_WriteDouble(DXF,&DXF->header.angdir,__70);
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadBlocks(DXF_Handle DXF, int pos){
int	err,which=_UNKNOWN;
	
	DXF->blocks.sec.begin=pos;
	DXF->blocks.nblks=0;
	DXF->blocks.blks=(BLOCK_Handle*)malloc(sizeof(BLOCK_Handle));
	DXF->blocks.sec.begin=pos;
	do{
		err=DXF->tag.err;
		if(!err){
			switch(DXF->tag.cod){
				case ___0:
					which=DXF_IsBlockTag(DXF->tag.cnt);
					if(which==_BLOCK){
						DXF->blocks.nblks++;
						DXF->blocks.blks=(BLOCK_Handle*)realloc(DXF->blocks.blks,(DXF->blocks.nblks*sizeof(BLOCK_Handle)));
						if(DXF->blocks.blks)
							err=DXF_ReadBlock(DXF);
						else
							err=-1;
					}
					else
						which=DXF_IsEndSec(DXF->tag.cnt);
					break;
				default :
					DXF_ReadTag(DXF);
					break;
			}
		}
	}
	while ((!err)&&(!DXF->tag.last)&&(which!=_ENDSEC));
	DXF->blocks.sec.end=DXF->curs;
	DXF->blocks.sec.err=err;
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadEntities(DXF_Handle DXF, int pos){
	DXF->entities.begin=pos;
	DXF->entities.end=DXF->tbl->CountRecords();
	DXF->entities.err=DXF_NOERR;
	return(DXF_NOERR);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_ReadDefault(DXF_Handle DXF){
int	err,which=_UNKNOWN;
	
	do{
		DXF_ReadTag(DXF);
		err=DXF->tag.err;
		if(!err){
			switch(DXF->tag.cod){
				case ___0:
					which=DXF_IsEndSec(DXF->tag.cnt);
					break;
				default :
					break;
			}
		}
	}
	while ((!err)&&(!DXF->tag.last)&&(which!=_ENDSEC));
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static int	DXF_FindSections(DXF_Handle DXF){
int	err;
int	off,which;
	
	do{
		off=DXF->curs;
//fprintf(stderr,"DXF_FindSections::curs=%d\n",off);
		DXF_ReadTag(DXF);
		
		if(DXF->tag.cod==__999){
			continue;
		}
		
		if((!DXF->tag.err)&&(DXF->tag.cod==___0)){
//fprintf(stderr,"DXF_FindSections::curs=%d, (%s)\n",off,DXF->tag.cnt);

			which=DXF_IsBegSec(DXF->tag.cnt);
			if(which){
//fprintf(stderr,"DXF_FindSections::ok for section begining\n");
				DXF_ReadTag(DXF);
				if(!DXF->tag.err){
					which=DXF_IsSection(DXF->tag.cnt);
					switch(which){
						case _HEADER:
							err=DXF_ReadHeader(DXF,off);
//fprintf(stderr,"DXF_FindSections::header error=%d\n",err);
							break;
						case _BLOCKS:
							err=DXF_ReadBlocks(DXF,off);
							break;
						case _ENTITIES:
							err=DXF_ReadEntities(DXF,off);
//fprintf(stderr,"DXF_FindSections::entities find at %d->%d error=%d\n",DXF->entities.begin,DXF->entities.end,err);
							err=-1;
							break;
						default:
							err=DXF_ReadDefault(DXF);
							break;
					}
				}
			}
		}
	}
	while ((!err)&&(!DXF->tag.err)&&(!DXF->tag.last));
/*	if(err){
fprintf(stderr,"DXF_FindSections::break with err=%d\n",err);
	}
	if(DXF->tag.err){
fprintf(stderr,"DXF_FindSections::break with DXF->tag.err=%d\n",DXF->tag.err);
	}
	if(DXF->tag.last){
fprintf(stderr,"DXF_FindSections::break with DXF->tag.last=%d\n",DXF->tag.last);
	}*/
	return(DXF_NOERR);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_SetSection(DXF_Handle	DXF, int which){
int	err=DXF_NOERR;
	
	switch(which){
		case _HEADER:
			DXF->curs=DXF->header.sec.begin;
			break;
		case _CLASSES:
			DXF->curs=DXF->classes.begin;
			break;
		case _TABLES:
			DXF->curs=DXF->tables.begin;
			break;
		case _BLOCKS:
			DXF->curs=DXF->blocks.sec.begin;
			break;
		case _ENTITIES:
			DXF->curs=DXF->entities.begin;
			break;
		case _OBJECTS:
			DXF->curs=DXF->objects.begin;
			break;
		case _THUMBNAILIMAGE:
			DXF->curs=DXF->thumbnail.begin;
			break;
		default:
			err=-1;
			break;
	}
	if(!err){
//fprintf(stderr,"DXF_SetSection::curs set to %d\n",DXF->curs);
		DXF_ReadTag(DXF);// Pour passer le tag 0
	}
	else{
fprintf(stderr,"DXF_SetSection::bad section tag (%d)\n",which);
	}
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_OpenSection(DXF_Handle	DXF, int which){
int		err;
char	s[256];
	
	DXF_WriteTag(DXF,___0,SECTION);
	switch(which){
		case _HEADER:
			strcpy(s,HEADER);
			break;
		case _CLASSES:
			strcpy(s,CLASSES);
			break;
		case _TABLES:
			strcpy(s,TABLES);
			break;
		case _BLOCKS:
			strcpy(s,BLOCKS);
			break;
		case _ENTITIES:
			strcpy(s,ENTITIES);
			break;
		case _OBJECTS:
			strcpy(s,OBJECTS);
			break;
		case _THUMBNAILIMAGE:
			strcpy(s,THUMBNAILIMAGE);
			break;
		default:
			err=-1;
			break;
	}
	err=DXF->tag.err;
	if(!err)
		DXF_WriteTag(DXF,___2,s);
	err=DXF->tag.err;
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_CloseSection(DXF_Handle	DXF){
	DXF_WriteTag(DXF,___0,ENDSEC);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_CloseDoc(DXF_Handle	DXF){
	DXF_WriteTag(DXF,___0,_EOF);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_WriteComment(DXF_Handle	DXF, char* s){
	DXF_WriteTag(DXF,__999,s);
	return(DXF->tag.err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXF_Handle DXF_Create(char* name, char* path, int world){
int			status;
DXF_Handle	DXF;
wtable		tbl;
	
	tbl=wtbl_alloc(kTableDXF,path,name,true,1,-1,&status);
	if(!tbl){
		return(NULL);
	}
	if(status<0){
		wtbl_free(tbl);
		return(NULL);
	}
	DXF=(DXF_Handle)malloc(sizeof(DXF_Rec));
	if(!DXF){
		wtbl_free(tbl);
		return(NULL);
	}
	DXF->tbl=tbl;
	DXF->curs=0;
	DXF->header.acadver=_UNKNOWN;
	DXF->header.angdir=1;
	DXF->header.sec.begin=-1;
	DXF->header.sec.end=-1;
	DXF->header.sec.err=-1;
	DXF->classes.begin=-1;
	DXF->classes.end=-1;
	DXF->classes.err=-1;
	DXF->tables.begin=-1;
	DXF->tables.end=-1;
	DXF->tables.err=-1;
	DXF->blocks.sec.begin=-1;
	DXF->blocks.sec.end=-1;
	DXF->blocks.sec.err=-1;
	DXF->blocks.nblks=0;
	DXF->blocks.blks=NULL;
	DXF->entities.begin=-1;
	DXF->entities.end=-1;
	DXF->entities.err=-1;
	DXF->objects.begin=-1;
	DXF->objects.end=-1;
	DXF->objects.err=-1;
	DXF->thumbnail.begin=-1;
	DXF->thumbnail.end=-1;
	DXF->thumbnail.err=-1;
	DXF->tag.last=0;
	DXF->tag.err=DXF_NOERR;
	DXF->tag.cod=-1;
	DXF->tag.cnt[0]=0;
/*	status=DXF_FindSections(DXF);
	if(status<0){
		free(DXF);
		DXF=NULL;
		wtbl_free(tbl);
	}*/
	return(DXF);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXF_Handle DXF_Open(char* name,	char* path){
int			status=0;
DXF_Handle	DXF;
wtable		tbl;
	
	tbl=wtbl_alloc(kTableDXF,path,name,false,1,-1,&status);
	if(!tbl){
fprintf(stderr,"DXF_Open::wtbl_alloc error\n");
		return(NULL);
	}
	if(status<0){
fprintf(stderr,"DXF_Open::wtbl_alloc error %d\n",status);
		wtbl_free(tbl);
		return(NULL);
	}
	DXF=(DXF_Handle)malloc(sizeof(DXF_Rec));
	if(!DXF){
fprintf(stderr,"DXF_Open::malloc error\n");
		wtbl_free(tbl);
		return(NULL);
	}
	DXF->tbl=tbl;
	DXF->curs=1;
	DXF->header.acadver=_UNKNOWN;
	DXF->header.angdir=1;
	DXF->header.sec.begin=-1;
	DXF->header.sec.end=-1;
	DXF->header.sec.err=-1;
	DXF->classes.begin=-1;
	DXF->classes.end=-1;
	DXF->classes.err=-1;
	DXF->tables.begin=-1;
	DXF->tables.end=-1;
	DXF->tables.err=-1;
	DXF->blocks.sec.begin=-1;
	DXF->blocks.sec.end=-1;
	DXF->blocks.sec.err=-1;
	DXF->blocks.nblks=0;
	DXF->blocks.blks=NULL;
	DXF->entities.begin=-1;
	DXF->entities.end=-1;
	DXF->entities.err=-1;
	DXF->objects.begin=-1;
	DXF->objects.end=-1;
	DXF->objects.err=-1;
	DXF->thumbnail.begin=-1;
	DXF->thumbnail.end=-1;
	DXF->thumbnail.err=-1;
	DXF->tag.last=0;
	DXF->tag.err=DXF_NOERR;
	DXF->tag.cod=-1;
	DXF->tag.cnt[0]=0;
	status=DXF_FindSections(DXF);
	if(status<0){
fprintf(stderr,"DXF_Open::DXF_FindSections error %d\n",status);
		free(DXF);
		DXF=NULL;
		wtbl_free(tbl);
	}
	return(DXF);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXF_Handle DXF_fCreate(char* name, char* path, int world, double* reso, double* ox, double* oy, int* tsrid, int* asrid){
int			status;
DXF_Handle	DXF;
wtable		tbl;
	
	tbl=wtbl_falloc(kTableDXF,path,name,true,reso,ox,oy,tsrid,asrid,&status);
	if(!tbl){
		return(NULL);
	}
	if(status<0){
		wtbl_free(tbl);
		return(NULL);
	}
	DXF=(DXF_Handle)malloc(sizeof(DXF_Rec));
	if(!DXF){
		wtbl_free(tbl);
		return(NULL);
	}
	DXF->tbl=tbl;
	DXF->curs=0;
	DXF->header.acadver=_UNKNOWN;
	DXF->header.angdir=1;
	DXF->header.sec.begin=-1;
	DXF->header.sec.end=-1;
	DXF->header.sec.err=-1;
	DXF->classes.begin=-1;
	DXF->classes.end=-1;
	DXF->classes.err=-1;
	DXF->tables.begin=-1;
	DXF->tables.end=-1;
	DXF->tables.err=-1;
	DXF->blocks.sec.begin=-1;
	DXF->blocks.sec.end=-1;
	DXF->blocks.sec.err=-1;
	DXF->blocks.nblks=0;
	DXF->blocks.blks=NULL;
	DXF->entities.begin=-1;
	DXF->entities.end=-1;
	DXF->entities.err=-1;
	DXF->objects.begin=-1;
	DXF->objects.end=-1;
	DXF->objects.err=-1;
	DXF->thumbnail.begin=-1;
	DXF->thumbnail.end=-1;
	DXF->thumbnail.err=-1;
	DXF->tag.last=0;
	DXF->tag.err=DXF_NOERR;
	DXF->tag.cod=-1;
	DXF->tag.cnt[0]=0;
/*	status=DXF_FindSections(DXF);
	if(status<0){
		free(DXF);
		DXF=NULL;
		wtbl_free(tbl);
	}*/
	return(DXF);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXF_Handle DXF_fOpen(char* name, char* path, double* reso, double* ox, double* oy, int* tsrid, int* asrid){
int			status=0;
DXF_Handle	DXF;
wtable		tbl;
	
	tbl=wtbl_falloc(kTableDXF,path,name,false,reso,ox,oy,tsrid,asrid,&status);
	if(!tbl){
fprintf(stderr,"DXF_fOpen::wtbl_falloc error\n");
		return(NULL);
	}
	if(status<0){
fprintf(stderr,"DXF_fOpen::wtbl_alloc error %d\n",status);
		wtbl_free(tbl);
		return(NULL);
	}
	DXF=(DXF_Handle)malloc(sizeof(DXF_Rec));
	if(!DXF){
fprintf(stderr,"DXF_fOpen::malloc error\n");
		wtbl_free(tbl);
		return(NULL);
	}
	DXF->tbl=tbl;
	DXF->curs=1;
	DXF->header.acadver=_UNKNOWN;
	DXF->header.angdir=1;
	DXF->header.sec.begin=-1;
	DXF->header.sec.end=-1;
	DXF->header.sec.err=-1;
	DXF->classes.begin=-1;
	DXF->classes.end=-1;
	DXF->classes.err=-1;
	DXF->tables.begin=-1;
	DXF->tables.end=-1;
	DXF->tables.err=-1;
	DXF->blocks.sec.begin=-1;
	DXF->blocks.sec.end=-1;
	DXF->blocks.sec.err=-1;
	DXF->blocks.nblks=0;
	DXF->blocks.blks=NULL;
	DXF->entities.begin=-1;
	DXF->entities.end=-1;
	DXF->entities.err=-1;
	DXF->objects.begin=-1;
	DXF->objects.end=-1;
	DXF->objects.err=-1;
	DXF->thumbnail.begin=-1;
	DXF->thumbnail.end=-1;
	DXF->thumbnail.err=-1;
	DXF->tag.last=0;
	DXF->tag.err=DXF_NOERR;
	DXF->tag.cod=-1;
	DXF->tag.cnt[0]=0;
	status=DXF_FindSections(DXF);
	if(status<0){
fprintf(stderr,"DXF_fOpen::DXF_FindSections error %d\n",status);
		free(DXF);
		DXF=NULL;
		wtbl_free(tbl);
	}
	return(DXF);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_Close(DXF_Handle	DXF){
int	err,i,j;
	
	if(!DXF){
		return(-1);
	}
	wtbl_free(DXF->tbl);
	for (i=1;i<DXF->blocks.nblks;i++){
		for (j=0;j<DXF->blocks.blks[i]->nshapes;j++)
			DXF_DestroyObject(DXF->blocks.blks[i]->shapes[j]);
		free(DXF->blocks.blks[i]);
	}
	free(DXF);
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXFObject DXF_ReadNext(DXF_Handle DXF){
int			err=DXF_NOERR,which=_UNKNOWN;
DXFObject	shape=NULL;
		
	do{
		if(!DXF->tag.err){
			which=DXF_IsEntityTag(DXF->tag.cnt);
			switch(which){
				case _3DFACE:
					err=DXF_Read3DFace(DXF,&shape);
					break;
				case _ARC:
					err=DXF_ReadArc(DXF,&shape);
					break;
				case _ARCALIGNEDTEXT:
					err=DXF_ReadArcAlignedText(DXF,&shape);
					break;
				case _ATTDEF:
					err=DXF_ReadAttdef(DXF,&shape);
					break;
				case _ATTRIB:
					err=DXF_ReadAttrib(DXF,&shape);
					break;
				case _CIRCLE:
					err=DXF_ReadCircle(DXF,&shape);
					break;
				case _DIMENSION:
					err=DXF_ReadDimension(DXF,&shape);
					break;
				case _ELLIPSE:
					err=DXF_ReadEllipse(DXF,&shape);
					break;
				case _INSERT:
					err=DXF_ReadInsert(DXF,&shape);
					break;
				case _LINE:
					err=DXF_ReadLine(DXF,&shape);
					break;
				case _LWPOLYLINE:
					err=DXF_ReadLWPolyline(DXF,&shape);
					break;
				case _MTEXT:
					err=DXF_ReadMText(DXF,&shape);
					break;
				case _POINT:
					err=DXF_ReadPoint(DXF,&shape);
					break;
				case _POLYLINE:
					err=DXF_ReadPolyline(DXF,&shape);
					break;
				case _RAY:
					err=DXF_ReadRay(DXF,&shape);
					break;
				case _RTEXT:
					err=DXF_ReadRText(DXF,&shape);
					break;
				case _SOLID:
					err=DXF_ReadSolid(DXF,&shape);
					break;
				case _TEXT:
					err=DXF_ReadText(DXF,&shape);
					break;
				case _TRACE:
					err=DXF_ReadTrace(DXF,&shape);
					break;
				case _XLINE:
					err=DXF_ReadXLine(DXF,&shape);
					break;
				case _3DSOLID:
				case _ACADPROXENTITY:
				case _BODY:
				case _HATCH:
				case _IMAGE:
				case _LEADER:
				case _MLINE:
				case _SHAPE:
				case _SPLINE:
				case _TOLERANCE:
				case _VERTEX:
				case _VIEWPORT:
				case _WIPEOUT:
					err=DXF_ReadNotShape(DXF,&shape);
					break;
				case _UNKNOWN:
					if(DXF_IsEndSec(DXF->tag.cnt)){
						err=-1;
					}
					else{
						DXF_ReadTag(DXF);
						err=DXF_NOERR;
					}
					break;
				default :
					DXF_ReadTag(DXF);
					err=DXF_NOERR;
					break;
			}
		}
	}
	while ((!err)&&(!DXF->tag.err)&&(!DXF->tag.last)/*&&(DXF->TXF->fpos<DXF->entities.end)*/&&(!shape));
	return(shape);
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_WriteNext(DXF_Handle DXF, DXFObject	shape){
int	err=DXF_NOERR;
	
	switch(shape->clss){
		case _POINT:
			err=DXF_WritePoint(DXF,shape);
			break;
		case _POLYLINE:
			err=DXF_WritePolyline(DXF,shape);
			break;
		case _TEXT:
			err=DXF_WriteText(DXF,shape);
			break;
		default :
			break;
	}
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void DXF_OffsetShape(DXFObject	shape, double	*dx, double	*dy, double	*dz){
int	i;
	
	for(i=0;i<shape->nvertices;i++){
		shape->vertices[i].x+=(*dx);
		shape->vertices[i].y+=(*dy);
		shape->vertices[i].z+=(*dz);
	}
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void DXF_ScaleShape(	DXFObject	shape,	
							VERTEX_Rec	*center,	
							double	*rx,	
							double	*ry,	
							double	*rz){
int	i;

	for(i=0;i<shape->nvertices;i++){
		shape->vertices[i].x=((shape->vertices[i].x-center->x)*(*rx))+center->x;
		shape->vertices[i].y=((shape->vertices[i].y-center->y)*(*ry))+center->y;
		shape->vertices[i].z=((shape->vertices[i].z-center->z)*(*rz))+center->z;
	}
	switch(shape->clss){
		case _ARC:
			if((*rx)<0){
				shape->user[1]=360.0-shape->user[1];
				shape->user[2]=360.0-shape->user[2];
				DXF_swapd(&shape->user[1],&shape->user[2]);
			}
			
			if((*ry)<0){
				shape->user[1]=360.0-shape->user[1];
				shape->user[2]=360.0-shape->user[2];
				DXF_swapd(&shape->user[1],&shape->user[2]);
			}
			
		case _CIRCLE:
			shape->user[0]*=(*rx);// Mise à l'échelle du rayon
			break;

		case _ELLIPSE:
			// A voir
			break;		
			
		case _POINT:	
		case _LINE:
		case _LWPOLYLINE:
		case _POLYLINE:
		case _RAY:
		case _XLINE:
			// Rien de plus
			break;
			
		case _ARCALIGNEDTEXT:
		case _ATTDEF:
		case _ATTRIB:
		case _3DFACE:
		case _DIMENSION:
		case _MTEXT:
		case _TEXT:
		case _RTEXT:
		case _SOLID:
		case _TRACE:
			// A voir
			break;
			
		case _INSERT:
			// A√Øe !!!
			break;
			
		default :
			break;
	}
}

// ---------------------------------------------------------------------------
// OK
// ------------
static void DXF_RotateShape(DXFObject	shape,	VERTEX_Rec	*center,	double	*a){
int			i;
double		angle,sinA,cosA,cx,cy;
VERTEX_Rec	newPos;
	
// 1 : Tourner les coordonnées des objets par rapport au centre
	angle=(*a);
	angle=angle/(180.0/M_PI);
	sinA=sin(angle);
	cosA=cos(angle);
	cx=center->x*(1-cosA)+center->y*sinA;
	cy=-center->x*sinA+center->y*(1-cosA);
	for(i=0;i<shape->nvertices;i++){
		newPos.x=shape->vertices[i].x*cosA-shape->vertices[i].y*sinA+cx;
		newPos.y=shape->vertices[i].x*sinA+shape->vertices[i].y*cosA+cy;
		newPos.z=shape->vertices[i].z;
		shape->vertices[i]=newPos;		
	}
// 2 : Mettre à jour les angles
	switch(shape->clss){
		case _ARC:
			shape->user[1]+=(*a);
			shape->user[2]+=(*a);
			break;
			
		default :
			break;
	}
}

// ---------------------------------------------------------------------------
// OK
// ------------
int DXF_Insert2Shapes(	DXF_Handle DXF, 
						DXFObject insert, 
						DXFObject **shapes, 
						int *count){	
int		err=0;
int		i,which,sz;
double	dx,dy,dz;
	
	(*count)=0;
	(*shapes)=(DXFObject*)malloc(sizeof(DXFObject));
	if(!(*shapes))
		return(-1);
	for(which=0;which<DXF->blocks.nblks;which++){
		if(	(!strcmp(insert->name,DXF->blocks.blks[which]->bn1))	||
			(!strcmp(insert->name,DXF->blocks.blks[which]->bn2))	){
			break;
		}
	}
	if(which==DXF->blocks.nblks){
		return(-1);
	}
	
	dx=insert->vertices[0].x-DXF->blocks.blks[which]->base.x;
	dy=insert->vertices[0].y-DXF->blocks.blks[which]->base.y;
	dz=insert->vertices[0].z-DXF->blocks.blks[which]->base.z;
	
	for(i=0;i<DXF->blocks.blks[which]->nshapes;i++){
		(*shapes)=(DXFObject*)realloc((*shapes),(sizeof(DXFObject)*(i+1)));
		if(!(*shapes))
			return(-1);
		sz=sizeof(DXFObject_Rec)+((DXF->blocks.blks[which]->shapes[i]->nvertices-1)*sizeof(VERTEX_Rec));
		(*shapes)[i]=(DXFObject)malloc(sz);
		if(!(*shapes)[i])
			return(-1);
		memmove((*shapes)[i],DXF->blocks.blks[which]->shapes[i],sz);
		DXF_OffsetShape((*shapes)[i],&dx,&dy,&dz);
		DXF_ScaleShape((*shapes)[i],&insert->vertices[0],&insert->user[0],&insert->user[1],&insert->user[2]);
		DXF_RotateShape((*shapes)[i],&insert->vertices[0],&insert->rotation);		

		(*count)++;
	}
	/*
	(*shapes)=realloc((*shapes),(sizeof(DXFObject)*(i+1)));
	(*shapes)[i]=insert;
	(*count)++;
	*/
	return(err);
}

// ---------------------------------------------------------------------------
// OK
// ------------
DXFObject DXF_NewObject(int DXFType, int nvertices, VERTEX_Rec *vertices){
int			vsz;
DXFObject	shape=NULL;
	
	vsz=nvertices*sizeof(VERTEX_Rec);
	shape=DXF_EmptyObject(nvertices);
	if(!shape)
		return(NULL);
	shape->clss=DXFType;
	shape->size=1;
	shape->scale=1;
	memmove(&shape->vertices,vertices,vsz);
	switch(DXFType){
		case _POINT:
			strcpy(shape->subclass,ACDBPOINT);
			break;
		case _POLYLINE:
			strcpy(shape->subclass,ACDB2DPOLYLINE);
			break;
		case _TEXT:
			strcpy(shape->subclass,ACDBTEXT);
			break;
	}
	return(shape);
}

// ---------------------------------------------------------------------------
// OK
// ------------
void DXF_DestroyObject(DXFObject shape){
	if(shape)
		free(shape);
}

// ---------------------------------------------------------------------------
// 
// ------------
VERTEX_Rec*	DXF_VXS2VERTEX(	DXF_Handle DXF,
							int closed,
							ivertices* vxs){
int			i,n;
dvertices*	dvxs=NULL;
VERTEX_Rec*	vertices;

	dvxs=((bTextDXFTable*)(DXF->tbl))->get_ageom(vxs);
	if(dvxs==NULL){
		return(NULL);
	}
	vertices=(VERTEX_Rec*)malloc(sizeof(VERTEX_Rec)*vxs->nv);
	if(vertices==NULL){
		dvs_free(dvxs);
		return(NULL);
	}
	n=(closed)?vxs->nv-1:vxs->nv;
	for(i=0;i<n;i++){
		vertices[i].x=dvxs->vx.vx2[i].x;
		vertices[i].y=dvxs->vx.vx2[i].y;
		vertices[i].z=0;
	}
	dvs_free(dvxs);
	return(vertices);
}

// ---------------------------------------------------------------------------
// 
// ------------
ivertices* DXF_VERTEX2VXS(DXF_Handle DXF, DXFObject shape){	
long		i,n;
double		a,b,d,d1,d2;
ivertices*	vxs=NULL;
dvertices*	dvxs=NULL;
d2dvertex	dvx;

	switch (shape->clss){
		case _3DFACE:
		case _LINE:
		case _LWPOLYLINE:
		case _POLYLINE:
		case _RAY:
		case _SOLID:
		case _TRACE:
		case _XLINE:
		
		case _POINT:
		case _ARCALIGNEDTEXT:
		case _ATTDEF:
		case _ATTRIB:
		case _DIMENSION:
		case _MTEXT:
		case _RTEXT:
		case _TEXT:
			dvxs=dvs_new(_2D_VX,shape->nvertices,0);
			if(dvxs){
				for(i=0;i<dvxs->nv;i++){
					dvxs->vx.vx2[i].x=shape->vertices[i].x;
					dvxs->vx.vx2[i].y=shape->vertices[i].y;
				}
			}
			break;
		case _ARC:
			a=shape->user[1]+shape->rotation;
			b=shape->user[2]+shape->rotation;
			DXF_swapd(&a,&b);
			a=360.0-a+90.0;
			b=360.0-b+90.0;
			if(b<a){
				b+=360.0;
			}
			d=deg2rad((b-a)/36.0);
			a=deg2rad(a);
			b=deg2rad(b);
			dvxs=dvs_new(_2D_VX,shape->nvertices,0);
			if(dvxs){
				for(i=0;i<36;i++){
					dvx.x=shape->vertices[0].x+(shape->user[0]*sin(a));
					dvx.y=shape->vertices[0].y+(shape->user[0]*cos(a));
					dvxs=dvx2_add(dvxs,&dvx);
					if(!dvxs){
						return(NULL);
					}
					a+=d;
				}
				dvx.x=shape->vertices[0].x+(shape->user[0]*sin(a));
				dvx.y=shape->vertices[0].y+(shape->user[0]*cos(a));
				dvxs=dvx2_add(dvxs,&dvx);	
			}
			break;
		case _CIRCLE:
			n=2.0*M_PI*shape->user[0];
			if (n>360)
				n=360;
			else if (n<12)
				n=12;
			dvxs=dvs_new(_2D_VX,n+2,0);
			if(vxs){
				a=deg2rad(360.0/(double)(n+1));
				for(i=0;i<vxs->nv;i++){
					dvxs->vx.vx2[i].x=shape->vertices[0].x+(shape->user[0]*sin(((double)i)*a));
					dvxs->vx.vx2[i].y=shape->vertices[0].y+(shape->user[0]*cos(((double)i)*a));
				}
			}
			break;
		case _ELLIPSE:
			a=shape->user[1]+shape->rotation;
			b=shape->user[2]+shape->rotation;
			d1=sqrt((shape->vertices[1].x-shape->vertices[0].x)*(shape->vertices[1].y-shape->vertices[0].y));
			d2=d1*shape->user[0];
			if(b<a){
				DXF_swapd(&a,&b);
			}
			d=deg2rad((b-a)/36.0);
			a=deg2rad(a);
			b=deg2rad(b);
			dvxs=dvs_new(_2D_VX,0,0);
			if(vxs){
				do{
					dvx.x=shape->vertices[0].x+(d1*sin(a));
					dvx.y=shape->vertices[0].y+(d2*cos(a));
					dvxs=dvx2_add(dvxs,&dvx);
					if(!vxs){
						return(NULL);
					}
					a+=d;
				}
				while((!i)&&(a<b));
				dvx.x=shape->vertices[0].x+(d1*sin(b));
				dvx.y=shape->vertices[0].y+(d2*cos(b));
				dvxs=dvx2_add(dvxs,&dvx);
			}
			break;
	}
	if(dvxs){
		vxs=((bTextDXFTable*)(DXF->tbl))->get_tgeom(dvxs);
		dvs_free(dvxs);
	}
	return(vxs);
}

