//----------------------------------------------------------------------------
// File : vx_raw.c
// Project : MacMapSuite
// Purpose : C source file : Geometry raw utils
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
// 27/10/2006 creation.
//----------------------------------------------------------------------------

#include "vx_raw.h"
#include "vx_measure.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// global
// -----------
static i2dvertex	q;

// ---------------------------------------------------------------------------
// theta comparison + dist for equal theta
// -----------
static int theta_compare(const void* a, const void* b){
i2dvertex*	va=(i2dvertex*)a;
i2dvertex*	vb=(i2dvertex*)b;
double		ra=vx_theta(&q,va);
double		rb=vx_theta(&q,vb);
double		r=ra-rb;
	if(r<0){
		return(-1);
	}
	if(r>0){
		return(1);
	}
	r=ivx2_dist(&q,va)-ivx2_dist(&q,vb);
	if(r<0){
		return(-1);
	}
	if(r>0){
		return(1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	vx_quad(int	dh, int	dv){
	if((dh>=0)&&(dv>0)){
		return(0);
	}
	else if((dh>0)&&(dv<=0)){
		return(1);
	}
	else if((dh<=0)&&(dv<0)){
		return(2);
	}
	else{
		return(3);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	vx_cquad(int oq, int q, int* lq, double* oh, double* ov, int dh, int dv){
double	e;

	switch(q-oq){
		case -3:
		case 1:
			(*lq)++;
			break;
		case -2:
		case 2:
			e=-(*oh)*dv+(*ov)*dh;
			if(e<0){
				(*lq)+=2;
			}
			else if(e>0){
				(*lq)-=2;
			}
			else{
				return(1);
			}
			break;
		case -1:
		case 3:
			(*lq)--;
			break;
		case 0:
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// theta sort
// -----------
void vx_theta_qsort(i2dvertex* r, i2dvertex* t, int n){
	q=*r;
fprintf(stderr,"q=(%d;%d)\n",q.h,q.v);
	qsort(t,n,sizeof(i2dvertex),theta_compare);
}

// ---------------------------------------------------------------------------
// rotation p1->p0->p2
// -----------
int vx_siam(const i2dvertex *p0, const i2dvertex *p1, const i2dvertex *p2){
// must be double for small angles	
double dx1=p1->h-p0->h;
double dy1=p1->v-p0->v;
double dx2=p2->h-p0->h;
double dy2=p2->v-p0->v;
	if(dx1*dy2>dy1*dx2){
		return(1);
	}
	if(dx1*dy2<dy1*dx2){
		return(-1);
	}
	if((dx1*dx2<0)||(dy1*dy2<0)){
		return(-1);
	}
	if((dx1*dx1+dy1*dy1)<(dx2*dx2+dy2*dy2)){
		return(1);
	}	
	return(0);
}
