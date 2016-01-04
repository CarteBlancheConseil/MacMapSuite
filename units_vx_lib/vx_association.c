//----------------------------------------------------------------------------
// File : vx_association.c
// Project : MacMapSuite
// Purpose : C source file : Geometry association utils
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
// 16/12/2005 creation.
//----------------------------------------------------------------------------

#include "vx_association.h"
#include "vx_allocation.h"
#include "vx_measure.h"
#include "vx_manip.h"
#include "vx_utils.h"
#include "vx_raw.h"
#include "vx_conv_geos.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>

// ---------------------------------------------------------------------------
// 
// -----------
static double vx_deg_90(i2dvertex* bp1, i2dvertex* bp2){
double	d=vx_theta(bp1,bp2);
	while(d>M_PI*2.0){
		d-=(M_PI*2.0);
	}
	while(d<=0){
		d+=(M_PI*2.0);
	}
	d=rad2deg(d)*180.0/M_PI+90.0;
	if(d>=360.0){
		d-=360.0;
	}
	return(rad2deg(vx_theta(bp1,bp2))+90.0);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_associate_as_line(ivertices* vsa, ivertices* vsb, double dmax){
ivertices*	res=NULL;
ivertices*	buff=NULL;
	if((vsa->nv<2)||(vsb->nv<2)){
fprintf(stderr,"ivs_associate_as_line:(vsa->nv<2)||(vsb->nv<2)\n");
		return(res);
	}
	if((ivs_n_parts(vsa)>1)||(ivs_n_parts(vsb)>1)){
fprintf(stderr,"ivs_associate_as_line:(ivs_n_parts(vsa)==%d)||(ivs_n_parts(vsb)==%d)\n",ivs_n_parts(vsa),ivs_n_parts(vsb));
		return(res);
	}
dst_end_kind	cas;
double			dmin=vxs_dist_end(vsa,vsb,&cas);
	if(dmin>dmax){
fprintf(stderr,"ivs_associate_as_line:dmin>dmax\n");
		return(res);
	}

	switch(cas){
		case kDistEndKindAC:
			ivs2ivs(vsb,&res);
			ivs_reverse(res);
			res=ivs_concat(res,vsa);
			break;
		case kDistEndKindAD:
			ivs2ivs(vsb,&res);
			res=ivs_concat(res,vsa);
			break;
		case kDistEndKindBC:
			ivs2ivs(vsa,&res);
			res=ivs_concat(res,vsb);
			break;
		case kDistEndKindBD:
			ivs2ivs(vsa,&res);
			ivs2ivs(vsb,&buff);
			ivs_reverse(buff);
			res=ivs_concat(res,buff);
			ivs_free(buff);
			break;
	}
	return(res);
}

// ---------------------------------------------------------------------------
// TO DO
// -----------
ivertices* ivs_associate_as_surf(ivertices* gvsa, ivertices* gvsb, double dmax){
//fprintf(stderr,"ivs_associate_as_surf::begin\n");
//fflush(stderr);
	if((gvsa->no>1)||(gvsb->no>1)){
fprintf(stderr,"ivs_associate_as_surf : nb parts>0 : %d %d\n",gvsa->no,gvsb->no);
		return(NULL);
	}
	if((!ivs_closed(gvsa))||(!ivs_closed(gvsb))){
fprintf(stderr,"ivs_associate_as_surf : not closed : %d %d\n",ivs_closed(gvsa),ivs_closed(gvsb));
		return(NULL);
	}
ivertices*	vsa=NULL;
ivertices*	vsb=NULL;
ivertices*	vsr=NULL;
ivertices*	splits[6]={NULL,NULL,NULL,NULL,NULL,NULL};
ivertices*	buf;
double		*da=NULL,*db=NULL,*dbuf=NULL;
double		dmin=LONG_MAX,fmax;
int			i,amax,bmax/*,j*/;
int			af=-1,al=-1,bf=-1,bl=-1;
int			ains=0,bins=0;
i2dvertex	vr;

	for(;;){
		if(ivs2ivs(gvsa,&vsa)){
fprintf(stderr,"ivs_associate_as_surf : ivs2ivs a\n");
			break;
		}
		if(ivs2ivs(gvsb,&vsb)){
fprintf(stderr,"ivs_associate_as_surf : ivs2ivs b\n");
			break;
		}
		if(ivx2_clockwise(vsa->vx.vx2,vsa->nv)!=1){
			ivs_reverse(vsa);
		}
		if(ivx2_clockwise(vsb->vx.vx2,vsb->nv)!=1){
			ivs_reverse(vsb);
		}

// insertion des éventuels points d'intersection
		ivs_sect_inserts(&vsa,&vsb);

// calcul des distances
		da=(double*)malloc((vsa->nv-1)*sizeof(double));
		if(!da){
fprintf(stderr,"ivs_associate_as_surf : malloc : a\n");
			break;
		}
		db=(double*)malloc((vsb->nv-1)*sizeof(double));
		if(!db){
fprintf(stderr,"ivs_associate_as_surf : malloc : b\n");
			break;
		}
		
		fmax=0;
		for(i=0;i<vsa->nv-1;i++){
			if(ivx_in_ivs(&vsa->vx.vx2[i],vsb,1)){
				da[i]=0;
				ains++;
			}
			else{
				da[i]=ivx2ivs_dist(&vsa->vx.vx2[i],vsb);			
			}
			if(da[i]<dmin){
				dmin=da[i];
			}
			if(da[i]>fmax){
				fmax=da[i];
				amax=i;
			}		
		}
		fmax=0;
		for(i=0;i<vsb->nv-1;i++){
			if(ivx_in_ivs(&vsb->vx.vx2[i],vsa,1)){
				db[i]=0;
				bins++;
			}
			else{
				db[i]=ivx2ivs_dist(&vsb->vx.vx2[i],vsa);			
			}			
			if(db[i]<dmin){
				dmin=db[i];
			}
			if(db[i]>fmax){
				fmax=db[i];
				bmax=i;
			}
		}

fprintf(stderr,"amax=%d, bmax=%d\n",amax,bmax);

		ivs_shift_origin(vsa,amax);
		if((amax>0)&&(amax<vsa->nv-1)){
			dbuf=(double*)malloc((vsa->nv-1)*sizeof(double));
			if(!dbuf){
				break;
			}
			memmove(dbuf,da,(vsa->nv-1)*sizeof(double));
			memmove(&da[0],&dbuf[amax],((vsa->nv-1)-amax)*sizeof(double));
			memmove(&da[((vsa->nv-1)-amax)],&dbuf[0],amax*sizeof(double));
			free(dbuf);
		}
		
		ivs_shift_origin(vsb,bmax);
		if((bmax>0)&&(bmax<vsb->nv-1)){
			dbuf=(double*)malloc((vsb->nv-1)*sizeof(double));
			if(!dbuf){
				break;
			}
			memmove(dbuf,db,(vsb->nv-1)*sizeof(double));
			memmove(&db[0],&dbuf[bmax],((vsb->nv-1)-bmax)*sizeof(double));
			memmove(&db[((vsb->nv-1)-bmax)],&dbuf[0],bmax*sizeof(double));
			free(dbuf);
		}

		if(dmin>dmax){
fprintf(stderr,"ivs_associate_as_surf : over dist :%.4f\n",dmin);
			break;
		}
		if(ains==vsa->nv-1){
fprintf(stderr,"ivs_associate_as_surf : a in b\n");
			break;
		}
		if(bins==vsb->nv-1){
fprintf(stderr,"ivs_associate_as_surf : b in a\n");
			break;
		}
// recherche des first/last
		for(i=0;i<vsa->nv-1;i++){
			if(da[i]<=dmax){
				if(af==-1){
					af=al=i;
				}
				else if(i>al){
					al=i;
				}
			}
		}
		for(i=0;i<vsb->nv-1;i++){
			if(db[i]<=dmax){
				if(bf==-1){
					bf=bl=i;
				}
				else if(i>bl){
					bl=i;
				}
			}
		}
		if((af==al)||(bf==bl)){
fprintf(stderr,"ivs_associate_as_surf : only one\n");
			break;
		}
fprintf(stderr,"af=%d, al=%d, bf=%d, bl=%d\n",af,al,bf,bl);

		ivs_split(vsa,af,&splits[0],&buf);
		if(!buf){
fprintf(stderr,"ivs_associate_as_surf : splits[0]==NULL\n");			
			ivs_split(vsa,al,&splits[1],&splits[2]);
		}
		else{
			ivs_split(buf,al-af,&splits[1],&splits[2]);
			ivs_free(buf);
		}
		
		ivs_split(vsb,bf,&splits[3],&buf);
		if(!buf){
fprintf(stderr,"ivs_associate_as_surf : splits[3]==NULL\n");			
			ivs_split(vsb,bl,&splits[4],&splits[5]);
		}
		else{
			ivs_split(buf,bl-bf,&splits[4],&splits[5]);
			ivs_free(buf);
		}
		
// cas tordus ?
		
		ivs2ivs(splits[0],&vsr);
		(void)ivx_proj(&vsr->vx.vx2[vsr->nv-1],splits[5],&vr,&i,&fmax);
		if((fmax>0)&&(fmax<1)){
			splits[5]->vx.vx2[0]=vr;
		}
		vsr=ivs_concat(vsr,splits[5]);
		vsr=ivs_concat(vsr,splits[3]);
		(void)ivx_proj(&vsr->vx.vx2[vsr->nv-1],splits[2],&vr,&i,&fmax);
		if((fmax>0)&&(fmax<1)){
			splits[2]->vx.vx2[0]=vr;
		}
		vsr=ivs_concat(vsr,splits[2]);
		
		break;
	}
	if(vsa){
		ivs_free(vsa);
	}
	if(vsb){
		ivs_free(vsb);
	}
	if(da){
		free(da);
	}
	if(db){
		free(db);
	}
	for(i=0;i<6;i++){
//fprintf(stderr,"%x\n",splits[i]);	
		if(splits[i]){
/*			for(j=0;j<splits[i]->nv;j++){
fprintf(stderr,"%d;%d\n",splits[i]->vx.vx2[j].h,splits[i]->vx.vx2[j].v);	
			}*/
			ivs_free(splits[i]);
		}
	}
//fprintf(stderr,"ivs_associate_as_surf::end\n");
//fflush(stderr);
	return(vsr);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_intersect_as_line(ivertices* vsa, ivertices* vsb){
GEOSGeom ggma=vxs2geosxlin(vsa);
	if(!ggma){
		return(NULL);
	}
GEOSGeom ggmb=vxs2geosxlin(vsb);
	if(!ggmb){
		GEOSGeom_destroy(ggma);
		return(NULL);
	}
GEOSGeom ggmr=GEOSIntersection(ggma,ggmb);
	GEOSGeom_destroy(ggma);
	GEOSGeom_destroy(ggmb);
	if(!ggmr){
		return(NULL);
	}
ivertices*	vxs;
	switch(GEOSGeomTypeId(ggmr)){
		case GEOS_LINESTRING:
		case GEOS_LINEARRING:
		case GEOS_MULTILINESTRING:
			vxs=geosxlin2vxs(ggmr);
			break;
		case GEOS_POINT:
		case GEOS_MULTIPOINT:
			vxs=geosxpoint2vxs(ggmr);
			break;
		case GEOS_GEOMETRYCOLLECTION:
			vxs=geoscollect2vxs(ggmr);
			break;
		default:
			break;
	}
	GEOSGeom_destroy(ggmr);
	return(vxs);
}


// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_intersect_as_surf(ivertices* vsa, ivertices* vsb){
GEOSGeom ggma=vxs2geosxgon(vsa);
	if(!ggma){
		return(NULL);
	}
GEOSGeom ggmb=vxs2geosxgon(vsb);
	if(!ggmb){
		GEOSGeom_destroy(ggma);
		return(NULL);
	}
GEOSGeom ggmr=GEOSIntersection(ggma,ggmb);
	GEOSGeom_destroy(ggma);
	GEOSGeom_destroy(ggmb);
	if(!ggmr){
		return(NULL);
	}
ivertices*	vxs=geosxgon2vxs(ggmr);	
	GEOSGeom_destroy(ggmr);
	return(vxs);
}

// ---------------------------------------------------------------------------
// PB : Erreur si on a déjà une enveloppe convexe
// -----------
ivertices* ivs_convexify(i2dvertex*	vx, int n){
int			i;
ivertices*	rvx=ivs_new(_2D_VX,n,0);
i2dvertex	a;

	rvx->nv=n;
	memmove(rvx->vx.vx2,vx,n*sizeof(i2dvertex));
	a=vx[0];
	for(i=1;i<n;i++){
		if(vx[i].v<a.v){
			a=vx[i];
		}
	}
	vx_theta_qsort(&a,rvx->vx.vx2,n);
	for(i=n-1;i>0;i--){
		if(eq_ivx2(&rvx->vx.vx2[i],&rvx->vx.vx2[i-1])){
			rvx=ivx_remove(rvx,i);
			if(rvx==NULL){
fprintf(stderr,"ivs_convexify:rvx==NULL for %d at remove (pass 1)\n",i);
				return(NULL);
			}
		}
	}
	a=rvx->vx.vx2[0];
	rvx=ivx2_add(rvx,&a);
	for(i=rvx->nv-3;i>-1;i--){
		while((rvx->nv>3)&&vx_siam(&rvx->vx.vx2[i+1],&rvx->vx.vx2[i],&rvx->vx.vx2[i+2])>=0){
			rvx=ivx_remove(rvx,i+1);
			if(rvx==NULL){
fprintf(stderr,"ivs_convexify:rvx==NULL for %d at remove  (pass 2)\n",i);
				return(NULL);
			}
		}
	}
	if(rvx->nv<3){
		ivs_free(rvx);
		rvx=ivs_new(_2D_VX,n,0);
		memmove(rvx->vx.vx2,vx,n*sizeof(i2dvertex));
	}
	return(rvx);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_polygonize(i2dvertex* vx, int n){
int			i;
ivertices*	rvx=ivs_new(_2D_VX,n,0);
i2dvertex	a;

	rvx->nv=n;
	memmove(rvx->vx.vx2,vx,n*sizeof(i2dvertex));
	a=vx[0];
	for(i=1;i<n;i++){
		if(vx[i].v<a.v){
			a=vx[i];
		}
	}
	vx_theta_qsort(&a,rvx->vx.vx2,n);
	for(i=n-1;i>0;i--){
		if(eq_ivx2(&rvx->vx.vx2[i],&rvx->vx.vx2[i-1])){
			rvx=ivx_remove(rvx,i);
			if(rvx==NULL){
fprintf(stderr,"ivs_polygonize:rvx==NULL for %d at remove (pass 1)\n",i);
				return(NULL);
			}
		}
	}
	a=rvx->vx.vx2[0];
	rvx=ivx2_add(rvx,&a);
	return(rvx);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_split_surf(ivertices* gvxs, 
					ivertices* glin,
					ivertices** ra,
					ivertices** rb){

int			idx,idx1,idx2,clkw,i;
double		a,ar1,ar2,ar3,al1,al2;
ivertices*	bvxs;
ivertices*	vxs;
ivertices*	lin;
i2dvertex	vr;

	*ra=NULL;
	*rb=NULL;

// clone
	if(ivs2ivs(gvxs,&vxs)){
		return;
	}
	if(ivs2ivs(glin,&lin)){
		ivs_free(vxs);
		return;
	}
// inserts
	for(i=0;i<lin->nv;i++){
		if(	(ivx2ivs_dist(&lin->vx.vx2[i],vxs)<=__FUZZY_DIST__)	||
			(!ivx_in_ivs(&lin->vx.vx2[i],vxs,0))				){
			
			(void)ivx_proj(&lin->vx.vx2[i],vxs,&vr,&idx,&a);
			if((a>0)&&(a<1)){
				vxs=ivx2_insert(vxs,&vr,idx);
			}
			
			lin->vx.vx2[i]=vr;
		}		
	}
// computes	
	idx=ivx2_exists(vxs,&lin->vx.vx2[0]);
	if(idx==0){
		ivs_free(lin);
		ivs_free(vxs);
		return;
	}
	ar1=vx_deg_90(&lin->vx.vx2[0],&lin->vx.vx2[lin->nv-1]);
	clkw=ivx2_clockwise(vxs->vx.vx2,vxs->nv);
	ar2=(clkw==1)?
		vx_deg_90(&vxs->vx.vx2[idx-1],&vxs->vx.vx2[idx]):
		vx_deg_90(&vxs->vx.vx2[idx],&vxs->vx.vx2[idx-1]);
	if(idx==1){
		ar3=(clkw==1)?
			vx_deg_90(&vxs->vx.vx2[idx-1],&vxs->vx.vx2[vxs->nv-1]):
			vx_deg_90(&vxs->vx.vx2[vxs->nv-1],&vxs->vx.vx2[idx-1]);
	}
	else{
		ar3=(clkw==1)?
			vx_deg_90(&vxs->vx.vx2[idx-1],&vxs->vx.vx2[idx-2]):
			vx_deg_90(&vxs->vx.vx2[idx-2],&vxs->vx.vx2[idx-1]);
	}
	if(clkw==1){
		al2=ar3-ar2;
	}
	else{
		al2=360.0;// -al2 :al2 n'est pas initialisé. Quelle est la bonne valeur ?
	}
	if(al2<0){
		al2+=360.0;
	}	
	a=ar1-ar2;
	if(a<0){
		a+=360.0;
	}
	al1=0;
	if((a<al1)||(a>al2)){
		ivs_free(lin);
		ivs_free(vxs);
		return;
	}
	idx1=ivx2_exists(vxs,&lin->vx.vx2[0]);
	idx2=ivx2_exists(vxs,&lin->vx.vx2[lin->nv-1]);
	
// first surf
	*ra=ivs_new(_2D_VX,0,0);
	idx=(idx1>idx2)?idx2:idx1;
	for(i=0;i<idx;i++){
		if(!(bvxs=ivx2_add(*ra,&vxs->vx.vx2[i]))){
			break;
		}
		*ra=bvxs;
	}
	if(eq_ivx2(&((*ra)->vx.vx2[(*ra)->nv-1]),&lin->vx.vx2[0])){
		for(i=1;i<lin->nv-1;i++){
			if(!(bvxs=ivx2_add(*ra,&lin->vx.vx2[i]))){
				break;
			}
			*ra=bvxs;				
		}
	}
	else{
		for(i=lin->nv-2;i>0;i--){
			if(!(bvxs=ivx2_add(*ra,&lin->vx.vx2[i]))){
				break;
			}
			*ra=bvxs;				
		}
	}
	idx=(idx1>idx2)?idx1:idx2;
	for(i=idx-1;i<vxs->nv;i++){
		if(!(bvxs=ivx2_add(*ra,&vxs->vx.vx2[i]))){
			break;
		}
		*ra=bvxs;
	}
	
// second surf
	*rb=ivs_new(_2D_VX,0,0);
	if(idx1>idx2){
		idx=idx1;
		idx1=idx2;
		idx2=idx;
	}
	for(i=idx1-1;i<idx2;i++){
		if(!(bvxs=ivx2_add(*rb,&vxs->vx.vx2[i]))){
			break;
		}
		*rb=bvxs;
	}
	if(eq_ivx2(&((*rb)->vx.vx2[(*rb)->nv-1]),&lin->vx.vx2[0])){
		for(i=1;i<lin->nv-1;i++){
			if(!(bvxs=ivx2_add(*rb,&lin->vx.vx2[i]))){
				break;
			}
			*rb=bvxs;
		}
	}
	else{
		for(i=lin->nv-2;i>0;i--){
			if(!(bvxs=ivx2_add(*rb,&lin->vx.vx2[i]))){
				break;
			}
			*rb=bvxs;				
		}
	}
	if((bvxs=ivx2_add(*rb,&(*rb)->vx.vx2[0]))){
		*rb=bvxs;				
	}
	ivs_free(lin);
	ivs_free(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_bufferize_surf(ivertices* vsa, double d){
GEOSGeom ggma=vxs2geosxgon(vsa);
	if(!ggma){
		return(NULL);
	}
GEOSGeom ggmr=GEOSBuffer(ggma,d,4);
	GEOSGeom_destroy(ggma);
	if(!ggmr){
		return(NULL);
	}
ivertices*	vxs=geosxgon2vxs(ggmr);	
	GEOSGeom_destroy(ggmr);
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_bufferize_line(ivertices* vsa, double d){
GEOSGeom ggma=vxs2geosxlin(vsa);
	if(!ggma){
		return(NULL);
	}
GEOSGeom ggmr=GEOSBuffer(ggma,d,4);
	GEOSGeom_destroy(ggma);
	if(!ggmr){
		return(NULL);
	}
ivertices*	vxs=geosxgon2vxs(ggmr);	
	GEOSGeom_destroy(ggmr);
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_bufferize_point(ivertices* vsa, double d){
GEOSGeom ggma=vxs2geosxpoint(vsa);
	if(!ggma){
		return(NULL);
	}
GEOSGeom ggmr=GEOSBuffer(ggma,d,4);
	GEOSGeom_destroy(ggma);
	if(!ggmr){
		return(NULL);
	}
ivertices*	vxs=geosxgon2vxs(ggmr);	
	GEOSGeom_destroy(ggmr);
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_split_surf_with_line(ivertices* gon, ivertices* lin){
GEOSGeom ggma=vxs2geosxgon(gon);
	if(!ggma){
fprintf(stderr,"vxs2geosxgon returns NULL\n");
		return(NULL);
	}
GEOSGeom ggmb=vxs2geosxlin(lin);
	if(!ggmb){
		GEOSGeom_destroy(ggma);
fprintf(stderr,"vxs2geosxlin returns NULL\n");
		return(NULL);
	}
GEOSGeom ggmr=GEOSSymDifference(ggma,ggmb);
	GEOSGeom_destroy(ggma);
	GEOSGeom_destroy(ggmb);
	if(!ggmr){
fprintf(stderr,"GEOSSymDifference returns NULL\n");
		return(NULL);
	}
fprintf(stderr,"Geometry type=%d\n",GEOSGeomTypeId(ggmr));
ivertices*	vxs=geoscollect2vxs(ggmr);	
	GEOSGeom_destroy(ggmr);
	return(vxs);
}

// ---------------------------------------------------------------------------
//
// -----------
int ivs_build_point_offsets(ivertices* vxs){
int*    buf=(int*)malloc(vxs->nv*sizeof(int));
    if(!buf){
        return 0;
    }
    if(vxs->offs){
        free(vxs->offs);
        vxs->offs=NULL;
    }
    vxs->no=vxs->nv;
    vxs->offs=buf;
    for(int i=0;i<vxs->no;i++){
        vxs->offs[i]=i;
    }
    return 1;
}
