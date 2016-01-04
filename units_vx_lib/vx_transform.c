//----------------------------------------------------------------------------
// File : vx_transform.c
// Project : MacMapSuite
// Purpose : C source file : Geometry transformation
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
// 03/08/2010 creation.
//----------------------------------------------------------------------------

#include "vx_transform.h"
#include "vx_measure.h"
#include "vx_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// ---------------------------------------------------------------------------
// 
// -----------
typedef int		MatPivT[5+1];
typedef double	MatNormT[15+1];
typedef double	TT[5+1];
#define sqr(x)	((x)*(x))

// ---------------------------------------------------------------------------
// 
// -----------
static void Cholelin(MatNormT A, 
					 MatPivT P, 
					 int N, 
					 TT S){
int		i,j,k;
double	X;
	
	A[P[1]]=sqrt(A[P[1]]);
	for(j=2;j<=N+1;j++){
		if(P[j]-P[j-1]>j-1){
			A[P[j]+1-j]=A[P[j]+1-j]/A[P[1]];
		}
	}
	for(i=2;i<=N+1;i++){
		X=A[P[i]];
		for(k=1;k<=i-1;k++){
			if(P[i]-P[i-1]>i-k){
				X=X-sqr(A[P[i]+k-i]);
			}
		}
		if(X<=0){
			X=1e4932;
		}
		A[P[i]]=sqrt(X);
		for(j=i+1;j<=N+1;j++){
			if(P[j]-P[j-1]>j-1){
				X=A[P[j]+i-j];
				for(k=1;k<=i-1;k++){
					if((P[i]-P[i-1]>i-k)&&(P[j]-P[j-1]>j-k)){
						X=X-A[P[i]+k-i]*A[P[j]+k-j];
					}
				}
				A[P[j]+i-j]=X/A[P[i]];
			}
		}
	}
	for(i=N;i>=1;i--){
		if(P[N+1]-P[N]>N+1-i){
			X=A[P[N+1]+i-N-1];
		}
		else{
			X=0;
		}
		for(j=i+1;j<=N;j++){
			if(P[j]-P[j-1]>j-i){
				X=X-S[j]*A[P[j]+i-j];
			}
		}
		S[i]=X/A[P[i]];
	}
	S[N+1]=sqr(A[P[N+1]]);
}

// ---------------------------------------------------------------------------
// 
// -----------
void SIMIL_Init(	SIMILPrm* prm, 
					dvertices* fich, 
					dvertices* click){
MatPivT		MatPiv;
MatNormT	MatNorm;
TT			T;
double		Moy,Somme,Somme2,tempReal,tpX,tpY;
int			i;
	
	MatPiv[1]=1;
	MatPiv[2]=3;
	MatPiv[3]=6;
	MatPiv[4]=10;
	MatPiv[5]=15;
	for(i=1;i<=15;i++){
		MatNorm[i]=0;
	}
	for(i=1;i<=5;i++){
		T[i]=0;
	}
	for(i=1;i<=fich->nv;i++){
		MatNorm[1]=MatNorm[1]+1;
		MatNorm[2]=0;
		MatNorm[3]=MatNorm[3]+1;
		MatNorm[4]=MatNorm[4]+fich->vx.vx2[i-1].x;
		MatNorm[5]=MatNorm[5]+fich->vx.vx2[i-1].y;
		MatNorm[6]=MatNorm[6]+sqr(fich->vx.vx2[i-1].x*1.0)+sqr(fich->vx.vx2[i-1].y*1.0);
		MatNorm[7]=MatNorm[7]+fich->vx.vx2[i-1].y;
		MatNorm[8]=MatNorm[8]-fich->vx.vx2[i-1].x;
		MatNorm[9]=0;
		MatNorm[10]=MatNorm[10]+sqr(fich->vx.vx2[i-1].x*1.0)+sqr(fich->vx.vx2[i-1].y*1.0);
		MatNorm[11]=MatNorm[11]+click->vx.vx2[i-1].x;
		MatNorm[12]=MatNorm[12]+click->vx.vx2[i-1].y;
		MatNorm[13]=MatNorm[13]+fich->vx.vx2[i-1].x*1.0*click->vx.vx2[i-1].x+fich->vx.vx2[i-1].y*1.0*click->vx.vx2[i-1].y;
		MatNorm[14]=MatNorm[14]+fich->vx.vx2[i-1].y*1.0*click->vx.vx2[i-1].x-fich->vx.vx2[i-1].x*1.0*click->vx.vx2[i-1].y;
		MatNorm[15]=MatNorm[15]+sqr(click->vx.vx2[i-1].x*1.0)+sqr(click->vx.vx2[i-1].y*1.0);
	}
	
	Cholelin(MatNorm,MatPiv,4,T);
	
	prm->Tx=round(T[1]);
	prm->Ty=round(T[2]);
	prm->R[1]=T[3];
	prm->R[2]=T[4];
	prm->R[3]=1;
	prm->R[4]=1;
	prm->R[5]=T[1];
	prm->R[6]=T[2];
	
	Somme=0;
	Somme2=0;
	for(i=1;i<=fich->nv;i++){
		tpX=T[1]+T[3]*fich->vx.vx2[i-1].x+T[4]*fich->vx.vx2[i-1].y;
		tpY=T[2]-T[4]*fich->vx.vx2[i-1].x+T[3]*fich->vx.vx2[i-1].y;
		tempReal=sqr(click->vx.vx2[i-1].x-tpX)+sqr(click->vx.vx2[i-1].y-tpY);
		Somme+=sqrt(tempReal);
		Somme2+=tempReal;
	}
	prm->EchelleX=1000000*sqrt(sqr(T[3])+sqr(T[4]));
	prm->EchelleY=prm->EchelleX;
	Moy=Somme/(double)fich->nv;
	prm->ResiduX=sqrt(Somme2/(double)fich->nv);
	prm->ResiduY=prm->ResiduX;
}

// ---------------------------------------------------------------------------
// 
// -----------
void SIMIL_Transform(	d2dvertex* fich, 
						d2dvertex* click, 
						SIMILPrm* prm){
	fich->x=(prm->R[5]+prm->R[1]*click->x+prm->R[2]*click->y);
	fich->y=(prm->R[6]-prm->R[2]*click->x+prm->R[1]*click->y);
}
