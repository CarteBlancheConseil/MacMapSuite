//----------------------------------------------------------------------------
// File : vx_manip.c
// Project : MacMapSuite
// Purpose : C source file : Geometry modification utils
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
// 16/08/2005 creation.
//----------------------------------------------------------------------------

#include "vx_manip.h"
#include "vx_allocation.h"
#include "vx_measure.h"
#include "vx_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// ---------------------------------------------------------------------------
// 
// -----------
void	ivx_shift				(	i2dvertex** vxs, 
									int* n, 
									double d);
void	ivx_simplify_with_angle	(	i2dvertex** vx, 
									int* n, 
									double a);
void	ivx_cut_miter_with_angle(	i2dvertex** vx, 
									int* n, 
									double a);

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_rotate_anchor(ivertices* vxs, double rad, i2dvertex* o){
double		sina=sin(rad);
double		cosa=cos(rad);
double		cx=(double)o->h*(1.0-cosa)+(double)o->v*sina;
double		cy=-(double)o->h*sina+(double)o->v*(1.0-cosa);
i2dvertex	tmp;
int			i;

	for(i=0;i<vxs->nv;i++){
		tmp.h=round(((double)vxs->vx.vx2[i].h)*cosa-((double)vxs->vx.vx2[i].v)*sina+cx);
		tmp.v=round(((double)vxs->vx.vx2[i].h)*sina+((double)vxs->vx.vx2[i].v)*cosa+cy);
		vxs->vx.vx2[i]=tmp;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_rotate(ivertices* vxs, double rad){
ivx_rect	vr;
	ivs_bounds(vxs,&vr);
i2dvertex	vx;
	ivr_mid(&vr,&vx);
	ivs_rotate_anchor(vxs,rad,&vx);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_scale_anchor(ivertices* vxs, double cx, double cy, i2dvertex* o){
int	i;
	for(i=0;i<vxs->nv;i++){
		vxs->vx.vx2[i].h=round(((vxs->vx.vx2[i].h-o->h)*cx)+o->h);
		vxs->vx.vx2[i].v=round(((vxs->vx.vx2[i].v-o->v)*cy)+o->v);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_scale(ivertices* vxs, double cx, double cy){
ivx_rect	vr;
	ivs_bounds(vxs,&vr);
i2dvertex	vx;
	ivr_mid(&vr,&vx);
	ivs_scale_anchor(vxs,cx,cy,&vx);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_move(ivertices* vxs, int dx, int dy){
	for(long i=0;i<vxs->nv;i++){
		vxs->vx.vx2[i].h+=dx;
		vxs->vx.vx2[i].v+=dy;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void dvs_move(dvertices* vxs, double dx, double dy){
	for(long i=0;i<vxs->nv;i++){
		vxs->vx.vx2[i].x+=dx;
		vxs->vx.vx2[i].y+=dy;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_shift(ivertices** vxs, double dst){
int			i,nv,sz,o=0;
i2dvertex*	p;
ivertices*	vxsr=ivs_new(_2D_VX,0,(*vxs)->no);
ivertices*	tmp;

	for(i=0;i<ivs_n_parts(*vxs);i++){
		p=ivs2_part(*vxs,i,&nv);
		ivx_shift(&p,&nv,dst);
		if(!p){
            ivs_free(vxsr);
			return;
		}
		sz=sizeof(ivertices)-sizeof(i3dvertex)+(vxsr->nv+nv)*sizeof(i2dvertex);
		tmp=realloc(vxsr,sz);
		if(!tmp){
            ivs_free(vxsr);
            free(p);
			return;
		}
        vxsr=tmp;
		memmove(&vxsr->vx.vx2[vxsr->nv],p,nv*sizeof(i2dvertex));
		vxsr->nv+=nv;
		if(vxsr->offs){
			vxsr->offs[i]=o;
		}
		o+=nv;
		free(p);
	}
	ivs_free(*vxs);
	*vxs=vxsr;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_line_pos(ivertices* vxs, i2dvertex* vx, double coef){
int			i,j,nv,x=0;
double		l=ivs_length(vxs);
double		midl=l*coef;
double		tl=0;
i2dvertex*	p=NULL;

	l=0;
	for(i=0;i<ivs_n_parts(vxs);i++){
		p=ivs2_part(vxs,i,&nv);
		for(j=0;j<nv-1;j++){
			l=ivx2_dist(&p[j],&p[j+1]);
			tl+=l;
			if(tl>midl){
                x=j;
				break;
			}
		}
		if(tl>midl){
			tl-=l;
			break;
		}
	}
    if(p==NULL){
        return;
    }
	if(l!=0){
		vx->h=p[x].h+round(((midl-tl)/l)*(double)(p[x+1].h-p[x].h));
		vx->v=p[x].v+round(((midl-tl)/l)*(double)(p[x+1].v-p[x].v));
	}
	else{
		vx->h=p[x].h;
		vx->v=p[x].v;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_centro(ivertices* vxs, i2dvertex* vx){
ivx_rect	vr;

	ivs_bounds(vxs,&vr);
	ivr_mid(&vr,vx);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_centro_line(ivertices* vxs, i2dvertex* vx){
int			i,j,nv,x=0;
double		l=ivs_length(vxs);
double		midl=l/2.0;
double		tl=0;
i2dvertex*	p=NULL;

	l=0;
	for(i=0;i<ivs_n_parts(vxs);i++){
		p=ivs2_part(vxs,i,&nv);
		for(j=0;j<nv-1;j++){
			l=ivx2_dist(&p[j],&p[j+1]);
			tl+=l;
			if(tl>midl){
                x=j;
				break;
			}
		}
		if(tl>midl){
			tl-=l;
			break;
		}
	}
    if(p==NULL){
        return;
    }
	if(l!=0){
		vx->h=p[x].h+round(((midl-tl)/l)*(double)(p[x+1].h-p[x].h));
		vx->v=p[x].v+round(((midl-tl)/l)*(double)(p[x+1].v-p[x].v));
	}
	else{
		vx->h=p[x].h;
		vx->v=p[x].v;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_centro_surf(ivertices* vxs, i2dvertex* vx){
int			i,nv=0,np;
ivx_rect	vr;
i2dvertex*	p=NULL;

	ivs_bounds(vxs,&vr);
	ivr_mid(&vr,vx);
	if(ivx_in_ivs(vx,vxs,1)){
		return;
	}
	
	np=ivs_n_parts(vxs);
	if(np==1){
		p=ivs2_part(vxs,0,&nv);
	}
	else{
		for(i=0;i<np;i++){
			p=ivs2_part(vxs,i,&nv);
			if(ivx2_clockwise(p,nv)==1){
				break;
			}
		}
	}
	vx->h=0;
	vx->v=0;
    if(p==NULL){
        return;
    }
	for(i=1;i<nv;i++){
		vx->h+=(p[i].h-p[0].h);
		vx->v+=(p[i].v-p[0].v);
	}
	vx->h=p[0].h+round(((double)vx->h)/((double)nv-1.0));
	vx->v=p[0].v+round(((double)vx->v)/((double)nv-1.0));
}

// ---------------------------------------------------------------------------
// VERIF 22/01/10 OK
// -----------
void ivs_reverse(ivertices* vxs){
int			i,j;
int			nv,np;
i2dvertex	vx;
i2dvertex*	p;

	np=ivs_n_parts(vxs);
	for(i=0;i<np;i++){
		p=ivs2_part(vxs,i,&nv);
		for(j=0;j<nv/2;j++){
			vx=p[j];
			p[j]=p[(nv-1)-j];
			p[(nv-1)-j]=vx;
		}
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void ivs_simplify_with_angle(ivertices** vxs, double a){
int			i,nv,sz,o=0;
i2dvertex*	p;
ivertices*	vxsr=ivs_new(_2D_VX,0,(*vxs)->no);
ivertices*	tmp;
    
	for(i=0;i<ivs_n_parts(*vxs);i++){
		p=ivs2_part(*vxs,i,&nv);
		ivx_simplify_with_angle(&p,&nv,a);
		if(!p){
            ivs_free(vxsr);
			return;
		}
		sz=sizeof(ivertices)-sizeof(i3dvertex)+(vxsr->nv+nv)*sizeof(i2dvertex);
		tmp=realloc(vxsr,sz);
		if(!tmp){
            ivs_free(vxsr);
            free(p);
			return;
		}
        vxsr=tmp;
		memmove(&vxsr->vx.vx2[vxsr->nv],p,nv*sizeof(i2dvertex));
		vxsr->nv+=nv;
		if(vxsr->offs){
			vxsr->offs[i]=o;
		}
		o+=nv;
		free(p);
	}
	ivs_free(*vxs);
	*vxs=vxsr;
}

// ---------------------------------------------------------------------------
// 
// ------------
void ivs_cut_miter_with_angle(ivertices** vxs, double a){
int			i,nv,sz,o=0;
i2dvertex*	p;
ivertices*	vxsr=ivs_new(_2D_VX,0,(*vxs)->no);
ivertices*	tmp;

	for(i=0;i<ivs_n_parts(*vxs);i++){
		p=ivs2_part(*vxs,i,&nv);
		ivx_cut_miter_with_angle(&p,&nv,a);
		if(!p){
            ivs_free(vxsr);
			return;
		}
		sz=sizeof(ivertices)-sizeof(i3dvertex)+(vxsr->nv+nv)*sizeof(i2dvertex);
		tmp=realloc(vxsr,sz);
		if(!vxsr){
            ivs_free(vxsr);
            free(p);
			return;
		}
        vxsr=tmp;
		memmove(&vxsr->vx.vx2[vxsr->nv],p,nv*sizeof(i2dvertex));
		vxsr->nv+=nv;
		if(vxsr->offs){
			vxsr->offs[i]=o;
		}
		o+=nv;
		free(p);
	}
	ivs_free(*vxs);
	*vxs=vxsr;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivx_shift(i2dvertex** vx, int* n, double ds){
int			i,ln=*n-1;
int			cur=0;
int			ring=ivx2_closed(*vx,*n);
double		a,a1,a2,r,d;
i2dvertex	pt,pt1,pt2,p;
i2dvertex*	res;
i2dvertex*	tmp;
    
	res=(i2dvertex*)malloc(((ln+1)*2L)*sizeof(i2dvertex));
	if(!res){
		return;
	}
	p=(*vx)[0];
	for(i=0;i<=ln;i++){
		pt=(*vx)[i];
		if(i!=0){
			pt1=(*vx)[i-1];
		}
		else if(ring){
			pt1=(*vx)[ln-1];
		}
		if(i!=ln){
			pt2=(*vx)[i+1];
		}
		else if(ring){
			pt2=(*vx)[1];
		}
		if((i==0)&&!ring){
			d=ds;
			a=hv_vx_angle(&pt,&pt2)-M_PI/2.0;
			if(a>2.0*M_PI){
				a-=2.0*M_PI;
			}
		}
		else if((i==ln)&&!ring){
			d=ds;
			a=hv_vx_angle(&pt1,&pt)-M_PI/2.0;
			if(a>2.0*M_PI){
				a-=2.0*M_PI;
			}
		}
		else{
			a1=hv_vx_angle(&pt,&pt1);
			a2=hv_vx_angle(&pt,&pt2);
			if(fabs(a1-a2)<0.1){
				r=M_PI/4.0;
				d=ds/fabs(sin(r));
				ivx_proj_at_dist(&pt,&p,d,a2+5.0*M_PI/4.0);
				//res[cur]=p;
				//cur++;
				a=a2+3.0*M_PI/4.0;
			}
			else{
				r=(a2-a1)/2.0;
				d=ds/fabs(sin(r));
				if(a2<a1){
					a1+=(2.0*M_PI);
				}
				a=(a1+a2)/2.0;
			}
		}
		ivx_proj_at_dist(&pt,&p,d,a);
		res[cur]=p;
		cur++;
	}
    if(cur==0){
        free(res);
        return;
    }
	*n=cur;
	tmp=(i2dvertex*)realloc(res,cur*sizeof(i2dvertex));
	if(!tmp){
        free(res);
		return;
	}
	*n=cur;
	*vx=tmp;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivx_simplify_with_angle(i2dvertex** vx, int* n, double a){
int			i,j,cn=1,cur,ok;
double		a1,a2,fa;
i2dvertex*	res=(i2dvertex*)malloc(sizeof(i2dvertex));
i2dvertex*	tmp;
    
	res[0]=(*vx)[0];
	for(i=0;i<(*n)-2;i++){
		cur=i+1;
		ok=0;
		for(j=i+1;j<(*n)-1;j++){
			a1=xy_vx_angle(&(*vx)[j],&(*vx)[i]);
			a2=xy_vx_angle(&(*vx)[j],&(*vx)[j+1]);
			fa=a2-a1;
			if(fa<0){
				fa+=(2.0*M_PI);
			}
			fa-=M_PI;
			fa=fabs(fa);
			if(fa>a){
				cur=j;
				ok=1;
				break;			
			}
		}
		if(ok){
			cn++;
			tmp=(i2dvertex*)realloc(res,cn*sizeof(i2dvertex));
			if(!tmp){
                free(res);
				return;
			}
            res=tmp;
			res[cn-1]=(*vx)[j];
		}
		i=cur-1;
	}
	
	if(!eq_ivx2(&(*vx)[(*n)-1],&res[cn-1])){
		cn++;
		res=(i2dvertex*)realloc(res,cn*sizeof(i2dvertex));
		if(!res){
			return;
		}
		res[cn-1]=(*vx)[(*n)-1];
	}
	*vx=res;
	*n=cn;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivx_cut_miter_with_angle(i2dvertex** vx, int* n, double a){
int			i,j,cn=1,cur,ok;
double		a1,a2,fa;
i2dvertex*	res=(i2dvertex*)malloc(sizeof(i2dvertex));
i2dvertex*	tmp;

	res[0]=(*vx)[0];
	for(i=0;i<(*n)-2;i++){
		cur=i+1;
		ok=0;
		for(j=i+1;j<(*n)-1;j++){
			a1=xy_vx_angle(&(*vx)[j],&(*vx)[i]);
			a2=xy_vx_angle(&(*vx)[j],&(*vx)[j+1]);
			fa=a2-a1;
			if(fa<0){
				fa+=(2.0*M_PI);
			}
			if(fa>M_PI){
				fa=(2.0*M_PI)-fa;
			}
			fa=fabs(fa);
			if(fa>a){
				cur=j;
				ok=1;
				break;			
			}
		}
		if(ok){
			cn++;
			tmp=(i2dvertex*)realloc(res,cn*sizeof(i2dvertex));
            if(!tmp){
                free(res);
                return;
            }
            res=tmp;
			res[cn-1]=(*vx)[j];
		}
		i=cur-1;
	}
	
	if(!eq_ivx2(&(*vx)[(*n)-1],&res[cn-1])){
		cn++;
		res=(i2dvertex*)realloc(res,cn*sizeof(i2dvertex));
		if(!res){
			return;
		}
		res[cn-1]=(*vx)[(*n)-1];
	}
	*vx=res;
	*n=cn;
}

#pragma mark -
#pragma mark OLD STUFF
#pragma mark -

// ---------------------------------------------------------------------------
// VERSION SANS OFFSETS
// -----------
/*void ivs_shift(ivertices** vxs, double dst){
int			i,sz;
int			nv=(*vxs)->nv-1;
int			closed=ivx2_closed((*vxs)->vx.vx2,(*vxs)->nv);
int			cur;
double		a,a1,a2,d,r;
i2dvertex	c,c1,c2,c3;
ivertices*	vxsr=ivs_new(_2D_VX,(nv+1L)*2L,0);

	cur=0;
	for(i=0;i<=nv;i++){
		c=(*vxs)->vx.vx2[i];
		
		if(i!=0){
			c1=(*vxs)->vx.vx2[i-1];
		}
		else if(closed){
			c1=(*vxs)->vx.vx2[nv-1];
		}
		
		if(i!=nv){
			c2=(*vxs)->vx.vx2[i+1];
		}
		else if(closed){
			c2=(*vxs)->vx.vx2[1];
		}
		
		if((i==0)&&(!closed)){
			d=dst;
			a=hv_vx_angle(&c,&c2)-M_PI/2.0;
			if(a>2.0*M_PI){
				a-=2.0*M_PI;
			}
		}
		else if((i==nv)&&(!closed)){
			d=dst;
			a=hv_vx_angle(&c1,&c)-M_PI/2.0;
			if(a>2.0*M_PI){
				a-=2.0*M_PI;
			}
		}
		else{
			a1=hv_vx_angle(&c,&c1);
			a2=hv_vx_angle(&c,&c2);
			if(fabs(a1-a2)<0.1){
				r=M_PI/4.0;
				d=dst/fabs(sin(r));
				ivx_proj_at_dist(&c,&c3,d,a2+5.0*M_PI/4.0);
				vxsr->vx.vx2[cur]=c3;
				cur++;
				a=a2+3.0*M_PI/4.0;
			}
			else{
				r=(a2-a1)/2.0;
				d=dst/fabs(sin(r));
				if(a2<a1){
					a1+=2.0*M_PI;
				}
				a=(a1+a2)/2.0;
			}
		}
		ivx_proj_at_dist(&c,&c3,d,a);
		vxsr->vx.vx2[cur]=c3;
		cur++;
	}
	vxsr->nv=cur;
//fprintf(stderr,"%d\n",vxsr->nv);

	sz=sizeof(ivertices)-sizeof(i3dvertex)+(vxsr->nv)*sizeof(i2dvertex);
	vxsr=realloc(vxsr,sz);
	
	if(!vxsr){
fprintf(stderr,"ivs_shift::realloc(%d) failed\n",sz);
		return;
	}
	ivs_free(*vxs);
	*vxs=vxsr;
}*/

// ---------------------------------------------------------------------------
// VERSION SANS OFFSETS
// ------------
/*ivertices* ivs_simplify_with_angle(ivertices* vxs, double a){
int			i,j;
ivertices*	res=ivs_new(_2D_VX,1,0);
double		a1,a2;

	res->vx.vx2[0]=vxs->vx.vx2[0];
	for(i=0;i<vxs->nv-2;i++){
		for(j=i+1;j<vxs->nv-2;j++){
			a1=rad2deg(xy_vx_angle(&vxs->vx.vx2[i],&vxs->vx.vx2[j]));
			a2=rad2deg(xy_vx_angle(&vxs->vx.vx2[j],&vxs->vx.vx2[j+1]));			
			if(fabs(a2-a1)>a){
				i=j;
				break;
			}
		}
		res=ivx2_add(res,&vxs->vx.vx2[i+1]);
	}
	res=ivx2_add(res,&vxs->vx.vx2[vxs->nv-1]);
	ivs_free(vxs);
	return(res);
}*/
