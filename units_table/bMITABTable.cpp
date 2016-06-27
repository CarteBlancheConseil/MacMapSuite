//----------------------------------------------------------------------------
// File : bMITABTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : MITAB table class (MITAB C++ wrapper)
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
// 24/05/2006 creation.
//----------------------------------------------------------------------------

#include "bMITABTable.h"
#include "bTrace.h"
#include "vx_utils.h"
#include "vx_measure.h"
#include "vx_manip.h"
#include "vx_allocation.h"
#include "base_def.h"
#include "valconv.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bMITABTable	::bMITABTable(	const char* path,
							const char* name,
							bool create, 
							double reso,
							int srid,
							int* status)
			:bStdTable(reso,0.0,0.0,srid,srid,status)
			,_arr(sizeof(mitab_feature)){
_bTrace_("bMITABTable::bMITABTable",false);
char	fpath[1024];
	_new=false;
	_ptype=db2mitab(*status);

	sprintf(fpath,"%s%s",path,name);
	_cur=-1;
	_feat=NULL;	
	if(create){
_tm_("mitab creation");
		_mt=mitab_c_create(fpath,"mif","",__BOUNDS_MAX__,-__BOUNDS_MAX__,__BOUNDS_MAX__,-__BOUNDS_MAX__);
		if(!_mt){
_te_("creation of "+fpath+" failed");
			*status=-1;
			return;
		}
		_new=true;
	}
	else{
		_mt=mitab_c_open(fpath);
_tm_("mitab opening");
		if(!_mt){
_te_("open of "+fpath+" failed");
			*status=-1;
			return;
		}
_tm_("mitab_c_get_field_count="+mitab_c_get_field_count(_mt));
int				feature_id;
		for(	feature_id = mitab_c_next_feature_id(_mt,-1);
				feature_id != -1;
				feature_id = mitab_c_next_feature_id(_mt,feature_id) ){
			_feat=mitab_c_read_feature(_mt,feature_id);
			if(_feat==NULL){
				continue;
			}
			_arr.add(&_feat);
		}
	}
	*status=0;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bMITABTable	::bMITABTable(	const char* path,
							const char* name,
							bool create, 
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
				:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status)
				,_arr(sizeof(mitab_feature)){
_bTrace_("bMITABTable::bMITABTable",false);
char	fpath[1024];
	_new=false;
	_ptype=db2mitab(*status);

	sprintf(fpath,"%s%s",path,name);
	_cur=-1;
	_feat=NULL;	
	if(create){
_tm_("mitab creation");
		_mt=mitab_c_create(fpath,"mif","",__BOUNDS_MAX__,-__BOUNDS_MAX__,__BOUNDS_MAX__,-__BOUNDS_MAX__);
		if(!_mt){
_te_("creation of "+fpath+" failed");
			*status=-1;
			return;
		}
		_new=true;
	}
	else{
		_mt=mitab_c_open(fpath);
_tm_("mitab opening");
		if(!_mt){
_te_("open of "+fpath+" failed");
			*status=-1;
			return;
		}
_tm_("mitab_c_get_field_count="+mitab_c_get_field_count(_mt));
int				feature_id;
		for(	feature_id = mitab_c_next_feature_id(_mt,-1);
				feature_id != -1;
				feature_id = mitab_c_next_feature_id(_mt,feature_id) ){
			_feat=mitab_c_read_feature(_mt,feature_id);
			if(_feat==NULL){
				continue;
			}
			_arr.add(&_feat);
		}
	}
	*status=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bMITABTable::~bMITABTable(){
	for(int i=1;i<=_arr.count();i++){
		_arr.get(i,&_feat);
		if(_new){
			mitab_c_write_feature(_mt,_feat);
		}
		mitab_c_destroy_feature(_feat);
	}
    _arr.reset();
	if(_mt){
		mitab_c_close(_mt);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::KillRecord(int o){
	return(-1);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::UnkillRecord(int	o){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::RecordKilled(int	o){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::CountRecords(){
	return(_arr.count());
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::CountFields(){
	if(!_mt){
		return(0);
	}
	return(mitab_c_get_field_count(_mt)+2);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::AddField(const char *nam, int sign, int length, int decs){
_bTrace_("bMITABTable::AddField",false);
	
//	_tm_(nam+":"+length+":"+decs);
int	sgn;
	switch(sign){
		case _char:
			sgn=TABFT_Char;
			break;
		case _int:
			sgn=TABFT_Integer;
			break;
		case _double:
			//sgn=TABFT_Float;-> ne permet pas d'utiliser la précision
			sgn=TABFT_Decimal;
			break;
		case _date:
			sgn=TABFT_Date;
			break;
		case _bool:
			sgn=TABFT_Logical;
			break;
		default:
			return(-1);
	}

	if(mitab_c_add_field(_mt,nam,sgn,length,decs,0,0)<0){
		return(-2);
	}
	
//	_tm_(mitab_c_get_field_name(_mt,mitab_c_get_field_count(_mt)-1)+":"+mitab_c_get_field_precision(_mt,mitab_c_get_field_count(_mt)-1));
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::RmvField(int f){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::ChgField(int	f, const char* nam, int sign, int length, int decs){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::FieldSign(int f, int* sign){
_bTrace_("bMITABTable::FieldSign",false);
	*sign=0;
	if(!_mt){
		return(-1);
	}
	if(f==mitab_c_get_field_count(_mt)+1){
		*sign=_ivxs2;
		return(0);
	}
	else if(f==mitab_c_get_field_count(_mt)+2){
		*sign=_ivxrect;
		return(0);
	}
	else if(f>mitab_c_get_field_count(_mt)+2){
_te_(f+">"+(mitab_c_get_field_count(_mt)+2));
		return(-1);
	}
int	k=mitab_c_get_field_type(_mt,f-1);
	switch(k){
		case TABFT_Char:
			*sign=_char;
			break;
		case TABFT_Integer:
		case TABFT_SmallInt:
			*sign=_int;
			break;
		case TABFT_Decimal:
		case TABFT_Float:
			*sign=_double;
			break;
		case TABFT_Date:
			*sign=_date;
			break;
		case TABFT_Logical:
			*sign=_bool;
			break;
		default:
_te_("bad TABFT sign "+k);			
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::FieldName(int f, char* name){
	sprintf(name,"");
	if(!_mt){
		return(-1);
	}
	if(f==mitab_c_get_field_count(_mt)+1){
		sprintf(name,"VERTICES");
		return(0);
	}
	else if(f==mitab_c_get_field_count(_mt)+2){
		sprintf(name,"BOUNDS");
		return(0);
	}
	else if(f>mitab_c_get_field_count(_mt)+2){
		return(-1);
	}
const char* c=mitab_c_get_field_name(_mt,f-1);
	if(c){
		strcpy(name,c);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::FieldLength(int f, int* len){
	*len=0;
	if(!_mt){
		return(-1);
	}
	if(f>mitab_c_get_field_count(_mt)){
		return(0);
	}
int	k=mitab_c_get_field_type(_mt,f-1);
	switch(k){
		case TABFT_Char:
			*len=mitab_c_get_field_width(_mt,f-1);
			break;
		case TABFT_Integer:
		case TABFT_SmallInt:
		case TABFT_Logical:
			*len=_kIntLen;
			break;
		case TABFT_Decimal:
		case TABFT_Float:
			*len=_kDoubleLen;
			break;
		case TABFT_Date:
			*len=_kDateTimeLen;
			break;
		default:
			*len=mitab_c_get_field_width(_mt,f-1);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::FieldDecimals(int f, int* decs){
	*decs=0;
	if(!_mt){
		return(-1);
	}
	if(f>mitab_c_get_field_count(_mt)){
		return(0);
	}
	*decs=mitab_c_get_field_precision(_mt,f-1);
	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::FieldSize(int f, int* sz){
	*sz=0;
	if(!_mt){
		return(-1);
	}
	if(f==mitab_c_get_field_count(_mt)+1){
		*sz=sizeof(ivertices*);
		return(0);
	}
	else if(f==mitab_c_get_field_count(_mt)+2){
		*sz=sizeof(ivx_rect);
		return(0);
	}
	else if(f>mitab_c_get_field_count(_mt)+2){
		return(-1);
	}
int	k=mitab_c_get_field_type(_mt,f-1);
	switch(k){
		case TABFT_Char:
			*sz=mitab_c_get_field_width(_mt,f-1)+1;
			break;
		case TABFT_Integer:
		case TABFT_SmallInt:
		case TABFT_Logical:
			*sz=sizeof(int);
			break;
		case TABFT_Decimal:
		case TABFT_Float:
		case TABFT_Date:
			*sz=sizeof(double);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::ReadVal(int o, int f, void* val){
_bTrace_("bMITABTable::ReadVal",false);
int sgn;
	if(FieldSign(f,&sgn)){
_te_("FieldSign error");
		return(-1);
	}

	_feat=NULL;
	_arr.get(o,&_feat);
	
	if(!_feat){
_te_("_feat==NULL");
		_cur=0;
		return(-2);
	}
	
int	res=0;
	switch(sgn){
		case _int:
		case _bool:
			(*(int*)val)=round(mitab_c_get_field_as_double(_feat,f-1));
			break;
		case _double:
		case _date:
			(*(double*)val)=mitab_c_get_field_as_double(_feat,f-1);
			break;
		case _char:
			strcpy((char*)val,mitab_c_get_field_as_string(_feat,f-1));
			break;
		case _ivxs2:
			(*(ivertices**)val)=feat2vxs(_feat,&res);
			break;
		case _ivxrect:{
ivertices*	vxs=feat2vxs(_feat,&res);
			ivs_bounds(vxs,(ivx_rect*)val);
			ivs_free(vxs);
			}break;
		default:
			return(-1);
	}
	return(res);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::WriteVal(int o, int f, void* val){
_bTrace_("bMITABTable::WriteVal",false);
int sgn,decs;
	if(FieldSign(f,&sgn)){
_te_("FieldSign error");
		return(-1);
	}
	if(FieldDecimals(f,&decs)){
_te_("FieldDecimals error");
		return(-2);
	}
	
	_feat=NULL;
	if(o==_arr.count()+1){
		if(sgn!=_ivxs2){
_te_("could not append on non geom field");
			return(-3);			
		}
ivertices*	vxs=(*(ivertices**)val);
		_feat=vxs2feat(vxs);
		if(_feat){
			_arr.add(&_feat);
			_cur=o;
		}
		else{
_te_("vxs2feat failed");
		}
		return(0);
	}
	else{
		_arr.get(o,&_feat);
	}
	if(!_feat){
_te_("_feat==NULL");
		_cur=0;
		return(-4);
	}
	_cur=o;

char	bf[1024];
	
/*	if(sgn==_double){
_tm_("nb decs="+decs);
	}*/
	xToChar(sgn,decs,val,bf);
	mitab_c_set_field(_feat,f-1,bf);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::signature(){
	return(kTableMITAB);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bMITABTable::feat2vxs(mitab_feature o, int* kind){
int	typ=mitab_c_get_type(o);
	switch(typ){
		case TABFC_NoGeom:
		case TABFC_Point:
		case TABFC_FontPoint:
		case TABFC_CustomPoint:
		case TABFC_MultiPoint:
			*kind=kBaseKindPoint;
			break;
		case TABFC_Polyline:
			*kind=kBaseKindPolyline;
			break;
		case TABFC_Region:
			*kind=kBaseKindPolygon;
			break;
		case TABFC_Text:
			*kind=kBaseKindText;
			break;
		case TABFC_Arc:
		case TABFC_Rectangle:
		case TABFC_Ellipse:
			*kind=kBaseNoKind;
			break;
	}

int	np=(mitab_c_get_parts(o)>1)?mitab_c_get_parts(o):0;
int	nv=0;
int	i,j;
	for(i=0;i<mitab_c_get_parts(o);i++){
		nv+=mitab_c_get_vertex_count(o,i);
	}

dvertices*	dvxs=dvs_new(_2D_VX,nv,np);
	if(np){
		nv=0;
		for(i=0;i<mitab_c_get_parts(o);i++){
			dvxs->offs[i]=nv;
			nv+=mitab_c_get_vertex_count(o,i);
		}
	}
	nv=0;
	for(i=0;i<mitab_c_get_parts(o);i++){
		for(j=0;j<mitab_c_get_vertex_count(o,i);j++){
			dvxs->vx.vx2[nv+j].x=mitab_c_get_vertex_x(o,i,j);
			dvxs->vx.vx2[nv+j].y=mitab_c_get_vertex_y(o,i,j);
		}
		nv+=mitab_c_get_vertex_count(o,i);
	}
	transform_t2a(dvxs);
	dvs_move(dvxs,-_ox,-_oy);
ivertices*	vxs;
	vxs_d2i(&vxs,dvxs,_reso);
	dvs_free(dvxs);
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
mitab_feature bMITABTable::vxs2feat(ivertices* vxs){
int				fttype;
mitab_feature	feat=NULL;

	switch(_ptype){
		case TABFC_MultiPoint:
			if(vxs->nv>1){
				fttype=TABFC_MultiPoint;
			}
			else{
				fttype=TABFC_Point;
			}
			break;
		case TABFC_Polyline:
			fttype=TABFC_Polyline;
			break;
		case TABFC_Region:
			fttype=TABFC_Region;
			break;
		default:
			if(vxs->nv==1){
				fttype=TABFC_Point;
			}
			else if(ivs_closed(vxs)){
				fttype=TABFC_Region;
			}
			else{
				fttype=TABFC_Polyline;
			}
			break;
	}
	feat=mitab_c_create_feature(_mt,fttype);
	if(!feat){
//_te_("mitab_c_create_feature failed");
		return(NULL);
	}
//i2dvertex*	p;
d2dvertex*	p;
//d2dvertex	dvx;
int			nv;
double		*x,*y;

dvertices*	dvxs;

	vxs_i2d(&dvxs,vxs,_reso);
	dvs_move(dvxs,_ox,_oy);
	transform_a2t(dvxs);

	for(int i=0;i<dvs_n_parts(dvxs);i++){
		p=dvs2_part(dvxs,i,&nv);
		x=new double[nv];
		y=new double[nv];
		for(int j=0;j<nv;j++){
			x[j]=p[j].x;
			y[j]=p[j].y;		
		}
		mitab_c_set_points(feat,i,nv,x,y);
		delete[] x;
		delete[] y;
	}
	dvs_free(dvxs);
	return(feat);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMITABTable::db2mitab(int sign){
int r=0;
	switch(sign){
		case kVXPoint:
		case 3:
			r=TABFC_MultiPoint;
			break;
		case kVXPolyline:
			r=TABFC_Polyline;
			break;
		case kVXPolygon:
		case 5:
			r=TABFC_Region;
			break;
		default:
			r=TABFC_NoGeom;
			break;
	}
	return(r);
}
