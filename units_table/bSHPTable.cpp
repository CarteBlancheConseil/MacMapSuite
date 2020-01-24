//----------------------------------------------------------------------------
// File : bSHPTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Shape table class (ShapeLib C++ wrapper)
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
// 21/11/2005 creation.
//----------------------------------------------------------------------------

#include "bSHPTable.h"
#include "bTrace.h"
#include "vx_utils.h"
#include "vx_manip.h"
#include "vx_allocation.h"
#include "lambert93.h"
#include "wgs84.h"
#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bSHPTable	::bSHPTable(	const char* path,
							const char* name,
							bool create, 
							double reso,
							int srid,
							int* status)
			:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bSHPTable::bSHPTable",false);
int		ptype=db2shp(*status);
char	fpath[1024];
	
	sprintf(fpath,"%s%s",path,name);
                
    if(create){
        _shp=SHPCreate(fpath,ptype);
        if(!_shp){
_te_(fpath+" : creation failed");
            *status=-1;
            return;
        }
        *status=0;
        return;
    }
                
    _shp=SHPOpen(fpath,"rb+");
	if(!_shp){
		_shp=SHPOpen(fpath,"rb");
	}
	if(_shp){
int		count;
double	bmin[4],bmax[4];
		SHPGetInfo(_shp,&count,&ptype,bmin,bmax);
		*status=shp2db(ptype);
		return;
	}
_te_(fpath+" : table not found");
    *status=-1;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bSHPTable	::bSHPTable(	const char* path,
							const char* name,
							bool create, 
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
			:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status){
_bTrace_("bSHPTable::bSHPTable",false);
int			ptype=db2shp(*status);
	
char	fpath[1024];
	
	sprintf(fpath,"%s%s",path,name);

    if(create){
        _shp=SHPCreate(fpath,ptype);
        if(!_shp){
_te_(fpath+" : creation failed");
            *status=-1;
            return;
        }
        *status=0;
        return;
    }
                
    _shp=SHPOpen(fpath,"rb+");
    if(!_shp){
        _shp=SHPOpen(fpath,"rb");
    }
    if(_shp){
int     count;
double  bmin[4],bmax[4];
        SHPGetInfo(_shp,&count,&ptype,bmin,bmax);
        *status=shp2db(ptype);
        return;
    }
_te_(fpath+" : table not found");
    *status=-1;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bSHPTable::~bSHPTable(){
	if(_shp){
		SHPClose(_shp);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::KillRecord(int o){
	return(-1);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::UnkillRecord(int	o){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::RecordKilled(int	o){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::CountRecords(){
	if(!_shp){
		return(0);
	}
int		count;
int		ptype;
double	bmin[4],bmax[4];
	SHPGetInfo(_shp,&count,&ptype,bmin,bmax);
	return(count);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::CountFields(){
	return(1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::AddField(const char *nam, int sign, int length, int decs){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::RmvField(int f){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::ChgField(int	f, const char* nam, int sign, int length, int decs){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::FieldSign(int f, int* sign){
	*sign=_ivxs2;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::FieldName(int f, char* name){
	sprintf(name,"VERTICES");
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::FieldLength(int f, int* len){
	*len=0;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::FieldDecimals(int	f,	int *decs){
	*decs=0;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::FieldSize(int	f,	int *sz){
	*sz=sizeof(ivertices*);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::ReadVal(int o, int f, void* val){
SHPObject*	obj=SHPReadObject(_shp,o-1);
	if(!obj){
		return(-1);
	}
	(*(ivertices**)val)=shape2vxs(obj);
	SHPDestroyObject(obj);
	if((*(ivertices**)val)==NULL){
		return(-2);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::WriteVal(int o, int f, void* val){
	if((o=CountRecords()+1)){
		o=0;
	}
SHPObject*	obj=vxs2shape(o-1,(*(ivertices**)val));
	if(!obj){
		return(-1);
	}
	if(SHPWriteObject(_shp,o-1,obj)){
	}
	else{
	}
	SHPDestroyObject(obj);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::signature(){
	return(kTableShape);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bSHPTable::shape2vxs(SHPObject* o){
ivertices*	vxs=NULL;
	if(o->nVertices>0){
int			np=(o->nParts>1)?o->nParts:0;
		if(transform_ready()==false){
d2dvertex	dvx;
			vxs=ivs_new(_2D_VX,o->nVertices,np);
			if(np){
				memmove(vxs->offs,o->panPartStart,vxs->no*sizeof(int));
			}
			for(int i=0;i<o->nVertices;i++){
				dvx.x=o->padfX[i]-_ox;
				dvx.y=o->padfY[i]-_oy;
				vx_d2i(&vxs->vx.vx2[i],&dvx,_reso);
			}
		}
		else{
dvertices*	dvxs=dvs_new(_2D_VX,o->nVertices,np);
			if(np){
				memmove(dvxs->offs,o->panPartStart,dvxs->no*sizeof(int));
			}
			for(int i=0;i<o->nVertices;i++){
				dvxs->vx.vx2[i].x=o->padfX[i];
				dvxs->vx.vx2[i].y=o->padfY[i];
			}
			transform_t2a(dvxs);
			dvs_move(dvxs,-_ox,-_oy);
			vxs_d2i(&vxs,dvxs,_reso);
			dvs_free(dvxs);
		}
	}
	else{
		vxs=ivs_new(_2D_VX,1,0);
		vxs->vx.vx2[0].h=0;
		vxs->vx.vx2[0].v=0;
	}
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
SHPObject* bSHPTable::vxs2shape(int o, ivertices* vxs){
int			count;
int			ptype;
double		bmin[4],bmax[4];
SHPObject*	obj;
	SHPGetInfo(_shp,&count,&ptype,bmin,bmax);

	if(ptype==SHPT_NULL){
		obj=SHPCreateSimpleObject(ptype,0,NULL,NULL,NULL);
	}
	else{
double*		x=(double*)malloc(ivs_n_vxs(vxs)*sizeof(double));
double*		y=(double*)malloc(ivs_n_vxs(vxs)*sizeof(double));
int			np=(ivs_n_parts(vxs)>1)?ivs_n_parts(vxs):0;
int*		p=(ivs_n_parts(vxs)>1)?(int*)malloc(ivs_n_parts(vxs)*sizeof(int)):NULL;
int*		pt=(ivs_n_parts(vxs)>1)?(int*)malloc(ivs_n_parts(vxs)*sizeof(int)):NULL;

dvertices*	dvxs=NULL;
		vxs_i2d(&dvxs,vxs,_reso);
		dvs_move(dvxs,_ox,_oy);
		transform_a2t(dvxs);

		if(p){
			memmove(p,vxs->offs,vxs->no*sizeof(int));
			memset(pt,SHPP_RING,vxs->no*sizeof(int));
		}
		for(int i=0;i<ivs_n_vxs(vxs);i++){
			x[i]=dvxs->vx.vx2[i].x;
			y[i]=dvxs->vx.vx2[i].y;
		}
		dvs_free(dvxs);
		obj=SHPCreateObject(ptype,o-1,np,p,pt,ivs_n_vxs(vxs),x,y,NULL,NULL);
	}
	return(obj);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::db2shp(int sign){
int r=0;
	switch(sign){
		case kVXPoint:
		case 3://kText_:
			r=SHPT_MULTIPOINT;
			break;
		case kVXPolyline:
			r=SHPT_ARC;
			break;
		case kVXPolygon:
		case 5://kRaster_:
			r=SHPT_POLYGON;
			break;
		default:
			r=SHPT_NULL;
			break;
	}
	return(r);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSHPTable::shp2db(int sign){
int r=0;
	switch(sign){
		case SHPT_POINT:
		case SHPT_MULTIPOINT:
		case SHPT_POINTZ:
		case SHPT_MULTIPOINTZ:
		case SHPT_POINTM:
		case SHPT_MULTIPOINTM:
			r=kVXPoint;
			break;
			
		case SHPT_ARC:
		case SHPT_ARCZ:
		case SHPT_ARCM:
			r=kVXPolyline;
			break;
		
		case SHPT_POLYGON:
		case SHPT_POLYGONZ:
		case SHPT_POLYGONM:
			r=kVXPolygon;
			break;
		default:
			break;
	}
	return(r);
}
