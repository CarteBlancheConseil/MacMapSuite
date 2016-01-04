//----------------------------------------------------------------------------
// File : PGis_Utils.c
// Project : MacMapSuite
// Purpose : C source file : Postgis utils
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
// 29/06/2005 creation.
//----------------------------------------------------------------------------

#include "PGis_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// 
// -----------
char outchr[]={"0123456789ABCDEF"};
int endian_check_int=1;

// ---------------------------------------------------------------------------
// byte order swapping
// -----------
static void	swapword(int sz, void* word){
int				i;
unsigned char	buff;

	for(i=0;i<sz/2;i++){
		buff=((unsigned char*)word)[i];
		((unsigned char*)word)[i]=((unsigned char*)word)[sz-i-1];
		((unsigned char*)word)[sz-i-1]=buff;
	}
}

// ---------------------------------------------------------------------------
// Point in polygon
// -----------
int PIP(gPoint P, gPoint* V, int n){
int i,cn=0;														// the crossing number counter

    for(i=0;i<n-1;i++){											// edge from V[i] to V[i+1]
       if (	((V[i].y<=P.y)&&(V[i+1].y>P.y)) ||					// an upward crossing
			((V[i].y>P.y)&&(V[i+1].y<=P.y))){					// a downward crossing
            double vt=(float)(P.y-V[i].y)/(V[i+1].y-V[i].y);
            if(P.x<V[i].x+vt*(V[i+1].x-V[i].x)){				// P.x < intersect
                ++cn;											// a valid crossing of y=P.y right of P.x
			}
        }
    }
    return(cn&1); // 0 if even (out), and 1 if odd (in)
}

// ---------------------------------------------------------------------------
// Endian check : 0 = big endian, 1 = little endian
// -----------
char getEndianByte(void){
	if(*((char*)&endian_check_int)){
		return(1);
	}
	else{
		return(0);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void skipbyte(byte **c) {
	*c+=1;
}

// ---------------------------------------------------------------------------
// 
// -----------
byte getbyte(byte *c) {
	return *((char*)c);
}

// ---------------------------------------------------------------------------
// 
// -----------
byte popbyte(byte **c){
	return *((*c)++);
}

// ---------------------------------------------------------------------------
// 
// -----------
UInt32 popint(byte **c, char swap){
UInt32 i;
	memcpy(&i,*c,4);
	*c+=4;
	if(swap){
		swapword(4,&i);
	}
	return(i);
}

// ---------------------------------------------------------------------------
// 
// -----------
UInt32 getint(byte *c) {
UInt32 i;
	memcpy(&i,c,4);
	return(i);
}

// ---------------------------------------------------------------------------
// 
// -----------
void skipint(byte **c){
	*c+=4;
}

// ---------------------------------------------------------------------------
// 
// -----------
double popdouble(byte **c, char swap){
double d;
	memcpy(&d,*c,8);
	*c+=8;
	if(swap){
		swapword(8,&d);
	}
	return(d);
}

// ---------------------------------------------------------------------------
// 
// -----------
void skipdouble(byte **c){
	*c+=8;
}

// ---------------------------------------------------------------------------
// Input : a NULL-terminated string, Output : a binary string 
// -----------
byte* HexDecode(byte *hex){
byte	*ret,*retptr,*hexptr;
byte	byt;
int		len;

//fprintf(stderr,(char*)hex);
//fprintf(stderr,"\n");

	len=strlen((char*)hex)/2;
	ret=(byte*)malloc(len);
	if(!ret){
		return(NULL);
	}
	hexptr=hex;
	retptr=ret;

// skip srid
	if(hexptr[0]=='S'){
		hexptr=(byte*)strchr((char*)hexptr,';');
		hexptr++;
	}

	while(*hexptr){
// All these checks on WKB correctness
// can be avoided, are only here because
// I keep getting segfaults whereas
// bytea unescaping works fine...
		if(*hexptr<58&&*hexptr>47){
			byt=(((*hexptr)-48)<<4);
		}
		else if(*hexptr>64&&*hexptr<71){
			byt=(((*hexptr)-55)<<4);
		}
		else{
			free(ret);
			return(NULL);
		}
		hexptr++;

		if(*hexptr<58&&*hexptr>47){
			byt|=((*hexptr)-48);
		}
		else if(*hexptr>64&&*hexptr<71){
			byt|=((*hexptr)-55);
		}
		else{
			free(ret);
			return(NULL);
		}
		hexptr++;
		*retptr=(byte)byt;
		retptr++;
	}
	return(ret);
}

// ---------------------------------------------------------------------------
// 
// -----------
int reverse_points(	int num_points,
					double* x, 
					double* y, 
					double* z, 
					double* m){
int i,j;
double temp;
	j=num_points-1;
	for(i=0;i<num_points;i++){
		if(j<=i){
			break;
		}
		temp=x[j];
		x[j]=x[i];
		x[i]=temp;

		temp=y[j];
		y[j]=y[i];
		y[i]=temp;

		if(z){
			temp=z[j];
			z[j]=z[i];
			z[i]=temp;
		}

		if(m){
			temp=m[j];
			m[j]=m[i];
			m[i]=temp;
		}
		j--;
	}
	return(1);
}

// ---------------------------------------------------------------------------
// 1 = counter-clockwise, 0 = clockwise
// -----------
int is_clockwise(	int num_points,
					double* x,
					double* y){
int		i;
double	x_change,y_change,area;
double	*x_new,*y_new; //the points, translated to the origin for safer accuracy

	x_new=(double*)malloc(sizeof(double)*num_points);	
	y_new=(double*)malloc(sizeof(double)*num_points);	
	area=0.0;
	x_change=x[0];
	y_change=y[0];

	for(i=0;i< num_points;i++){
		x_new[i]=x[i]-x_change;
		y_new[i]=y[i]-y_change;
	}

	for(i=0;i<num_points-1;i++){
		area+=(x[i]*y[i+1])-(y[i]*x[i+1]);//calculate the area	
	}
	
	free(x_new);
	free(y_new);
	
	return((area>0)?0:1);
}

