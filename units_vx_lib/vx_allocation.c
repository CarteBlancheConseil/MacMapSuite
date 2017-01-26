//----------------------------------------------------------------------------
// File : vx_allocation.c
// Project : MacMapSuite
// Purpose : C source file : Geometry allocation / deallocation utils
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
// 20/08/2002 creation.
//----------------------------------------------------------------------------

#include "vx_allocation.h"
#include "vx_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// 
// -----------
int sizeofivs2(int	nv,	int	no){
	return(sizeof(ivertices)-sizeof(i3dvertex)+nv*sizeof(i2dvertex)+no*sizeof(int));
}

// ---------------------------------------------------------------------------
// 
// -----------
int sizeofivs3(int	nv,	int	no){
	return(sizeof(ivertices)-sizeof(i3dvertex)+nv*sizeof(i3dvertex)+no*sizeof(int));
}

// ---------------------------------------------------------------------------
// 
// -----------
int sizeofdvs2(int	nv,	int	no){
	return(sizeof(dvertices)-sizeof(d3dvertex)+nv*sizeof(d2dvertex)+no*sizeof(int));
}

// ---------------------------------------------------------------------------
// 
// -----------
int sizeofdvs3(int	nv,	int	no){
	return(sizeof(dvertices)-sizeof(d3dvertex)+nv*sizeof(d3dvertex)+no*sizeof(int));
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivssize(ivertices* vxs, int* hSz, int* vSz, int* oSz){
	(*hSz)=(sizeof(ivertices)-sizeof(i3dvertex));
	(*oSz)=(vxs->no*sizeof(int));
	if(vxs->sign==_2D_VX){
		(*vSz)=(vxs->nv*sizeof(i2dvertex));
		return((*hSz)+(*vSz)+(*oSz));
	}
	else if(vxs->sign==_3D_VX){
		(*vSz)=(vxs->nv*sizeof(i3dvertex));
		return((*hSz)+(*vSz)+(*oSz));
	}
	else{
		(*hSz)=0;
		(*vSz)=0;
		(*oSz)=0;
		return(0);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int dvssize(dvertices* vxs, int* hSz, int* vSz, int* oSz){
	(*hSz)=(sizeof(dvertices)-sizeof(d3dvertex));
	(*oSz)=(vxs->no*sizeof(int));
	if(vxs->sign==_2D_VX){
		(*vSz)=(vxs->nv*sizeof(d2dvertex));
		return((*hSz)+(*vSz)+(*oSz));
	}
	else if(vxs->sign==_3D_VX){
		(*vSz)=(vxs->nv*sizeof(d3dvertex));
		return((*hSz)+(*vSz)+(*oSz));
	}
	else{
		(*hSz)=0;
		(*vSz)=0;
		(*oSz)=0;
		return(0);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivs_new(int kind, int nv, int no){
int			sz=(kind==_2D_VX)?nv*sizeof(i2dvertex):nv*sizeof(i3dvertex);
ivertices*	vxs=(ivertices*)malloc((sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!vxs){
		return(NULL);
	}
	(void)memset(vxs,0,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(no>0){
		vxs->offs=(int*)malloc(no*sizeof(int));
		if(!vxs->offs){
			free(vxs);
			return(NULL);
		}
		memset(vxs->offs,0,no*sizeof(int));
		vxs->no=no;
	}
	else{
		vxs->offs=NULL;
		vxs->no=0;
	}
	vxs->sign=kind;
	vxs->nv=nv;
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_free(ivertices* vxs){
//fprintf(stderr,"ivs_free %x\n",(int)vxs);
	if(!vxs){
		return;
	}
	if(vxs->offs){
		free(vxs->offs);
	}
	free(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices*	dvs_new(int kind, int nv, int no){
int			sz=(kind==_2D_VX)?nv*sizeof(d2dvertex):nv*sizeof(d3dvertex);
dvertices*	vxs=(dvertices*)malloc((sizeof(dvertices)-sizeof(d3dvertex))+sz);
	if(!vxs){
		return(NULL);
	}
	(void)memset(vxs,0,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
	if(no>0){
		vxs->offs=(int*)malloc(no*sizeof(int));
		if(!vxs->offs){
			free(vxs);
			return(NULL);
		}
		memset(vxs->offs,0,no*sizeof(int));
		vxs->no=no;
	}
	else{
		vxs->offs=NULL;
		vxs->no=0;
	}
	vxs->sign=kind;
	vxs->nv=nv;
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void dvs_free(dvertices* vxs){
	if(!vxs){
		return;
	}
	if(vxs->offs){
		free(vxs->offs);
	}
	free(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivs2ivs(ivertices *vin, ivertices **vxs){	
	(*vxs)=ivs_new(vin->sign,vin->nv,vin->no);
	if(!(*vxs)){
		return(-2);
	}
	if(vin->offs){
		memmove((*vxs)->offs,vin->offs,vin->no*sizeof(int));
	}
	if(vin->sign==_2D_VX){
		memmove((*vxs)->vx.vx2,&vin->vx.vx2,vin->nv*sizeof(i2dvertex));
	}
	else{
		memmove((*vxs)->vx.vx3,vin->vx.vx3,vin->nv*sizeof(i3dvertex));
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int dvs2dvs(dvertices *vin, dvertices **vxs){	
	(*vxs)=dvs_new(vin->sign,vin->nv,vin->no);
	if(!(*vxs)){
		return(-2);
	}
	if(vin->offs){
		memmove((*vxs)->offs,vin->offs,vin->no*sizeof(int));
	}
	if(vin->sign==_2D_VX){
		memmove((*vxs)->vx.vx2,&vin->vx.vx2,vin->nv*sizeof(d2dvertex));
	}
	else{
		memmove((*vxs)->vx.vx3,vin->vx.vx3,vin->nv*sizeof(d3dvertex));
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivr2ivs(ivx_rect *vr, ivertices **vxs){	
	(*vxs)=ivs_new(_2D_VX,5,0);
	if(!(*vxs)){
		return(-2);
	}
	(*vxs)->vx.vx2[0].h=vr->left;
	(*vxs)->vx.vx2[0].v=vr->bottom;
	(*vxs)->vx.vx2[1].h=vr->right;
	(*vxs)->vx.vx2[1].v=vr->bottom;
	(*vxs)->vx.vx2[2].h=vr->right;
	(*vxs)->vx.vx2[2].v=vr->top;
	(*vxs)->vx.vx2[3].h=vr->left;
	(*vxs)->vx.vx2[3].v=vr->top;
	(*vxs)->vx.vx2[4]=(*vxs)->vx.vx2[0];
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivspart2ivs(ivertices *vin, int part, ivertices **vxs){
int			n;
	(*vxs)=NULL;
	if(vin->sign==_2D_VX){
i2dvertex*	p=ivs2_part(vin,part,&n);
		if(p==NULL){
			return(-1);
		}
		(*vxs)=ivs_new(_2D_VX,n,0);
		memmove((void*)(*vxs)->vx.vx2,(void*)p,n*sizeof(i2dvertex));
	}
	else if(vin->sign==_3D_VX){
i3dvertex*	p=ivs3_part(vin,part,&n);
		if(p==NULL){
			return(-1);
		}
		(*vxs)=ivs_new(_3D_VX,n,0);
		memmove((void*)(*vxs)->vx.vx3,(void*)p,n*sizeof(i3dvertex));
	}
	else{
		return(-2);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int dvspart2dvs(dvertices *vin, int part, dvertices **vxs){
int			n;
	(*vxs)=NULL;
	if(vin->sign==_2D_VX){
d2dvertex*	p=dvs2_part(vin,part,&n);
		if(p==NULL){
			return(-1);
		}
		(*vxs)=dvs_new(_2D_VX,n,0);
		memmove((void*)(*vxs)->vx.vx2,(void*)p,n*sizeof(d2dvertex));
	}
	else if(vin->sign==_3D_VX){
d3dvertex*	p=dvs3_part(vin,part,&n);
		if(p==NULL){
			return(-1);
		}
		(*vxs)=dvs_new(_3D_VX,n,0);
		memmove((void*)(*vxs)->vx.vx3,(void*)p,n*sizeof(d3dvertex));
	}
	else{
		return(-2);
	}
	return(0);
}

// ---------------------------------------------------------------------------
//
// -----------
ivertices* nth_ivs(ivertices *vxs, long part){
i2dvertex*  vxpart;
int         nvx;
ivertices*  res;
    
    vxpart=ivs2_part(vxs,part,&nvx);
    if(vxpart==NULL){
        return NULL;
    }
    if(nvx<=0){
        return NULL;
    }
    res=ivs_new(_2D_VX,nvx,0);
    if(res==NULL){
        return NULL;
    }
    memmove(res->vx.vx2,vxpart,nvx*sizeof(i2dvertex));
    return(res);
}

