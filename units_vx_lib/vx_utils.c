//----------------------------------------------------------------------------
// File : vx_utils.c
// Project : MacMapSuite
// Purpose : C source file : Geometry utils
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
#include "vx_measure.h"
#include "vx_utils.h"
#include "C_Utils.h"

#include "wtrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ---------------------------------------------------------------------------
// 
// -----------
enum{
	kGeomError,
	kGeomNotInited,
	kGeomBegin,
	kPartBegin,
	kCoordX,
	kCoordY,
	kPartEnd,
	kGeomEnd
};

// ---------------------------------------------------------------------------
// 
// -----------
double ivx_projonseg(	i2dvertex*	vx,
						i2dvertex*	vxa,
						i2dvertex*	vxb,
						i2dvertex*	vr,
						double*		r){
double	dh=vxb->h-vxa->h;
double	dv=vxb->v-vxa->v;

	if((dh==0)&&(dv==0)){
	    *r=0;
		*vr=*vxa;
		return(ivx2_dist(vxa,vx));
	}
	
	*r=(((double)(vx->h-vxa->h))*dh+((double)(vx->v-vxa->v))*dv)/(dh*dh+dv*dv);
	if(*r<=0){
		*vr=*vxa;
		return(ivx2_dist(vxa,vx));
	}
	
	if(*r>=1){
		*vr=*vxb;
		return(ivx2_dist(vxb,vx));
	}
	
	dh=vxa->h+round(*r*dh)-vx->h;
	dv=vxa->v+round(*r*dv)-vx->v;
	vr->h=round(vx->h+dh);
	vr->v=round(vx->v+dv);
	return(sqrt(dh*dh+dv*dv));
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivrset(ivx_rect* vr, int left, int top, int right, int bottom){
	if(left<right){
		vr->left=left;
		vr->right=right;
	}
	else{
		vr->right=left;
		vr->left=right;
	}
	if(top<bottom){
		vr->top=top;
		vr->bottom=bottom;
	}
	else{
		vr->bottom=top;
		vr->top=bottom;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivs_good(ivertices* vxs){
	if(vxs==NULL){
fprintf(stderr,"ivs_good::NULL vxs\n");
		return(-10);
	}	
	if((vxs->sign!=_2D_VX)&&(vxs->sign!=_3D_VX)){
fprintf(stderr,"ivs_good::bad sign\n");
		return(-1);
	}
	if(vxs->nv<=0){
fprintf(stderr,"ivs_good::nv=%d\n",vxs->nv);
		return(-2);
	}
	if(vxs->no<0){
fprintf(stderr,"ivs_good::no=%d\n",vxs->no);
		return(-3);
	}
	if(vxs->no>vxs->nv){
fprintf(stderr,"ivs_good::no=%d>nv=%d\n",vxs->no,vxs->nv);
		return(-4);
	}
	if((vxs->no>0)&&(vxs->offs==NULL)){
fprintf(stderr,"ivs_good::no=%d, offs=NULL\n",vxs->no);
		return(-5);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivs_goodgeo(ivertices* vxs){
int	i;
	
	for(i=0;i<vxs->nv;i++){
		if(abs(vxs->vx.vx2[i].h)>__BOUNDS_MAX__){
			return(0);
		}
		if(abs(vxs->vx.vx2[i].v)>__BOUNDS_MAX__){
			return(0);
		}
	}
	return(1);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_closegeom(ivertices* vxs){
int			i,n,np=ivs_n_parts(vxs);
i2dvertex*	p;

	for(i=0;i<np;i++){
		p=ivs2_part(vxs,i,&n);
		if(!eq_ivx2(&p[0],&p[n-1])){
			p[n-1]=p[0];
		}
	}
}

// ---------------------------------------------------------------------------
//
// -----------
int	ivs_integrity(int kind, ivertices* vxs){
i2dvertex*	vp;
long		i;
int         n;

    if((vxs->no>0)&&(vxs->offs==NULL)){
        return -200;
    }
    for(i=0;i<ivs_n_parts(vxs);i++){
        vp=ivs2_part(vxs,i,&n);
        if(vp==NULL){
            return -201;
        }
        if(n>vxs->nv){
            return -202;
        }
        switch(kind){
            case kVXPoint:
                if((n!=1)&&(vxs->nv!=vxs->no)){
                    return -210;
                }
                break;
            case kVXPolyline:
                if(n<2){
                    return -220;
                }
                break;
            case 3:// Text
                if(n<1){
                    return -230;
                }
                break;
            case kVXPolygon:
                if(n<4){
                    return -240;
                }
                break;
            case 5:// Raster
                if(n<5){
                    return -250;
                }
                break;
        }
    }
    return(0);
}

// ---------------------------------------------------------------------------
//
// -----------
ivertices*  ivs_polylineFix(ivertices* vxs){
    if(ivs_n_parts(vxs)==0){
        return NULL;
    }
/*    if(ivs_n_parts(vxs)==1){
        return vxs;
    }*/

int         n;
long        i,j;
ivertices   *res,*tmp;
i2dvertex   *vp;
    
    res=ivs_new(_2D_VX,0,0);
    for(i=0;i<ivs_n_parts(vxs);i++){
        vp=ivs2_part(vxs,i,&n);
        if(vp==NULL){
            continue;
        }
        if(n>vxs->nv){
            continue;
        }
        tmp=ivs_new(_2D_VX,1,0);
        tmp->vx.vx2[0]=vp[0];
        for(j=0;j<n-1;j++){
            if(eq_ivx2(&vp[j],&vp[j+1])){
                continue;
            }
            tmp=ivx2_add(tmp,&vp[j+1]);
        }
        if(tmp->nv>=2){
            if(res->nv==0){
                res=ivs_concat(res,tmp);
            }
            else{
                res=ivs_group(res,tmp);
            }
        }
        ivs_free(tmp);
    }
    if(ivs_integrity(kVXPolyline,res)!=0){
        ivs_free(res);
        res=NULL;
    }
    
//char* txt=ivs2text(kVXPolyline,1,res);
//fprintf(stderr,"%s\n",txt);
//free(txt);
    
    return res;
}

// ---------------------------------------------------------------------------
//
// -----------
ivertices*  ivs_polygonFix(ivertices* vxs){
    if(ivs_n_parts(vxs)==0){
        return NULL;
    }
/*    if(ivs_n_parts(vxs)==1){
        return vxs;
    }*/

int         n;
long        i,j;
ivertices   *res,*tmp;
i2dvertex   *vp;
    
    res=ivs_new(_2D_VX,0,0);
    for(i=0;i<ivs_n_parts(vxs);i++){
        vp=ivs2_part(vxs,i,&n);
        if(vp==NULL){
            continue;
        }
        if(n>vxs->nv){
            continue;
        }
        tmp=ivs_new(_2D_VX,1,0);
        tmp->vx.vx2[0]=vp[0];
        for(j=0;j<n-1;j++){
            if(eq_ivx2(&vp[j],&vp[j+1])){
                continue;
            }
            tmp=ivx2_add(tmp,&vp[j+1]);
        }
        if(tmp->nv>=4){
            if(res->nv==0){
                res=ivs_concat(res,tmp);
            }
            else{
                res=ivs_group(res,tmp);
            }
        }
        ivs_free(tmp);
    }
    if(ivs_integrity(kVXPolyline,res)!=0){
        ivs_free(res);
        res=NULL;
    }
    return res;
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivx_angleproj(i2dvertex* vx, i2dvertex* vx1, i2dvertex* vx2, double rad){
// géometries entières décroissant vers le haut
	vx1->v=-vx1->v;
	vx2->v=-vx2->v;
double	alpha=vx_angle(vx1,vx2);
	vx1->v=-vx1->v;
	vx2->v=-vx2->v;
double	r=ivx2_dist(vx1,vx2);
double	beta=((round((alpha+rad)/rad)-1.0)*rad);	
	vx->h=vx1->h+round(r*cos(beta));
	vx->v=vx1->v+round(r*sin(beta));
}


// ---------------------------------------------------------------------------
// 
// -----------
void ivx_proj_at_dist(	i2dvertex*	vx,
						i2dvertex*	vr,
						double		d,
						double		rad){
	vr->h=vx->h+d*cos(rad);
	vr->v=vx->v+d*sin(rad);
}

// ---------------------------------------------------------------------------
// 
// -----------
static i2dvertex*	ivx2_sub(ivertices* vxs, int idx, int* count){
	if(vxs->sign==_3D_VX){
		return(NULL);
	}
	if(idx>=vxs->no){
		return(NULL);
	}
	*count=(idx==vxs->no-1)?vxs->nv-vxs->offs[idx]:vxs->offs[idx+1]-vxs->offs[idx];
	return(&vxs->vx.vx2[vxs->offs[idx]]);
}

// ---------------------------------------------------------------------------
// 
// -----------
static i3dvertex*	ivx3_sub(ivertices* vxs, int idx, int* count){
	if(vxs->sign==_2D_VX){
		return(NULL);
	}
	if(idx>=vxs->no){
		return(NULL);
	}
	*count=(idx==vxs->no-1)?vxs->nv-vxs->offs[idx]:vxs->offs[idx+1]-vxs->offs[idx];
	return(&vxs->vx.vx3[vxs->offs[idx]]);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivx2_add(ivertices* vxs, i2dvertex* vx){
int			sz;
ivertices*	rvxs;
//-> CHECKED
	if(vxs->sign==_3D_VX){
		return(vxs);
	}
	if(eq_ivx2(&vxs->vx.vx2[vxs->nv-1],vx)){
//fprintf(stderr,"ivx2_add::equal points \n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(i2dvertex);
	rvxs=(ivertices*)realloc(vxs,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
		return(vxs);
	}
	rvxs->nv++;
	rvxs->vx.vx2[rvxs->nv-1]=*vx;
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivx3_add(ivertices* vxs, i3dvertex* vx){
int			sz;
ivertices*	rvxs;
//-> CHECKED
	if(vxs->sign==_2D_VX){
		return(vxs);
	}
	if(eq_ivx3(&vxs->vx.vx3[vxs->nv-1],vx)){
//fprintf(stderr,"ivx3_add::equal points \n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(i3dvertex);
	rvxs=(ivertices*)realloc(vxs,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
		return(vxs);
	}
	rvxs->nv++;
	rvxs->vx.vx3[rvxs->nv-1]=*vx;
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices*	dvx2_add(dvertices* vxs, d2dvertex* vx){
int			sz;
dvertices*	rvxs;
//-> CHECKED
	if(vxs->sign==_3D_VX){
		return(vxs);
	}
	if(eq_dvx2(&vxs->vx.vx2[vxs->nv-1],vx)){
//fprintf(stderr,"dvx2_add::equal points \n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(d2dvertex);
	rvxs=(dvertices*)realloc(vxs,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
	if(!rvxs){
		return(vxs);
	}
	rvxs->nv++;
	rvxs->vx.vx2[rvxs->nv-1]=*vx;
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices*	dvx3_add(dvertices* vxs, d3dvertex* vx){
int			sz;
dvertices*	rvxs;
//-> CHECKED
	if(vxs->sign==_2D_VX){
		return(vxs);
	}
	if(eq_dvx3(&vxs->vx.vx3[vxs->nv-1],vx)){
//fprintf(stderr,"dvx3_add::equal points \n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(d3dvertex);
	rvxs=(dvertices*)realloc(vxs,(sizeof(dvertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
		return(vxs);
	}
	rvxs->nv++;
	rvxs->vx.vx3[rvxs->nv-1]=*vx;
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivx2_insert(ivertices* vxs, i2dvertex* vx, int idx){
int			i,sz;
ivertices*	rvxs;
i2dvertex	bf=*vx;	// pour régler le cas ou on passe directement une référence à 
					// un vertex de vxs

//-> CHECKED
	if(vxs->sign==_3D_VX){
fprintf(stderr,"ivx2_insert::bad sign (%d)\n",vxs->sign);
		return(vxs);
	}
	if(eq_ivx2(&vxs->vx.vx2[idx],&bf)){
fprintf(stderr,"ivx2_insert::equal points \n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(i2dvertex);
	rvxs=(ivertices*)realloc(vxs,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
fprintf(stderr,"ivx2_insert::realloc failed for idx=%d (sz=%d)\n",idx,sz);
		return(vxs);
	}
	(void)memmove(&rvxs->vx.vx2[idx+1],&rvxs->vx.vx2[idx],(rvxs->nv-idx)*sizeof(i2dvertex));
	rvxs->vx.vx2[idx]=bf;
	rvxs->nv++;
	if(rvxs->no>0){
		for(i=rvxs->no-1;i>=0;i--){
			if(idx<=rvxs->offs[i]){
				rvxs->offs[i]++;
			}
		}
	}
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivx3_insert(ivertices* vxs, i3dvertex* vx, int idx){
int			i,sz;
ivertices*	rvxs;
i3dvertex	bf=*vx;	// pour régler le cas ou on passe directement une référence à 
					// un vertex de vxs
//-> CHECKED
	if(vxs->sign==_2D_VX){
fprintf(stderr,"ivx2_insert::bad sign (%d)\n",vxs->sign);
		return(vxs);
	}
	if(eq_ivx3(&vxs->vx.vx3[idx],&bf)){
fprintf(stderr,"ivx2_insert::equal points\n");
		return(vxs);
	}
	sz=(vxs->nv+1)*sizeof(i3dvertex);
	rvxs=(ivertices*)realloc(vxs,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
fprintf(stderr,"ivx2_insert::realloc failed for idx=%d (sz=%d)\n",idx,sz);
		return(vxs);
	}
	(void)memmove(&rvxs->vx.vx3[idx+1],&rvxs->vx.vx3[idx],(rvxs->nv-idx)*sizeof(i3dvertex));
	rvxs->vx.vx3[idx]=bf;
	rvxs->nv++;
	if(rvxs->no>0){
		for(i=rvxs->no-1;i>=0;i--){
			if(idx<=rvxs->offs[i]){
				rvxs->offs[i]++;
			}
		}
	}
//<- CHECKED
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivx_remove(ivertices* vxs, int idx){
int			i,sz;
ivertices*	rvxs;

//-> CHECKED
	sz=(vxs->sign==_2D_VX)?(vxs->nv-1)*sizeof(i2dvertex):(vxs->nv-1)*sizeof(i3dvertex);
	if((idx<vxs->nv)&&(idx>=0)){
		if(vxs->sign==_2D_VX){
			(void)memmove(&vxs->vx.vx2[idx],&vxs->vx.vx2[idx+1],((vxs->nv)-(idx+1))*sizeof(i2dvertex));
		}
		else{
			(void)memmove(&vxs->vx.vx3[idx],&vxs->vx.vx3[idx+1],((vxs->nv)-(idx+1))*sizeof(i3dvertex));
		}
	}
	rvxs=(ivertices*)realloc(vxs,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
	if(!rvxs){
		return(NULL);
	}
	rvxs->nv--;
//<- CHECKED

	if(rvxs->no>0){
		for(i=rvxs->no-1;i>=0;i--){
			if(idx<=rvxs->offs[i]){
				rvxs->offs[i]--;
			}
		}
	}
	return(rvxs);
}

// ---------------------------------------------------------------------------
// TO DO : gestion offsets
// -----------
void	ivs_split(ivertices* vxs, int idx, ivertices** vsa, ivertices** vsb){
	*vsa=NULL;
	*vsb=NULL;
	if(!vxs){
        fprintf(stderr,"ivs_split : NO VXS");
		return;
	}
	if(vxs->nv<3){
        fprintf(stderr,"ivs_split : vxs->nv<3");
		return;
	}
	if((idx<=0)||(idx>=vxs->nv-1)){
        fprintf(stderr,"ivs_split : (idx<=0)||(idx>=vxs->nv-1) [0 < %d < %d]",idx,vxs->nv-1);
		return;
	}

int	na=idx+1;
int	nb=vxs->nv-idx;
	*vsa=ivs_new(vxs->sign,na,0);
	*vsb=ivs_new(vxs->sign,nb,0);
	if(vxs->sign==_2D_VX){
		memmove((*vsa)->vx.vx2,&vxs->vx.vx2[0],na*sizeof(i2dvertex));
		memmove((*vsb)->vx.vx2,&vxs->vx.vx2[idx],nb*sizeof(i2dvertex));
	}
	else{
		memmove((*vsa)->vx.vx2,&vxs->vx.vx3[0],na*sizeof(i3dvertex));
		memmove((*vsb)->vx.vx2,&vxs->vx.vx3[idx],nb*sizeof(i3dvertex));		
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivs_concat(ivertices* vsa, ivertices* vsb){
int			sz,first;
ivertices*	rvxs;
	if(vsa->sign!=vsb->sign){
		return(NULL);
	}
	if(vsa->sign==_2D_VX){
		if(	eq_ivx2(&vsa->vx.vx2[vsa->nv-1],
					&vsb->vx.vx2[0])		){
			first=1;
		}
		else{
			first=0;
		}
		if((first)&&(vsb->nv==1)){
			return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(i2dvertex);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx2[rvxs->nv],
						&vsb->vx.vx2[first],
						(vsb->nv-first)*sizeof(i2dvertex));
		rvxs->nv+=(vsb->nv-first);
	}
	else{
		if(	eq_ivx3(&vsa->vx.vx3[vsa->nv-1],
					&vsb->vx.vx3[0])		){
			first=1;
		}
		else{
			first=0;
		}
		if((first)&&(vsb->nv==1)){
			return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(i3dvertex);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx3[rvxs->nv],
						&vsb->vx.vx2[first],
						(vsb->nv-first)*sizeof(i3dvertex));
		rvxs->nv+=(vsb->nv-first);
	}
	rvxs->no=0;
	rvxs->offs=NULL;
	return(rvxs);
}

// ---------------------------------------------------------------------------
// VERIF 22/01/10 OK
// -----------
ivertices*	ivs_group(ivertices* vsa, ivertices* vsb){
int			sz,i,nvb=vsa->nv;
ivertices*	rvxs;
int*		buff;

	if(vsa->sign!=vsb->sign){
		return(NULL);
	}
	if(vsa->sign==_2D_VX){
		sz=(vsa->nv+vsb->nv)*sizeof(i2dvertex);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&(rvxs->vx.vx2[rvxs->nv]),
						&(vsb->vx.vx2[0]),
						(vsb->nv)*sizeof(i2dvertex));
		rvxs->nv+=(vsb->nv);
	}
	else{
		sz=(vsa->nv+vsb->nv)*sizeof(i3dvertex);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx3[rvxs->nv],
						&vsb->vx.vx2[0],
						(vsb->nv)*sizeof(i3dvertex));
		rvxs->nv+=(vsb->nv);
	}	
	
	if((rvxs->no==0)&&(vsb->no==0)){
		if(rvxs->nv<2){
			return(rvxs);
		}
		rvxs->offs=(int*)malloc(2*sizeof(int));
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs->offs=(int*)malloc(2*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no=2;
		rvxs->offs[0]=0;
		rvxs->offs[1]=rvxs->nv-vsb->nv;
	}
	else if((rvxs->no>0)&&(vsb->no>0)){
		buff=(int*)realloc(rvxs->offs,(rvxs->no+vsb->no)*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)realloc(rvxs->offs,rvxs->no*sizeof(int))\n");
            ivs_free(rvxs);
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[rvxs->no],vsb->offs,(vsb->no)*sizeof(int));
		for(i=rvxs->no;i<rvxs->no+vsb->no;i++){
			rvxs->offs[i]+=nvb;
		}
		rvxs->no+=vsb->no;
	}
	else if((rvxs->no>0)&&(vsb->no==0)){
		buff=(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int))\n");
            ivs_free(rvxs);
			return(NULL);
		}
		rvxs->no+=1;
		rvxs->offs=buff;
		rvxs->offs[rvxs->no-1]=nvb;
	}
	else if((rvxs->no==0)&&(vsb->no>0)){
		buff=(int*)malloc(vsb->no+1*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)malloc(rvxs->no*sizeof(int))\n");
            ivs_free(rvxs);
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[1],vsb->offs,(vsb->no)*sizeof(int));
		for(i=1;i<=vsb->no;i++){
			rvxs->offs[i]+=nvb;
		}
		rvxs->offs[0]=0;
		rvxs->no=vsb->no+1;
	}
	else{
	}
	return(rvxs);
}

// ---------------------------------------------------------------------------
// VERIF 22/01/10 OK
// -----------
dvertices*	dvs_group(dvertices* vsa, dvertices* vsb){
int			sz,i,nvb=vsa->nv;
dvertices*	rvxs;
int*		buff;

	if(vsa->sign!=vsb->sign){
		return(NULL);
	}
	if(vsa->sign==_2D_VX){
		sz=(vsa->nv+vsb->nv)*sizeof(d2dvertex);
		rvxs=(dvertices*)realloc(vsa,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"dvs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&(rvxs->vx.vx2[rvxs->nv]),
						&(vsb->vx.vx2[0]),
						(vsb->nv)*sizeof(d2dvertex));
		rvxs->nv+=(vsb->nv);
	}
	else{
		sz=(vsa->nv+vsb->nv)*sizeof(d3dvertex);
		rvxs=(dvertices*)realloc(vsa,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"dvs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx3[rvxs->nv],
						&vsb->vx.vx2[0],
						(vsb->nv)*sizeof(d3dvertex));
		rvxs->nv+=(vsb->nv);
	}
		
	if((rvxs->no==0)&&(vsb->no==0)){
		if(rvxs->nv<2){
			return(rvxs);
		}
		rvxs->offs=(int*)malloc(2*sizeof(int));
		if(!rvxs){
fprintf(stderr,"dvs_group:rvxs->offs=(int*)malloc(2*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no=2;
		rvxs->offs[0]=0;
		rvxs->offs[1]=rvxs->nv-vsb->nv;
	}
	else if((rvxs->no>0)&&(vsb->no>0)){
		buff=(int*)realloc(rvxs->offs,(rvxs->no+vsb->no)*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)realloc(rvxs->offs,rvxs->no*sizeof(int))\n");
            dvs_free(rvxs);
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[rvxs->no],vsb->offs,(vsb->no)*sizeof(int));
		for(i=rvxs->no;i<rvxs->no+vsb->no;i++){
			rvxs->offs[i]+=nvb;
		}
		rvxs->no+=vsb->no;
	}
	else if((rvxs->no>0)&&(vsb->no==0)){
		buff=(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int))\n");
            dvs_free(rvxs);
			return(NULL);
		}
		rvxs->no+=1;
		rvxs->offs=buff;
		rvxs->offs[rvxs->no-1]=nvb;
	}
	else if((rvxs->no==0)&&(vsb->no>0)){
		buff=(int*)malloc(vsb->no+1*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)malloc(rvxs->no*sizeof(int))\n");
            dvs_free(rvxs);
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[1],vsb->offs,(vsb->no)*sizeof(int));
		for(i=1;i<=vsb->no;i++){
			rvxs->offs[i]+=nvb;
		}
		rvxs->offs[0]=0;
		rvxs->no=vsb->no+1;
	}
	else{
	}
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_strip_offs(ivertices* vxs){
	if(vxs->offs){
		free(vxs->offs);
		vxs->offs=NULL;
	}
	vxs->no=0;
}

// ---------------------------------------------------------------------------
// 
// -----------
int	ivs_n_vxs(ivertices* vxs){
	return(vxs->nv);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivs_n_parts(ivertices* vxs){
	return((vxs->no>0)?vxs->no:1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivs_part_num(ivertices* vxs, int idx){
int		i;
	if(vxs->offs==NULL){
		if((idx>=0)&&(idx<vxs->nv)){
			return(0);
		}
	}
	for(i=0;i<vxs->no-1;i++){
		if((idx>=vxs->offs[i])&&(idx<vxs->offs[i+1])){
			return(i);
		}
	}
	if((idx>=vxs->offs[vxs->no-1])&&(idx<vxs->nv)){
		return(vxs->no-1);
	}
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
i2dvertex* ivs2_part(ivertices* vxs, int idx, int* nvx){
	if((vxs->sign!=_2D_VX)||(idx<0)||((vxs->no>0)&&(idx>=vxs->no))){
		*nvx=0;
//fprintf(stderr,"bad : %d / %d\n",vxs->sign,idx);
		return(NULL);
	}
	if((vxs->offs)&&(vxs->no>0)){
//fprintf(stderr,"some Offsets\n");
		*nvx=(idx<(vxs->no-1))?(vxs->offs[idx+1]-vxs->offs[idx]):(vxs->nv-vxs->offs[idx]);
		return(&vxs->vx.vx2[vxs->offs[idx]]);
	}
	else{
//fprintf(stderr,"no Offsets\n");
		*nvx=vxs->nv;
		return(&vxs->vx.vx2[0]);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
i3dvertex* ivs3_part(ivertices* vxs, int idx, int* nvx){
	if((vxs->sign!=_3D_VX)||(idx<0)||(idx>=vxs->no)){
		*nvx=0;
		return(NULL);
	}
	if(vxs->offs){
		*nvx=(idx<(vxs->no-1))?(vxs->offs[idx+1]-vxs->offs[idx]):(vxs->nv-vxs->offs[idx]);
		return(&vxs->vx.vx3[vxs->offs[idx]]);
	}
	else{
		*nvx=vxs->nv;
		return(&vxs->vx.vx3[0]);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	dvs_n_vxs(dvertices* vxs){
	return(vxs->nv);
}

// ---------------------------------------------------------------------------
// 
// -----------
int dvs_n_parts(dvertices* vxs){
	return((vxs->no>0)?vxs->no:1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int dvs_part_num(dvertices* vxs, int idx){
int		i;
	if(vxs->offs==NULL){
		if((idx>=0)&&(idx<vxs->nv)){
			return(0);
		}
	}
	for(i=0;i<vxs->no-1;i++){
		if((idx>=vxs->offs[i])&&(idx<vxs->offs[i+1])){
			return(i);
		}
	}
	if((idx>=vxs->offs[vxs->no-1])&&(idx<vxs->nv)){
		return(vxs->no-1);
	}
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
d2dvertex* dvs2_part(dvertices* vxs, int idx, int* nvx){
	if((vxs->sign!=_2D_VX)||(idx<0)||((vxs->no>0)&&(idx>=vxs->no))){
		*nvx=0;
//fprintf(stderr,"bad : %d / %d\n",vxs->sign,idx);
		return(NULL);
	}
	if((vxs->offs)&&(vxs->no>0)){
//fprintf(stderr,"some Offsets\n");
		*nvx=(idx<(vxs->no-1))?(vxs->offs[idx+1]-vxs->offs[idx]):(vxs->nv-vxs->offs[idx]);
		return(&vxs->vx.vx2[vxs->offs[idx]]);
	}
	else{
//fprintf(stderr,"no Offsets\n");
		*nvx=vxs->nv;
		return(&vxs->vx.vx2[0]);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
d3dvertex* dvs3_part(dvertices* vxs, int idx, int* nvx){
	if((vxs->sign!=_3D_VX)||(idx<0)||(idx>=vxs->no)){
		*nvx=0;
		return(NULL);
	}
	if(vxs->offs){
		*nvx=(idx<(vxs->no-1))?(vxs->offs[idx+1]-vxs->offs[idx]):(vxs->nv-vxs->offs[idx]);
		return(&vxs->vx.vx3[vxs->offs[idx]]);
	}
	else{
		*nvx=vxs->nv;
		return(&vxs->vx.vx3[0]);
	}
}





// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_pack(ivertices* vxs){
int sz;
	if((vxs->no<=0)||(vxs->offs==NULL)){
        return vxs;
	}
	sz=sizeofivs2(vxs->nv,vxs->no);
    vxs=(ivertices*)realloc(vxs,sz);
	switch(vxs->sign){
		case _2D_VX:
			memmove(&vxs->vx.vx2[vxs->nv],vxs->offs,vxs->no*sizeof(int));
			free(vxs->offs);
			vxs->offs=(int*)&vxs->vx.vx2[vxs->nv];
			break;
		case _3D_VX:
			memmove(&vxs->vx.vx3[vxs->nv],vxs->offs,vxs->no*sizeof(int));
			free(vxs->offs);
			vxs->offs=(int*)&vxs->vx.vx3[vxs->nv];
			break;
	}
    return vxs;
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* ivs_unpack(ivertices* vxs){
int sz;
	if(vxs->no<=0){
        vxs->offs=NULL;
        return vxs;
	}
	vxs->offs=(int*)malloc(vxs->no*sizeof(int));
	switch(vxs->sign){
		case _2D_VX:
			memmove(vxs->offs,&vxs->vx.vx2[vxs->nv],vxs->no*sizeof(int));
			break;
		case _3D_VX:
			memmove(vxs->offs,&vxs->vx.vx3[vxs->nv],vxs->no*sizeof(int));
			break;
	}
	sz=sizeofivs2(vxs->nv,0);
	vxs=(ivertices*)realloc(vxs,sz);
    return vxs;
}

// ---------------------------------------------------------------------------
//
// -----------
ivertices* ivs_unpack32(ivertices32* vxs32){
ivertices* vxs=ivs_new(vxs32->sign,vxs32->nv,vxs32->no);
    switch(vxs->sign){
        case _2D_VX:
            memmove(vxs->vx.vx2,&vxs32->vx.vx2,vxs->nv*sizeof(i2dvertex));
            break;
        case _3D_VX:
            memmove(vxs->vx.vx3,&vxs32->vx.vx3,vxs->nv*sizeof(i3dvertex));
            break;
    }
    if(vxs32->no<=0){
        vxs->offs=NULL;
        return vxs;
    }
    vxs->offs=(int*)malloc(vxs->no*sizeof(int));
size_t    sz;    
    switch(vxs->sign){
        case _2D_VX:
            memmove(vxs->offs,&vxs32->vx.vx2[vxs->nv],vxs->no*sizeof(int));
            break;
        case _3D_VX:
            memmove(vxs->offs,&vxs32->vx.vx3[vxs->nv],vxs->no*sizeof(int));
            break;
    }
    sz=sizeofivs2(vxs->nv,0);
    vxs=(ivertices*)realloc(vxs,sz);
    return vxs;
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices* dvs_pack(dvertices* vxs){
int sz;
	if((vxs->no<=0)||(vxs->offs==NULL)){
        return vxs;
	}
	sz=sizeofdvs2(vxs->nv,vxs->no);
	vxs=(dvertices*)realloc(vxs,sz);
	switch(vxs->sign){
		case _2D_VX:
			memmove(&vxs->vx.vx2[vxs->nv],vxs->offs,vxs->no*sizeof(int));
			free(vxs->offs);
			vxs->offs=(int*)&vxs->vx.vx2[vxs->nv];
			break;
		case _3D_VX:
			memmove(&vxs->vx.vx3[vxs->nv],vxs->offs,vxs->no*sizeof(int));
			free(vxs->offs);
			vxs->offs=(int*)&vxs->vx.vx3[vxs->nv];
			break;
	}
    return vxs;
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices* dvs_unpack(dvertices* vxs){
int sz;
	if(vxs->no<=0){
		return vxs;
	}
	vxs->offs=(int*)malloc(vxs->no*sizeof(int));
	switch(vxs->sign){
		case _2D_VX:
			memmove(vxs->offs,&vxs->vx.vx2[vxs->nv],vxs->no*sizeof(int));
			break;
		case _3D_VX:
			memmove(vxs->offs,&vxs->vx.vx3[vxs->nv],vxs->no*sizeof(int));
			break;
	}
	sz=sizeofdvs2(vxs->nv,0);
	vxs=(dvertices*)realloc(vxs,sz);
    return vxs;
}

// ---------------------------------------------------------------------------
// 
// -----------
double deg2rad(double deg){
	return(deg*M_PI/180.0);
}

// ---------------------------------------------------------------------------
// 
// -----------
double rad2deg(double rad){
	return(rad*180.0/M_PI);
}

// ---------------------------------------------------------------------------
// 
// -----------
void vx_i2d(d2dvertex* dvx, i2dvertex* ivx, double reso){
	dvx->x=((double)ivx->h)*reso;
	dvx->y=((double)-ivx->v)*reso;
}

// ---------------------------------------------------------------------------
// 
// -----------
void vx_d2i(i2dvertex* ivx, d2dvertex* dvx, double reso){
	ivx->h=round(dvx->x/reso);
	ivx->v=round(-dvx->y/reso);
}

// ---------------------------------------------------------------------------
// 
// -----------
void vxs_i2d(dvertices** dvxs, ivertices* ivxs, double reso){
int	i;
	(*dvxs)=dvs_new(ivxs->sign,ivxs->nv,ivxs->no);
	if((*dvxs)==NULL){
		return;
	}
	for(i=0;i<(*dvxs)->nv;i++){
		(*dvxs)->vx.vx2[i].x=((double)ivxs->vx.vx2[i].h)*reso;
		(*dvxs)->vx.vx2[i].y=((double)-ivxs->vx.vx2[i].v)*reso;
	}
	if((*dvxs)->offs){
		memmove((*dvxs)->offs,ivxs->offs,ivxs->no*sizeof(int));
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void vxs_d2i(ivertices** ivxs, dvertices* dvxs, double reso){
int	i;
	(*ivxs)=ivs_new(dvxs->sign,dvxs->nv,dvxs->no);
	if((*ivxs)==NULL){
		return;
	}
	for(i=0;i<(*ivxs)->nv;i++){
		(*ivxs)->vx.vx2[i].h=round(dvxs->vx.vx2[i].x/reso);
		(*ivxs)->vx.vx2[i].v=round(-dvxs->vx.vx2[i].y/reso);
	}
	if((*ivxs)->offs){
		memmove((*ivxs)->offs,dvxs->offs,dvxs->no*sizeof(int));
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int ivx2_exists(ivertices* vxs, i2dvertex* vx){
int	i;
	for(i=0;i<vxs->nv;i++){
		if(eq_ivx2(&vxs->vx.vx2[i],vx)){
			return(i+1);
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_sect_inserts(ivertices** vsa, ivertices** vsb){
//fprintf(stderr,"ivs_sect_inserts::begin\n");
//fflush(stderr);
i2dvertex	vx;
int			i,j;
int			na=(*vsa)->nv;
int			nb=(*vsb)->nv;
int			ia,ib;
ivertices*	buf;

	for(i=na-1;i>0;i--){
		for(j=nb-1;j>0;j--){
			ia=0;
			ib=0;
			if(ivx2_sect(	&(*vsa)->vx.vx2[i-1],
							&(*vsa)->vx.vx2[i],
							&(*vsb)->vx.vx2[j-1],
							&(*vsb)->vx.vx2[j])){
				ivx2_get_sect(	&(*vsa)->vx.vx2[i-1],
								&(*vsa)->vx.vx2[i],
								&(*vsb)->vx.vx2[j-1],
								&(*vsb)->vx.vx2[j],
								&vx);
				if(	(!eq_ivx2(&vx,&(*vsa)->vx.vx2[i-1]))	&&
					(!eq_ivx2(&vx,&(*vsa)->vx.vx2[i]))		){
					buf=ivx2_insert(*vsa,&vx,i);
					if(!buf){
fprintf(stderr,"ivs_sect_inserts::ivx2_insert failed for a at %d;%d\n",i,j);
fflush(stderr);
						return;
					}
					*vsa=buf;
					ia=1;
				}
				if(	(!eq_ivx2(&vx,&(*vsb)->vx.vx2[j-1]))	&&
					(!eq_ivx2(&vx,&(*vsb)->vx.vx2[j]))		){
					buf=ivx2_insert(*vsb,&vx,j);
					if(!buf){
fprintf(stderr,"ivs_sect_inserts::ivx2_insert failed for b at %d;%d\n",i,j);
fflush(stderr);
						return;
					}
					*vsb=buf;
					ib=1;
				}
				if((ib)&&(j<nb-1)){
					j++;
				}
			}
			if((ia)&&(i<na-1)){
				i++;
			}
		}
	}
//fprintf(stderr,"ivs_sect_inserts::end\n");
//fflush(stderr);
}

// ---------------------------------------------------------------------------
// 
// -----------
void ivs_shift_origin(ivertices* vxs, int o){
	if(ivs_n_parts(vxs)>1){
fprintf(stderr,"ivs_shift_origin::nparts!=1 (%d)\n",ivs_n_parts(vxs));
		return;
	}
	if((o<=0)||(o>=vxs->nv)){
fprintf(stderr,"ivs_shift_origin::bad origin (%d for %d)\n",o,vxs->nv);
		return;
	}
int			c=ivs_closed(vxs)?1:0;
int			n=c?vxs->nv-1:vxs->nv;
ivertices*	buffer;
	if(ivs2ivs(vxs,&buffer)){
fprintf(stderr,"ivs_shift_origin::ivs2ivs failed\n");
		return;
	}
	switch(vxs->sign){
		case _2D_VX:
			memmove(&vxs->vx.vx2[0],&buffer->vx.vx2[o],(n-o)*sizeof(i2dvertex));
			memmove(&vxs->vx.vx2[n-o],&buffer->vx.vx2[0],o*sizeof(i2dvertex));
			if(c){
				vxs->vx.vx2[vxs->nv-1]=vxs->vx.vx2[0];
			}
			break;
		case _3D_VX:
			memmove(&vxs->vx.vx3[0],&buffer->vx.vx3[o],(n-o)*sizeof(i3dvertex));
			memmove(&vxs->vx.vx3[vxs->nv-o],&buffer->vx.vx3[c],(o-c)*sizeof(i3dvertex));
			if(c){
				vxs->vx.vx3[vxs->nv-1]=vxs->vx.vx3[0];
			}
			break;
	}	
	ivs_free(buffer);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* text2ivs(int* kind, double reso, char* txt){
int			i,n=strlen(txt);
int			code=kGeomNotInited;
int			nested=0;
int			start=-1;
char		buf[256];
char*		str;
d2dvertex	dvx;
i2dvertex	ivx;
ivertices*	cur=NULL;
ivertices*	res=NULL;

	str=strstr(txt,kTagPolygon);
	if(str){
		*kind=kVXPolygon;
	}
	if(str==NULL){
		str=strstr(txt,kTagLinestring);
		if(str){
			*kind=kVXPolyline;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagPoint);
		if(str){
			*kind=kVXPoint;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagText);
		if(str){
			*kind=kVXText;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagRaster);
		if(str){
			*kind=kVXRaster;
		}	
	}
	
	if(str==NULL){
fprintf(stderr,"text2ivs::No kind\n");
		//ivs_free(res);
		return(NULL);
	}
	
	for(i=0;i<n;i++){
		if(code==kGeomEnd){
fprintf(stderr,"text2ivs::Malformed, remaining char at (code==kGeomEnd)\n");
			ivs_free(res);
			if(cur){
				ivs_free(cur);
			}
			return(NULL);
		}
		switch(str[i]){
			case '(':
				nested++;
				if(nested>2){
fprintf(stderr,"text2ivs::Malformed, (nested>2)\n");
					ivs_free(res);
					if(cur){
						ivs_free(cur);
					}
					return(NULL);
				}
				if(nested==1){
					code=kGeomBegin;
				}
				else if(nested==2){
					code=kCoordX;
					cur=ivs_new(_2D_VX,0,0);
				}
				break;
			case ')':
				nested--;
				if(nested<0){
fprintf(stderr,"text2ivs::Malformed, (nested<0)\n");
					ivs_free(res);
					if(cur){
						ivs_free(cur);
					}
				}
				else if(nested==0){
					code=kGeomEnd;
					start=-1;
					//res=ivs_group(res,cur);
					//ivs_free(cur);
					//cur=NULL;
				}
				else if(nested==1){
					if(code!=kCoordY){
fprintf(stderr,"text2ivs::Malformed, (code!=kCoordY)\n");
						ivs_free(res);
						if(cur){
							ivs_free(cur);
						}
					}
					if(start!=-1){
						if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
							ivs_free(res);
							if(cur){
								ivs_free(cur);
							}
							return(NULL);
						}
						memcpy(buf,&str[start],i-start);
						buf[i-start]=0;
						dvx.y=atof(buf);
						vx_d2i(&ivx,&dvx,reso);
						cur=ivx2_add(cur,&ivx);
						start=-1;
						code=kPartEnd;
						if(res==NULL){
							ivs2ivs(cur,&res);
						}
						else{
							res=ivs_group(res,cur);
						}
					}
					ivs_free(cur);
					cur=NULL;
				}
				break;
			case ',':
				if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
					ivs_free(res);
					if(cur){
						ivs_free(cur);
					}
					return(NULL);
				}
				memcpy(buf,&str[start],i-start);
				buf[i-start]=0;
				dvx.y=atof(buf);
				vx_d2i(&ivx,&dvx,reso);
				cur=ivx2_add(cur,&ivx);	
				code=kCoordX;
				start=-1;
				break;
			case ' ':
				if(start==-1){
					break;
				}
				if(code!=kCoordX){
fprintf(stderr,"text2ivs::Malformed, (code!=kCoordX)\n");
					ivs_free(res);
					if(cur){
						ivs_free(cur);
					}
					return(NULL);
				}
				if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
					ivs_free(res);
					if(cur){
						ivs_free(cur);
					}
					return(NULL);
				}
				memcpy(buf,&str[start],i-start);
				buf[i-start]=0;
				dvx.x=atof(buf);
				code=kCoordY;
				start=-1;
				break;
			default:
				if(nested==2){
					if(start==-1){
						start=i;
					}
				}
				break;
		}
	}
    return(res);
}

// ---------------------------------------------------------------------------
// 
// -----------
char* ivs2text(int kind, double reso, ivertices* vxs){
char		buf[256];
char*		res;
int			i,j,np,n=ivs_n_parts(vxs);
i2dvertex*	p;
d2dvertex	dvx;

	switch(kind){
		case kVXPolygon:
			res=strdup(kTagPolygon);
			break;
		case kVXPolyline:
			res=strdup(kTagLinestring);
			break;
		case kVXPoint:
			res=strdup(kTagPoint);
			break;
		case kVXRaster:
			res=strdup(kTagRaster);
			break;
		case kVXText:
			res=strdup(kTagText);
			break;
		default:
			return(NULL);
			break;
	}
	
	res=strcatr(res,"(");
	for(i=0;i<n;i++){
		res=strcatr(res,"(");
		p=ivs2_part(vxs,i,&np);
		for(j=0;j<np;j++){
			vx_i2d(&dvx,&p[j],reso);
			if(j<np-1){
				sprintf(buf,"%f %f, ",dvx.x,dvx.y);
			}
			else{
				sprintf(buf,"%f %f",dvx.x,dvx.y);
			}
			res=strcatr(res,buf);
		}
		res=strcatr(res,")");
	}
	res=strcatr(res,")");
	
	return(res);
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices* text2dvs(int* kind, char* txt){
int			i,n=strlen(txt);
int			code=kGeomNotInited;
int			nested=0;
int			start=-1;
char		buf[256];
char*		str;
d2dvertex	dvx;
//d2dvertex	ivx;
dvertices*	cur=NULL;
dvertices*	res=NULL;

	str=strstr(txt,kTagPolygon);
	if(str){
		*kind=kVXPolygon;
	}
	if(str==NULL){
		str=strstr(txt,kTagLinestring);
		if(str){
			*kind=kVXPolyline;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagPoint);
		if(str){
			*kind=kVXPoint;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagText);
		if(str){
			*kind=kVXText;
		}	
	}
	if(str==NULL){
		str=strstr(txt,kTagRaster);
		if(str){
			*kind=kVXRaster;
		}	
	}
	
	if(str==NULL){
fprintf(stderr,"text2ivs::No kind\n");
		return(NULL);
	}
	
	for(i=0;i<n;i++){
		if(code==kGeomEnd){
fprintf(stderr,"text2ivs::Malformed, remaining char at (code==kGeomEnd)\n");
			dvs_free(res);
			if(cur){
				dvs_free(cur);
			}
			return(NULL);
		}
		switch(str[i]){
			case '(':
				nested++;
				if(nested>2){
fprintf(stderr,"text2ivs::Malformed, (nested>2)\n");
					dvs_free(res);
					if(cur){
						dvs_free(cur);
					}
					return(NULL);
				}
				if(nested==1){
					code=kGeomBegin;
				}
				else if(nested==2){
					code=kCoordX;
					cur=dvs_new(_2D_VX,0,0);
				}
				break;
			case ')':
				nested--;
				if(nested<0){
fprintf(stderr,"text2ivs::Malformed, (nested<0)\n");
					dvs_free(res);
					if(cur){
						dvs_free(cur);
					}
				}
				if(nested==0){
					code=kGeomEnd;
					start=-1;
					//res=ivs_group(res,cur);
					//ivs_free(cur);
					//cur=NULL;
				}
				else if(nested==1){
					if(code!=kCoordY){
fprintf(stderr,"text2ivs::Malformed, (code!=kCoordY)\n");
						dvs_free(res);
						if(cur){
							dvs_free(cur);
						}
					}
					if(start!=-1){
						if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
							dvs_free(res);
							if(cur){
								dvs_free(cur);
							}
							return(NULL);
						}
						memcpy(buf,&str[start],i-start);
						buf[i-start]=0;
						dvx.y=atof(buf);
						//vx_d2i(&ivx,&dvx,reso);
						cur=dvx2_add(cur,&dvx);
						start=-1;
						code=kPartEnd;
						if(res==NULL){
							dvs2dvs(cur,&res);
						}
						else{
							res=dvs_group(res,cur);
						}
					}					
					dvs_free(cur);
					cur=NULL;
				}
				break;
			case ',':
				if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
					dvs_free(res);
					if(cur){
						dvs_free(cur);
					}
					return(NULL);
				}
				memcpy(buf,&str[start],i-start);
				buf[i-start]=0;
				dvx.y=atof(buf);
				//vx_d2i(&ivx,&dvx,reso);
				cur=dvx2_add(cur,&dvx);	
				code=kCoordX;
				start=-1;
				break;
			case ' ':
				if(start==-1){
					break;
				}
				if(code!=kCoordX){
fprintf(stderr,"text2ivs::Malformed, (code!=kCoordX)\n");
					dvs_free(res);
					if(cur){
						dvs_free(cur);
					}
					return(NULL);
				}
				if(i-start>=sizeof(buf)){
fprintf(stderr,"text2ivs::size out of range\n");
					dvs_free(res);
					if(cur){
						dvs_free(cur);
					}
					return(NULL);
				}
				memcpy(buf,&str[start],i-start);
				buf[i-start]=0;
				dvx.x=atof(buf);
				code=kCoordY;
				start=-1;
				break;
			default:
				if(nested==2){
					if(start==-1){
						start=i;
					}
				}
				break;
		}
	}
	return(res);
}

// ---------------------------------------------------------------------------
// 
// -----------
char* dvs2text(int kind, dvertices* vxs){
char		buf[256];
char*		res;
int			i,j,np,n=dvs_n_parts(vxs);
d2dvertex*	p;

	switch(kind){
		case kVXPolygon:
			res=strdup(kTagPolygon);
			break;
		case kVXPolyline:
			res=strdup(kTagLinestring);
			break;
		case kVXPoint:
			res=strdup(kTagPoint);
			break;
		case kVXRaster:
			res=strdup(kTagRaster);
			break;
		case kVXText:
			res=strdup(kTagText);
			break;
		default:
			return(NULL);
			break;
	}
	
	res=strcatr(res,"(");
	for(i=0;i<n;i++){
		res=strcatr(res,"(");
		p=dvs2_part(vxs,i,&np);
		for(j=0;j<np;j++){
			if(j<np-1){
				sprintf(buf,"%f %f, ",p[j].x,p[j].y);
			}
			else{
				sprintf(buf,"%f %f",p[j].x,p[j].y);
			}
			res=strcatr(res,buf);
		}
		res=strcatr(res,")");
	}
	res=strcatr(res,")");
	
	return(res);
}


/*
// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	ivs_group(ivertices* vsa, ivertices* vsb){
int			sz,first;
ivertices*	rvxs;
int*		buff;

	if(vsa->sign!=vsb->sign){
		return(NULL);
	}
//fprintf(stderr,"ivs_group: before realloc -> vsa->no==%d, vsb->no==%d\n",vsa->no,vsb->no);
	if(vsa->sign==_2D_VX){
		if(	(eq_ivx2(	&vsa->vx.vx2[vsa->nv-1],
						&vsb->vx.vx2[0]))		&&(
			(vsa->no==0)						&&
			(vsb->no==0)						)){
			first=1;
		}
		else{
			first=0;
		}
//fprintf(stderr,"ivs_group:first=%d\n",first);
		if((first)&&(vsb->nv==1)){
fprintf(stderr,"ivs_group:(first)&&(vsb->nv==1)\n");
			return(NULL);
			//return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(i2dvertex);
//fprintf(stderr,"ivs_group:vsa->nv=%d, vsb->nv=%d, sz=%d\n",vsa->nv,vsb->nv,sz);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&(rvxs->vx.vx2[rvxs->nv]),
						&(vsb->vx.vx2[first]),
						(vsb->nv-first)*sizeof(i2dvertex));
		rvxs->nv+=(vsb->nv-first);
	}
	else{
		if(	(eq_ivx3(&vsa->vx.vx3[vsa->nv-1],
					&vsb->vx.vx3[0]))		&&(
			(vsa->no==0)					&&
			(vsb->no==0)					)){
			first=1;
		}
		else{
			first=0;
		}
		if((first)&&(vsb->nv==1)){
fprintf(stderr,"ivs_group:(first)&&(vsb->nv==1)\n");
			return(NULL);
			//return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(i3dvertex);
		rvxs=(ivertices*)realloc(vsa,(sizeof(ivertices)-sizeof(i3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx3[rvxs->nv],
						&vsb->vx.vx2[first],
						(vsb->nv-first)*sizeof(i3dvertex));
		rvxs->nv+=(vsb->nv-first);
	}

//fprintf(stderr,"ivs_group: after realloc -> vsa->no==%d, vsb->no==%d\n",vsa->no,vsb->no);
	
	if((rvxs->no==0)&&(vsb->no==0)){
//TEST OK
fprintf(stderr,"ivs_group:cas 1\n");
		if(rvxs->nv<2){
			return(rvxs);
		}
		rvxs->offs=(int*)malloc(2*sizeof(int));
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs->offs=(int*)malloc(2*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no=2;
		rvxs->offs[0]=0;
		rvxs->offs[1]=rvxs->nv-vsb->nv-first;
	}
	else if((rvxs->no>0)&&(vsb->no>0)){
fprintf(stderr,"ivs_group:cas 2\n");
		first=rvxs->no;
		rvxs->no+=vsb->no;
		buff=(int*)realloc(rvxs->offs,rvxs->no*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)realloc(rvxs->offs,rvxs->no*sizeof(int))\n");
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[first],vsb->offs,(vsb->no)*sizeof(int));
	}
	else if((rvxs->no>0)&&(vsb->no==0)){
fprintf(stderr,"ivs_group:cas 3\n");
		buff=(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no+=1;
		rvxs->offs=buff;
		rvxs->offs[rvxs->no-1]=rvxs->nv-vsb->nv-first;
	}
	else if((rvxs->no==0)&&(vsb->no>0)){
fprintf(stderr,"ivs_group:cas 4\n");
		rvxs->no=vsb->no;
		buff=(int*)malloc(rvxs->no*sizeof(int));
		if(!buff){
fprintf(stderr,"ivs_group:(int*)malloc(rvxs->no*sizeof(int))\n");
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(rvxs->offs,vsb->offs,(vsb->no)*sizeof(int));
	}
	else{
fprintf(stderr,"ivs_group : ????\n");
	}
	return(rvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices*	dvs_group(dvertices* vsa, dvertices* vsb){
int			sz,first;
dvertices*	rvxs;
int*		buff;

	if(vsa->sign!=vsb->sign){
		return(NULL);
	}
//fprintf(stderr,"dvs_group: before realloc -> vsa->no==%d, vsb->no==%d\n",vsa->no,vsb->no);
	if(vsa->sign==_2D_VX){
		if(	(eq_dvx2(	&vsa->vx.vx2[vsa->nv-1],
						&vsb->vx.vx2[0]))		&&(
			(vsa->no==0)						&&
			(vsb->no==0)						)){
			first=1;
		}
		else{
			first=0;
		}
//fprintf(stderr,"dvs_group:first=%d\n",first);
		if((first)&&(vsb->nv==1)){
fprintf(stderr,"dvs_group:(first)&&(vsb->nv==1)\n");
			return(NULL);
			//return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(d2dvertex);
//fprintf(stderr,"dvs_group:vsa->nv=%d, vsb->nv=%d, sz=%d\n",vsa->nv,vsb->nv,sz);
		rvxs=(dvertices*)realloc(vsa,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"dvs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&(rvxs->vx.vx2[rvxs->nv]),
						&(vsb->vx.vx2[first]),
						(vsb->nv-first)*sizeof(d2dvertex));
		rvxs->nv+=(vsb->nv-first);
	}
	else{
		if(	(eq_dvx3(&vsa->vx.vx3[vsa->nv-1],
					&vsb->vx.vx3[0]))		&&(
			(vsa->no==0)					&&
			(vsb->no==0)					)){
			first=1;
		}
		else{
			first=0;
		}
		if((first)&&(vsb->nv==1)){
fprintf(stderr,"ivs_group:(first)&&(vsb->nv==1)\n");
			return(NULL);
			//return(vsa);
		}
		sz=(vsa->nv+vsb->nv-first)*sizeof(d3dvertex);
		rvxs=(dvertices*)realloc(vsa,(sizeof(dvertices)-sizeof(d3dvertex))+sz);
		if(!rvxs){
fprintf(stderr,"ivs_group:rvxs==NULL\n");
			return(NULL);
		}
		(void)memmove(	&rvxs->vx.vx3[rvxs->nv],
						&vsb->vx.vx2[first],
						(vsb->nv-first)*sizeof(d3dvertex));
		rvxs->nv+=(vsb->nv-first);
	}

//fprintf(stderr,"dvs_group: after realloc -> vsa->no==%d, vsb->no==%d\n",vsa->no,vsb->no);
	
	if((rvxs->no==0)&&(vsb->no==0)){
//TEST OK
fprintf(stderr,"dvs_group:cas 1\n");
		if(rvxs->nv<2){
			return(rvxs);
		}
		rvxs->offs=(int*)malloc(2*sizeof(int));
		if(!rvxs){
fprintf(stderr,"dvs_group:rvxs->offs=(int*)malloc(2*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no=2;
		rvxs->offs[0]=0;
		rvxs->offs[1]=rvxs->nv-vsb->nv-first;
	}
	else if((rvxs->no>0)&&(vsb->no>0)){
fprintf(stderr,"ivs_group:cas 2\n");
		first=rvxs->no;
		rvxs->no+=vsb->no;
		buff=(int*)realloc(rvxs->offs,rvxs->no*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)realloc(rvxs->offs,rvxs->no*sizeof(int))\n");
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(&rvxs->offs[first],vsb->offs,(vsb->no)*sizeof(int));
	}
	else if((rvxs->no>0)&&(vsb->no==0)){
fprintf(stderr,"ivs_group:cas 3\n");
		buff=(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)realloc(rvxs->offs,(rvxs->no+1)*sizeof(int))\n");
			return(NULL);
		}
		rvxs->no+=1;
		rvxs->offs=buff;
		rvxs->offs[rvxs->no-1]=rvxs->nv-vsb->nv-first;
	}
	else if((rvxs->no==0)&&(vsb->no>0)){
fprintf(stderr,"dvs_group:cas 4\n");
		rvxs->no=vsb->no;
		buff=(int*)malloc(rvxs->no*sizeof(int));
		if(!buff){
fprintf(stderr,"dvs_group:(int*)malloc(rvxs->no*sizeof(int))\n");
			return(NULL);
		}
		rvxs->offs=buff;
		(void)memmove(rvxs->offs,vsb->offs,(vsb->no)*sizeof(int));
	}
	else{
fprintf(stderr,"dvs_group : ????\n");
	}
	return(rvxs);
}

*/
