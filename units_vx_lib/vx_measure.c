//----------------------------------------------------------------------------
// File : vx_measure.c
// Project : MacMapSuite
// Purpose : C source file : Geometry measurement utils
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

#include "vx_measure.h"
#include "vx_utils.h"
#include "vx_raw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx2_surf(i2dvertex* vx, int n){
int		i;
double	s=0,X0,Y0,X1,X2,Y1,Y2;
	
	s=0;
	if(n>2){
		X1=vx[0].h;
		X0=X1;
		Y1=vx[0].v;
		Y0=Y1;
		for(i=0;i<n;i++){
			Y2=vx[i].v;
			X2=vx[i].h;
			s=s+(Y1+Y2)*(X2-X1);
			Y1=Y2;
			X1=X2;
		}
		if(X1!=X0){
			s=s+(Y1+Y0)*(X0-X1);
		}
	}
	return(-s/2.0);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx3_surf(i3dvertex* vx, int n){
int		i;
double	s=0,X0,Y0,X1,X2,Y1,Y2;
	
	s=0;
	if(n>2){
		X1=vx[0].h;
		X0=X1;
		Y1=vx[0].v;
		Y0=Y1;
		for(i=0;i<n;i++){
			Y2=vx[i].v;
			X2=vx[i].h;
			s=s+(Y1+Y2)*(X2-X1);
			Y1=Y2;
			X1=X2;
		}
		if(X1!=X0){
			s=s+(Y1+Y0)*(X0-X1);
		}
	}
	return(-s/2.0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivx2_clockwise(i2dvertex* vx, int n){
double	s=ivx2_surf(vx,n);
	if(s>0){
		return(1);
	}
	if(s<0){
		return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivx3_clockwise(i3dvertex* vx, int n){
double	s=ivx3_surf(vx,n);
	if(s>0){
		return(1);
	}
	if(s<0){
		return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivx2_in_surf(i2dvertex* vx, i2dvertex* vs, int n, int limit){
int			i,first,last,oq,q;
double		oh,ov;
i2dvertex	vp;
	
	if(n<3){
		return(0);
	}
	vp.h=vs[0].h-vx->h;
	vp.v=vs[0].v-vx->v;
	if((vp.h==0)&&(vp.v==0)){
		return(limit);
	}
	first=vx_quad(vp.h,-vp.v);
	last=first;
	oq=first;
	oh=vp.h;
	ov=vp.v;
	for(i=1;i<n;i++){
		vp.h=vp.h+(vs[i].h-vs[i-1].h);
		vp.v=vp.v+(vs[i].v-vs[i-1].v);
		if((vp.h==0)&&(vp.v==0)){
			return(limit);
		}
		q=vx_quad(vp.h,-vp.v);
		if(vx_cquad(oq,q,&last,&oh,&ov,vp.h,vp.v)){
			return(limit);
		}
		oq=q;
		oh=vp.h;
		ov=vp.v;
	}
	return((labs(last-first)/4)>0);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivs_length(ivertices* vs){
int		i,j,np;
double	l=0;

	if(vs->sign==_2D_VX){
i2dvertex*	vx;
		for(i=0;i<ivs_n_parts(vs);i++){
			vx=ivs2_part(vs,i,&np);
			for(j=1;j<np;j++){
				l+=ivx2_dist(&vx[j],&vx[j-1]);
			}
		}
	}
	else{
i3dvertex*	vx;
		for(i=0;i<ivs_n_parts(vs);i++){
			vx=ivs3_part(vs,i,&np);
			for(j=1;j<np;j++){
				l+=ivx3_dist(&vx[j],&vx[j-1]);
			}
		}
	}
	return(l);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx2_length(i2dvertex*	vs,
					int nv){
int		j;
double	l=0;

	for(j=1;j<nv;j++){
		l+=ivx2_dist(&vs[j],&vs[j-1]);
	}
	return(l);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivs_surf(ivertices* vs){
int		i,n,np;
double	s=0;
	
	n=ivs_n_parts(vs);
	if(vs->sign==_2D_VX){
i2dvertex*	vx;
		for(i=0;i<n;i++){
			vx=ivs2_part(vs,i,&np);
			s+=ivx2_surf(vx,np);
		}
	}
	else{
i3dvertex*	vx;
		for(i=0;i<n;i++){
			vx=ivs3_part(vs,i,&np);
			s+=ivx3_surf(vx,np);
		}
	}
	if(s<0){
		s=-s;
	}
	return(s);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivx_in_ivs(i2dvertex* vx, ivertices* vs, int limit){
int		i,np,nv,inh=0,ins=0;
	
	np=ivs_n_parts(vs);
	if(vs->sign==_2D_VX){
i2dvertex*	p;
		if(np==1){
			p=ivs2_part(vs,0,&nv);
			return(ivx2_in_surf(vx,p,nv,limit));
		}
		else{
			for(i=0;i<np;i++){
				p=ivs2_part(vs,i,&nv);
				if(ivx2_in_surf(vx,p,nv,limit)){
					switch(ivx2_clockwise(p,nv)){
						case -1:
							inh++;
							break;
						case 0:
							break;
						case 1:
							ins++;
							break;
					}
				}
				//if((ivx2_surf(p,nv)>0)&&(ivx2_in_surf(vx,p,nv,limit))){
				//	return(1);
				//}
			}
			if(ins>inh){
				return(1);
			}
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx2_dist(i2dvertex* vxa, i2dvertex* vxb){
double	dh,dv;
	
	dh=vxb->h-vxa->h;
	dv=vxb->v-vxa->v;
	return(sqrt(dh*dh+dv*dv));
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx3_dist(i3dvertex* vxa, i3dvertex* vxb){
double	dh,dv;
	
	dh=vxb->h-vxa->h;
	dv=vxb->v-vxa->v;
	return(sqrt(dh*dh+dv*dv));
}

// ---------------------------------------------------------------------------
// 
// -----------
double	vx2vec_dist(i2dvertex* vx,	i2dvertex* vxa,	i2dvertex* vxb){
double	r,dh,dv;
	
	dh=vxb->h-vxa->h;
	dv=vxb->v-vxa->v;
	if((dh==0)&&(dv==0)){
		return(ivx2_dist(vxa,vx));
	}
	else{
		r=((vx->h-vxa->h)*dh+(vx->v-vxa->v)*dv)/(dh*dh+dv*dv);
		if(r<=0){
			return(ivx2_dist(vxa,vx));
		}
		else if(r>=1){
			return(ivx2_dist(vxb,vx));
		}
		else{
			dh=vxa->h+r*dh-vx->h;
			dv=vxa->v+r*dv-vx->v;
			return(sqrt(dh*dh+dv*dv));
		}
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
double	ivx2ivs_dist(i2dvertex* vx,	ivertices* vs){
int		i;
double	d,dmin;

//	-> 21/05/08, suite à bug identifié dans le nettoyage
	if(vs->nv<2){// Un point
		return(ivx2_dist(vx,&vs->vx.vx2[0]));
	}
	if(vs->nv==vs->no){// Un multi-point
		dmin=(ivx2_dist(vx,&vs->vx.vx2[0]));
		for(i=1;i<vs->nv;i++){
			d=ivx2_dist(vx,&vs->vx.vx2[i]);
			if(d<dmin){
				dmin=d;
			}	
		}
		return(dmin);
	}
//	<- 21/05/08, suite à bug identifié dans le nettoyage
	
	dmin=vx2vec_dist(vx,&vs->vx.vx2[0],&vs->vx.vx2[1]);
	for(i=1;i<vs->nv-1;i++){
		d=vx2vec_dist(vx,&vs->vx.vx2[i],&vs->vx.vx2[i+1]);
		if(d<dmin){
			dmin=d;
		}
	}
	return(dmin);
}

// ---------------------------------------------------------------------------
// 
// -----------
double	vxs_dist(ivertices* vsa, ivertices* vsb){
int		i;
double	d,dmin;
	
	if((vsa->nv==1)&&(vsb->nv==1)){
		return(ivx2_dist(&vsa->vx.vx2[0],&vsb->vx.vx2[0]));
	}
	else if((vsa->nv==1)&&(vsb->nv>1)){
		return(ivx2ivs_dist(&vsa->vx.vx2[0],vsb));
	}
	else if((vsb->nv==1)&&(vsa->nv>1)){
		return(ivx2ivs_dist(&vsb->vx.vx2[0],vsa));
	}
	else if((vsa->nv>1)&&(vsb->nv>1)){
		dmin=ivx2ivs_dist(&vsa->vx.vx2[0],vsb);
		for(i=1;i<vsa->nv;i++){
			d=ivx2ivs_dist(&vsa->vx.vx2[i],vsb);
			if(d<dmin){
				dmin=d;
			}
		}
		for(i=0;i<vsb->nv;i++){
			d=ivx2ivs_dist(&vsb->vx.vx2[i],vsa);
			if(d<dmin){
				dmin=d;
			}
		}
		return(dmin);
	}
	else{
		return(-1);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
double	vxs_dist_end(ivertices* vsa, ivertices* vsb, dst_end_kind* k){
double		dac=ivx2_dist(&vsa->vx.vx2[0],&vsb->vx.vx2[0]);
double		dad=ivx2_dist(&vsa->vx.vx2[0],&vsb->vx.vx2[vsb->nv-1]);
double		dbc=ivx2_dist(&vsa->vx.vx2[vsa->nv-1],&vsb->vx.vx2[0]);
double		dbd=ivx2_dist(&vsa->vx.vx2[vsa->nv-1],&vsb->vx.vx2[vsb->nv-1]);
double		dmin=dac;
	*k=kDistEndKindAC;
	if(dmin>dad){
		*k=kDistEndKindAD;
		dmin=dad;
	}
	if(dmin>dbc){
		*k=kDistEndKindBC;
		dmin=dbc;
	}
	if(dmin>dbd){
		*k=kDistEndKindBD;
		dmin=dbd;
	}
	return(dmin);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivs_multigon(ivertices* vs){
int		i,n,np,ck=0;
	
	n=ivs_n_parts(vs);
	if(n==1){
		return(0);
	}
	if(vs->sign==_2D_VX){
i2dvertex*	vx;
		for(i=0;i<n;i++){
			vx=ivs2_part(vs,i,&np);
			if(ivx2_clockwise(vx,np)==1){
				ck++;
			}
		}
	}
	else{
i3dvertex*	vx;
		for(i=0;i<n;i++){
			vx=ivs3_part(vs,i,&np);
			if(ivx3_clockwise(vx,np)==1){
				ck++;
			}		
		}
	}
	return(ck>1);
}

// ---------------------------------------------------------------------------
// result in radian
// -----------
double	vx_angle(i2dvertex* vxa, i2dvertex* vxb){
	return(xy_vx_angle(vxa,vxb));
}

// ---------------------------------------------------------------------------
// result in radian
// -----------
double	xy_vx_angle(i2dvertex* vxa, i2dvertex* vxb){
double	x=vxb->h-vxa->h;
double	y=(-vxb->v)-(-vxa->v);// for x/y geometry
double	a;

	if(x==0){
		a=M_PI/2.0;
		if(y<0){
			a+=M_PI;
		}
	}
	else{
	 	a=atan(y/x);
	 	if(x<0){
	 		a+=M_PI;
	 	}
	} 
	if(a<0){
		a+=(2.0*M_PI);
	}
	return(a);
}

// ---------------------------------------------------------------------------
// result in radian
// -----------
double	hv_vx_angle(i2dvertex* vxa, i2dvertex* vxb){
double	x=vxb->h-vxa->h;
double	y=vxb->v-vxa->v;// for h/v geometry
double	a;

	if(x==0){
		a=M_PI/2.0;
		if(y<0){
			a+=M_PI;
		}
	}
	else{
	 	a=atan(y/x);
	 	if(x<0){
	 		a+=M_PI;
	 	}
	} 
	if(a<0){
		a+=(2.0*M_PI);
	}
	return(a);
}

// ---------------------------------------------------------------------------
// result in radian
// -----------
double	vx_theta(i2dvertex* vxa, i2dvertex* vxb){
int		dx=vxb->h-vxa->h;
int		ax=labs(dx);
int		dy=vxb->v-vxa->v;
int		ay=labs(dy);
double	t=(ax+ay==0)?0.0:(double)dy/(ax+ay);
	if(dx<0){
		t=2.0-t;
	}
	else if(dy<0){
		t+=4.0;
	}
	return(deg2rad(t*90.0));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivr_sect(ivx_rect* vr1,	ivx_rect* vr2,	ivx_rect* vr){
	vr->left=(vr1->left<vr2->left)?vr2->left:vr1->left;
	vr->top=(vr1->top<vr2->top)?vr2->top:vr1->top;
	vr->right=(vr1->right>vr2->right)?vr2->right:vr1->right;
	vr->bottom=(vr1->bottom>vr2->bottom)?vr2->bottom:vr1->bottom;
	return((vr->left<=vr->right)&&(vr->top<=vr->bottom));
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivr_union(ivx_rect* vr1, ivx_rect* vr2, ivx_rect* vr){
	vr->left=(vr1->left<vr2->left)?vr1->left:vr2->left;
	vr->top=(vr1->top<vr2->top)?vr1->top:vr2->top;
	vr->right=(vr1->right>vr2->right)?vr1->right:vr2->right;
	vr->bottom=(vr1->bottom>vr2->bottom)?vr1->bottom:vr2->bottom;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivr_inset(ivx_rect* vr, int h, int v){
	vr->left+=h;
	vr->top+=v;
	vr->right-=h;
	vr->bottom-=v;
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivr_in_ivr(ivx_rect* vr1,	ivx_rect* vr2){
	return(	(vr1->left>=vr2->left)		&&
			(vr1->top>=vr2->top)		&&
			(vr1->right<=vr2->right)	&&
			(vr1->bottom<=vr2->bottom)	);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	eq_ivx2(i2dvertex* vx1,	i2dvertex* vx2){
	return((vx1->h==vx2->h)&&(vx1->v==vx2->v));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	eq_ivx3(i3dvertex* vx1,	i3dvertex* vx2){
	return((vx1->h==vx2->h)&&(vx1->v==vx2->v)&&(vx1->m==vx2->m));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	eq_dvx2(d2dvertex* vx1,	d2dvertex* vx2){
	return((vx1->x==vx2->x)&&(vx1->y==vx2->y));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	eq_dvx3(d3dvertex* vx1,	d3dvertex* vx2){
	return((vx1->x==vx2->x)&&(vx1->y==vx2->y)&&(vx1->z==vx2->z));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	eq_ivr(ivx_rect* vr1,	ivx_rect* vr2){
	return(	(vr1->left==vr2->left)		&&
			(vr1->top==vr2->top)		&&
			(vr1->right==vr2->right)	&&
			(vr1->bottom==vr2->bottom)	);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivx2_closed(i2dvertex* ring, int n){
	return(eq_ivx2(&ring[0],&ring[n-1]));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivs_closed(ivertices* vxs){
	return((vxs->nv>3)&&(eq_ivx2(&vxs->vx.vx2[0],&vxs->vx.vx2[vxs->nv-1])));
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_bounds(ivertices* vxs, ivx_rect* vr){
int	i;	
	if(!vxs){
		vr->top=vr->bottom=vr->left=vr->right=0;
		return;
	}
	if(vxs->nv<=0){
		vr->top=vr->bottom=vr->left=vr->right=0;
		return;
	}
	vr->top=vxs->vx.vx2[0].v;
	vr->bottom=vxs->vx.vx2[0].v;
	vr->right=vxs->vx.vx2[0].h;
	vr->left=vxs->vx.vx2[0].h;
	for(i=1;i<vxs->nv;i++){
		if(vr->top>vxs->vx.vx2[i].v){
			vr->top=vxs->vx.vx2[i].v;
		}
		else if(vr->bottom<vxs->vx.vx2[i].v){
			vr->bottom=vxs->vx.vx2[i].v;
		}
		if(vr->left>vxs->vx.vx2[i].h){
			vr->left=vxs->vx.vx2[i].h;
		}
		else if(vr->right<vxs->vx.vx2[i].h){
			vr->right=vxs->vx.vx2[i].h;
		}
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivr_mid(ivx_rect* vr, i2dvertex* vx){
	vx->h=round(((double)(vr->left+vr->right))/2.0);
	vx->v=round(((double)(vr->top+vr->bottom))/2.0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivr_ok(ivx_rect* vr){
	return((vr->left<vr->right)&&(vr->top<vr->bottom));
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivx2_sect(	i2dvertex* aa,
				i2dvertex* ab,
				i2dvertex* ba,
				i2dvertex* bb){
int	c1,c2,c3,c4;
	c1=vx_siam(aa,ab,ba);
	c2=vx_siam(aa,ab,bb);
	c3=vx_siam(ba,bb,aa);
	c4=vx_siam(ba,bb,ab);
	return	(	((c1*c2)<=0)
			&&	((c3*c4)<=0)
			&&	((c1+c2+c3+c4)<=4));
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivx2_get_sect(	i2dvertex* aa,
					i2dvertex* ab,
					i2dvertex* ba,
					i2dvertex* bb,
					i2dvertex* r){
int			v1=0,v2=0;
double		a1,a2,b1,b2,q;
i2dvertex	p;
	
	q=(aa->h-ab->h);
	if(q!=0){
		a1=((double)(aa->v-ab->v))/q;
		b1=((double)aa->v)-(a1*((double)aa->h));
	}
	else{
		a1=0;
		b1=0;
		v1=1;
	}
	q=(ba->h-bb->h);
	if(q!=0){
		a2=((double)(ba->v-bb->v))/q;
		b2=((double)ba->v)-(a2*((double)ba->h));
	}
	else{
		a2=0;
		b2=0;
		v2=1;
	}
	if((!v1)&&(!v2)){
		q=(b2-b1)/(a1-a2);
		p.v=round(q*a1+b1);
		p.h=round(q);
	}
	else if(v1){
		q=aa->h;
		p.v=round(q*a2+b2);
		p.h=round(q);
	}
	else{
		q=ba->h;
		p.v=round(q*a1+b1);
		p.h=round(q);
	}
	if((a1==a2)&&(b1==b2)){
		if(ivx2_dir(aa,ab)==ivx2_dir(ba,bb)){
			if(	beetween(aa->h,bb->h,ab->h)	&&
				beetween(aa->v,bb->v,ab->v)	){
				p=*bb;
			}
			else{
				p=*ab;
			}
		}
		else{
			if(	beetween(ba->h,ab->h,bb->h)	&&
				beetween(ba->v,ab->v,bb->v)	){
				p=*ab;
			}
			else{
				p=*ba;
			}
		}
	}	
	*r=p;
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivx2_dir(i2dvertex* va,
			 i2dvertex* vb){
double	a,sens,d;

	a=vx_angle(va,vb);
	d=ivx2_dist(va,vb);
	sens=((a>M_PI/2.0)&&(a<=3.0*M_PI/2.0))?-d:d;
	return(sens>=0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivx2_in_ivr(ivx_rect*	vr,	
				i2dvertex*	vx){
	return(vx->h>=vr->left&&vx->h<=vr->right&&vx->v>=vr->top&&vx->v<=vr->bottom);
}

// ---------------------------------------------------------------------------
// OLD KLIK STUFF (FOR JOIN ON DIR)
// -----------
/*
// ---------------------------------------------------------------------------
// 
// -----------
int		ivx_sect		(	i2dvertex* a,
							i2dvertex* b,
							i2dvertex* c,
							i2dvertex* d,
							i2dvertex* r);

int ivx_sect(	i2dvertex* a,
				i2dvertex* b,
				i2dvertex* c,
				i2dvertex* d,
				i2dvertex* r){
double ca,cb,cc,cd,cr;

	if((a->h!=b->h)&&(c->h!=d->h)){
//fprintf(stderr,"CAS 1\n");
		ca=((double)(b->v-a->v))/((double)(b->h-a->h));
		cb=(double)a->v-(double)a->h*ca;
		cc=((double)(d->v-c->v))/((double)(d->h-c->h));
		cd=(double)c->v-(double)c->h*cc;
		if(cc==ca){
//fprintf(stderr,"REJET 1\n");
			return(0);
		}
		cr=(cb-cd)/(cc-ca);
		r->h=round(cr);
		r->v=round(ca*cr+cb);
		return(1);
	}
	
	if((a->h==b->h)&&(c->h!=d->h)){
//fprintf(stderr,"CAS 2\n");
		r->h=a->h;
		r->v=c->v+round(((double)(d->v-c->v))/((double)(d->h-c->h))*((double)(a->h-c->h)));
		return(1);
	}
	
	if((c->h==d->h)&&(a->h!=b->h)){
//fprintf(stderr,"CAS 3\n");
		r->h=d->h;
		r->v=a->v+round(((double)(b->v-a->v))/((double)(b->h-a->h))*((double)(c->h-a->h)));
		return(1);
	}
//fprintf(stderr,"REJET 2\n");
	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int		ivx_on_seg		(	i2dvertex* vx,
							i2dvertex* a,
							i2dvertex* b);
int ivx_on_seg(	i2dvertex* vx,
				i2dvertex* a,
				i2dvertex* b){
	return(	(	((a->h<=vx->h)&&(vx->h<=b->h))	||
				((a->h>=vx->h)&&(vx->h>=b->h))	)
			&& 
			(	((a->v<=vx->v)&&(vx->v<=b->v))	|| 
				((a->v>=vx->v)&&(vx->v>=b->v))	));
}
*/
