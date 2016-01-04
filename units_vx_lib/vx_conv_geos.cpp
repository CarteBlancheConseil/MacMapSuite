//----------------------------------------------------------------------------
// File : vx_conv_geos.cpp
// Project : MacMapSuite
// Purpose : CPP source file : vx <-> GEOS conversion
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

#include "vx_conv_geos.h"
#include "vx_measure.h"
#include "vx_utils.h"
#include "vx_allocation.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include "bTrace.h"

// ---------------------------------------------------------------------------
// 
// -----------
static int _g_inited=0;

// ---------------------------------------------------------------------------
// 
// -----------
static void g_func(const char *fmt, ...){
	fprintf(stderr,"<%s>\n",fmt);
}

// ---------------------------------------------------------------------------
// 
// -----------
static ivertices* geosrawgeom2vxs(GEOSGeom geom){
//bTrace	trc("geosrawgeom2vxs",true);
GEOSCoordSeq	gsqr=(GEOSCoordSequence*)GEOSGeom_getCoordSeq(geom);
	if(!gsqr){
//trc.msg("xgon has no GEOSCoordSeq");
		return(NULL);
	}
int			i,nc=GEOSGetNumCoordinates(geom);
//trc.msg("GEOSGetNumCoordinates returns %d",nc);
ivertices*	vxs=ivs_new(_2D_VX,nc,0);
double		bf;
	for(i=0;i<nc;i++){
		GEOSCoordSeq_getX(gsqr,i,&bf);
		vxs->vx.vx2[i].h=round(bf);
		GEOSCoordSeq_getY(gsqr,i,&bf);
		vxs->vx.vx2[i].v=-round(bf);
	}
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
GEOSGeom vxs2geosxgon(ivertices* vxs){
//
//bTrace	trc("vxs2geosxgon",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
	
int				i,j;
int				np,n=ivs_n_parts(vxs);
GEOSCoordSeq	gsq;
GEOSGeom		ggmlr,ggmlrf;
GEOSGeom		ggmpgr=NULL;
GEOSGeom*		ggmp=NULL;
i2dvertex*		vx=ivs2_part(vxs,0,&np);

	if(n==1){
		gsq=GEOSCoordSeq_create(np,2);
		for(i=0;i<np;i++){
			(void)GEOSCoordSeq_setX(gsq,i,vx[i].h);
			(void)GEOSCoordSeq_setY(gsq,i,-vx[i].v);
		}
		ggmlr=GEOSGeom_createLinearRing(gsq);
		ggmpgr=GEOSGeom_createPolygon(ggmlr,NULL,0);
	}
	else{
		if(ivs_multigon(vxs)){
// ne traite pas le cas des multigon à trou
			ggmp=(GEOSGeom*)malloc(sizeof(GEOSGeom)*n);
			for(j=0;j<n;j++){
				vx=ivs2_part(vxs,j,&np);
				gsq=GEOSCoordSeq_create(np,2);
				for(i=0;i<np;i++){
					(void)GEOSCoordSeq_setX(gsq,i,vx[i].h);
					(void)GEOSCoordSeq_setY(gsq,i,-vx[i].v);
				}
				ggmlr=GEOSGeom_createLinearRing(gsq);
				ggmp[j]=GEOSGeom_createPolygon(ggmlr,NULL,0);
			}
			ggmpgr=GEOSGeom_createCollection(GEOS_MULTIPOLYGON,ggmp,n);
			free(ggmp);
		}
		else{
			gsq=GEOSCoordSeq_create(np,2);
			for(i=0;i<np;i++){
				(void)GEOSCoordSeq_setX(gsq,i,vx[i].h);
				(void)GEOSCoordSeq_setY(gsq,i,-vx[i].v);
			}
			ggmlrf=GEOSGeom_createLinearRing(gsq);
			ggmp=(GEOSGeom*)malloc(sizeof(GEOSGeom)*n-1);
			for(j=1;j<n;j++){
				vx=ivs2_part(vxs,j,&np);
				gsq=GEOSCoordSeq_create(np,2);
				for(i=0;i<np;i++){
					(void)GEOSCoordSeq_setX(gsq,i,vx[i].h);
					(void)GEOSCoordSeq_setY(gsq,i,-vx[i].v);
				}
				ggmp[j-1]=GEOSGeom_createLinearRing(gsq);
			}
			ggmpgr=GEOSGeom_createPolygon(ggmlrf,ggmp,n-1);
			free(ggmp);
		}
	}	
	return(ggmpgr);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* geosxgon2vxs(GEOSGeom xgon){
//bTrace	trc("geosxgon2vxs",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
int			i,j,ng,nir,gid=GEOSGeomTypeId(xgon);
GEOSGeom	ggmn,ggmer,ggmir;
ivertices	*vxs=NULL,*vbf=NULL;

//    extern const GEOSGeometry GEOS_DLL *GEOSGetExteriorRing(const GEOSGeometry* g);
    
	if(gid==GEOS_POLYGON){
        ggmer=(GEOSGeometry*)GEOSGetExteriorRing(xgon);
		if(!ggmer){
			return(geosrawgeom2vxs(xgon));
		}
		vxs=geosrawgeom2vxs(ggmer);
		ng=GEOSGetNumInteriorRings(xgon);
		for(i=0;i<ng;i++){
			ggmir=(GEOSGeometry*)GEOSGetInteriorRingN(xgon,i);
			vbf=geosrawgeom2vxs(ggmir);
			vxs=ivs_group(vxs,vbf);
			ivs_free(vbf);
		}
	}
	else if(gid==GEOS_MULTIPOLYGON){
		vxs=ivs_new(_2D_VX,0,0);
		ng=GEOSGetNumGeometries(xgon);
//trc.msg("ng=%d",ng);
		for(j=0;j<ng;j++){
			ggmn=(GEOSGeometry*)GEOSGetGeometryN(xgon,j);
			if(!ggmn){
//trc.msg("ggmn[%d]==NULL",j);
				continue;
			}
			ggmer=(GEOSGeometry*)GEOSGetExteriorRing(ggmn);
			if(!ggmer){
//trc.msg("GEOSGetExteriorRing==NULL (%d)",j);
				vbf=geosrawgeom2vxs(ggmn);
				vxs=ivs_group(vxs,vbf);
				ivs_free(vbf);
				continue;
			}
			vbf=geosrawgeom2vxs(ggmer);
			vxs=ivs_group(vxs,vbf);
			ivs_free(vbf);
			nir=GEOSGetNumInteriorRings(ggmn);
//trc.msg("nir=%d",nir);
			for(i=0;i<nir;i++){
				ggmir=(GEOSGeometry*)GEOSGetInteriorRingN(ggmn,i);
				vbf=geosrawgeom2vxs(ggmir);
				vxs=ivs_group(vxs,vbf);
				ivs_free(vbf);
			}
		}
	}
	else{
	}
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
GEOSGeom vxs2geosxlin(ivertices* vxs){
//bTrace	trc("vxs2geosxlin",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
	
int				i,j;
int				np,n=ivs_n_parts(vxs);
GEOSCoordSeq	gsq;
GEOSGeom		ggmpgr=NULL;
GEOSGeom*		ggmp=NULL;
i2dvertex*		vx;

	if(n==1){
		gsq=GEOSCoordSeq_create(vxs->nv,2);
		for(i=0;i<vxs->nv;i++){
			(void)GEOSCoordSeq_setX(gsq,i,vxs->vx.vx2[i].h);
			(void)GEOSCoordSeq_setY(gsq,i,-vxs->vx.vx2[i].v);
		}
		ggmpgr=GEOSGeom_createLineString(gsq);
	}
	else{
		ggmp=(GEOSGeom*)malloc(sizeof(GEOSGeom)*n);
		for(j=0;j<n;j++){
			vx=ivs2_part(vxs,j,&np);
			gsq=GEOSCoordSeq_create(np,2);
			for(i=0;i<np;i++){
				(void)GEOSCoordSeq_setX(gsq,i,vx[i].h);
				(void)GEOSCoordSeq_setY(gsq,i,-vx[i].v);
			}
			ggmp[j]=GEOSGeom_createLineString(gsq);
		}
		ggmpgr=GEOSGeom_createCollection(GEOS_MULTILINESTRING,ggmp,n);
		free(ggmp);
	}	
	return(ggmpgr);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* geosxlin2vxs(GEOSGeom xlin){
//bTrace	trc("geosxgon2vxs",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
int			j,ng,gid=GEOSGeomTypeId(xlin);
GEOSGeom	ggmn;
ivertices	*vxs=NULL,*vbf=NULL;

	if(gid==GEOS_LINESTRING){
		vxs=geosrawgeom2vxs(xlin);
	}
	else if(gid==GEOS_MULTILINESTRING){
		vxs=ivs_new(_2D_VX,0,0);
		ng=GEOSGetNumGeometries(xlin);
//trc.msg("ng=%d",ng);
		for(j=0;j<ng;j++){
			ggmn=(GEOSGeometry*)GEOSGetGeometryN(xlin,j);
			if(!ggmn){
//trc.msg("ggmn[%d]==NULL",j);
				continue;
			}
			vbf=geosrawgeom2vxs(ggmn);
			vxs=ivs_group(vxs,vbf);
			ivs_free(vbf);
		}
	}
	else{
	}	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
GEOSGeom vxs2geosxpoint(ivertices* vxs){
//bTrace	trc("vxs2geosxpoint",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
	
int				j;
GEOSCoordSeq	gsq;
GEOSGeom		ggmpgr=NULL;
GEOSGeom*		ggmp=NULL;

	if(vxs->nv==1){
		gsq=GEOSCoordSeq_create(1,2);
		(void)GEOSCoordSeq_setX(gsq,0,vxs->vx.vx2[0].h);
		(void)GEOSCoordSeq_setY(gsq,0,-vxs->vx.vx2[0].v);
		ggmpgr=GEOSGeom_createPoint(gsq);
	}
	else{
		ggmp=(GEOSGeom*)malloc(sizeof(GEOSGeom)*vxs->nv);
		for(j=0;j<vxs->nv;j++){
			gsq=GEOSCoordSeq_create(1,2);
			if(!gsq){
//trc.err("GEOSCoordSeq_create");
			}
			(void)GEOSCoordSeq_setX(gsq,0,vxs->vx.vx2[j].h);
			(void)GEOSCoordSeq_setY(gsq,0,-vxs->vx.vx2[j].v);
			ggmp[j]=GEOSGeom_createPoint(gsq);
			if(!ggmp[j]){
//trc.err("GEOSGeom_createPoint");
			}
		}
		ggmpgr=GEOSGeom_createCollection(GEOS_MULTIPOINT,ggmp,vxs->nv);
		if(!ggmpgr){
//trc.err("GEOSGeom_createCollection");
		}
		free(ggmp);
	}	
	return(ggmpgr);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* geosxpoint2vxs(GEOSGeom xpoint){
//bTrace	trc("geosxpoint2vxs",true);
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
int			j,ng,gid=GEOSGeomTypeId(xpoint);
GEOSGeom	ggmn;
ivertices	*vxs=NULL,*vbf=NULL;

	if(gid==GEOS_POINT){
		vxs=geosrawgeom2vxs(xpoint);
	}
	else if(gid==GEOS_MULTIPOINT){
		vxs=ivs_new(_2D_VX,0,0);
		ng=GEOSGetNumGeometries(xpoint);
//trc.msg("ng=%d",ng);
		for(j=0;j<ng;j++){
			ggmn=(GEOSGeometry*)GEOSGetGeometryN(xpoint,j);
			if(!ggmn){
//trc.msg("ggmn[%d]==NULL",j);
				continue;
			}
			vbf=geosrawgeom2vxs(ggmn);
			vxs=ivs_group(vxs,vbf);
			ivs_free(vbf);
		}
	}
	else{
	}	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* geoscollect2vxs(GEOSGeom collect){
	if(_g_inited==0){
		initGEOS(g_func,g_func);
		_g_inited=1;
	}
int			j,ng;
GEOSGeom	ggmn;
ivertices	*vxs=NULL,*vbf=NULL;

	vxs=ivs_new(_2D_VX,0,0);
	ng=GEOSGetNumGeometries(collect);
	for(j=0;j<ng;j++){
		ggmn=(GEOSGeometry*)GEOSGetGeometryN(collect,j);
		if(!ggmn){
			continue;
		}
		switch(GEOSGeomTypeId(ggmn)){
			case GEOS_POLYGON:
			case GEOS_MULTIPOLYGON:
				vbf=geosxgon2vxs(ggmn);
fprintf(stderr,"geosxgon2vxs\n");				
				break;
			case GEOS_LINESTRING:
			case GEOS_LINEARRING:
			case GEOS_MULTILINESTRING:
				vbf=geosxlin2vxs(ggmn);
fprintf(stderr,"geosxlin2vxs\n");				
				break;
			case GEOS_POINT:
			case GEOS_MULTIPOINT:
				vbf=geosxpoint2vxs(ggmn);
fprintf(stderr,"geosxpoint2vxs\n");				
				break;
			default:
fprintf(stderr,"default\n");				
				vbf=NULL;
				break;
		}
		if(vbf){
			vxs=ivs_group(vxs,vbf);
			ivs_free(vbf);
		}
	}
	return(vxs);
}

