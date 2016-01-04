//----------------------------------------------------------------------------
// File : bStdProj.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Proj base class (Proj4 C++ wrapper)
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
// 10/02/2006 creation.
//----------------------------------------------------------------------------

#include "bStdProj.h"
#include "bTrace.h"
#include "wtable.h"
#include "bLocalDBTable.h"
#include "wproj.h"
#include "proj_api.h"
#include "vx_utils.h"

#pragma mark --bStdProjExt--
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdProj::bStdProj(const char* projdef){
	_pj=pj_init_plus(projdef);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdProj::bStdProj(int srid, bool *init){
	*init=false;
	_pj=NULL;
	if(srid<=0){
		return;
	}
_bTrace_("bStdProj::bStdProj(int)",false);
bStdTable*	tbl=wprj_gettable();
	if(tbl==NULL){
_te_("tbl not found");
		return;
	}
int		bf;
char	projdef[1024];

	for(int i=1;i<=tbl->CountRecords();i++){
		tbl->ReadVal(i,1,&bf);
		if(bf==srid){
			tbl->ReadVal(i,3,projdef);
			_pj=pj_init_plus(projdef);
			break;
		}
	}
	delete tbl;
	if(!_pj){
_te_("proj not defined");
	}
	*init=(_pj!=NULL);
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdProj::~bStdProj(){
	if(_pj){
		pj_free(_pj);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdProj::transform(bStdProj& src, dvertices* vxs){
	if(!_pj||!src._pj){
		return(false);
	}
/*	if(strcmp(_pj,src._pj)==0){
		return(true);
	}*/
double*	x=new double[vxs->nv];
double*	y=new double[vxs->nv];
double*	z=new double[vxs->nv];
	if(vxs->sign==_3D_VX){
		if(src.is_latlong()){
			for(int i=0;i<vxs->nv;i++){
				x[i]=deg2rad(vxs->vx.vx3[i].x);
				y[i]=deg2rad(vxs->vx.vx3[i].y);
				z[i]=deg2rad(vxs->vx.vx3[i].z);
			}
		}
		else{
			for(int i=0;i<vxs->nv;i++){
				x[i]=vxs->vx.vx3[i].x;
				y[i]=vxs->vx.vx3[i].y;
				z[i]=vxs->vx.vx3[i].z;
			}
		}
	}
	else{
		if(src.is_latlong()){
			for(int i=0;i<vxs->nv;i++){
				x[i]=deg2rad(vxs->vx.vx2[i].x);
				y[i]=deg2rad(vxs->vx.vx2[i].y);
				z[i]=0;
			}
		}
		else{
			for(int i=0;i<vxs->nv;i++){
				x[i]=vxs->vx.vx2[i].x;
				y[i]=vxs->vx.vx2[i].y;
				z[i]=0;
			}
		}
	}
	if(pj_transform(src._pj,_pj,vxs->nv,0,x,y,z)){
        delete[] x;
		delete[] y;
		delete[] z;
		return(false);
	}
	if(vxs->sign==_3D_VX){
		if(is_latlong()){
			for(int i=0;i<vxs->nv;i++){
				vxs->vx.vx3[i].x=rad2deg(x[i]);
				vxs->vx.vx3[i].y=rad2deg(y[i]);
				vxs->vx.vx3[i].z=rad2deg(z[i]);
			}
		}
		else{
			for(int i=0;i<vxs->nv;i++){
				vxs->vx.vx3[i].x=x[i];
				vxs->vx.vx3[i].y=y[i];
				vxs->vx.vx3[i].z=z[i];
			}
		}
	}
	else{
		if(is_latlong()){
			for(int i=0;i<vxs->nv;i++){
				vxs->vx.vx2[i].x=rad2deg(x[i]);
				vxs->vx.vx2[i].y=rad2deg(y[i]);
			}
		}
		else{
			for(int i=0;i<vxs->nv;i++){
				vxs->vx.vx2[i].x=x[i];
				vxs->vx.vx2[i].y=y[i];
			}
		}
	}
	delete[] x;
	delete[] y;
	delete[] z;
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdProj::is_latlong(){
	if(!_pj){
		return(false);
	}
	return(pj_is_latlong(_pj));
}

#pragma mark --bStdProjExt--
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdProjExt::bStdProjExt(int srid, bool *init)
			:bStdProj(srid,init){
	_name=NULL;
	if(*init){
bStdTable*	tbl=wprj_gettable();
		if(tbl==NULL){
			return;
		}
int		bf;
char	name[1024];
		for(int i=1;i<=tbl->CountRecords();i++){
			tbl->ReadVal(i,1,&bf);
			if(bf==srid){
				tbl->ReadVal(i,2,name);
				_name=strdup(name);
				break;
			}
		}
		delete tbl;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdProjExt::~bStdProjExt(){
	if(_name){
		free(_name);
	}
}

