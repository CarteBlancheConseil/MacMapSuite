//----------------------------------------------------------------------------
// File : NMEAParser.cpp
// Project : MacMapSuite
// Purpose : CPP source file : NMEA parser utils
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
// 19/05/09 creation.
//----------------------------------------------------------------------------

#include "NMEAParser.h"
#include "bTrace.h"

// ---------------------------------------------------------------------------
// 
// ------------
static time_t parse_time(char* str){
char	sp[12];

time_t		t=time(NULL);
struct tm	ts=*localtime(&t);
	
	t=0;
	ts.tm_hour=0;
	ts.tm_min=0;
	ts.tm_sec=0;
	
	sp[2]=0;
	strncpy(sp,str,2);
	ts.tm_hour=atoi(sp);
	
	strncpy(sp,&str[2],2);
	ts.tm_min=atoi(sp);

	strncpy(sp,&str[4],2);
	ts.tm_sec=atoi(sp);

	t=mktime(&ts);
	return(t);
}

// ---------------------------------------------------------------------------
// 
// ------------
static double dms2dec(double d, double m, double s){
	return(d+(m+s/60.0)/60.0);
}

// ---------------------------------------------------------------------------
// 
// ------------
static double parse_xory(char* str){
double	x=atof(str);
double	d=(int)(x/100.0);
double	m=(int)(((x/100.0)-d)*100.0);
double	s=(x-((int)(x)))*60.0;
	return(dms2dec(d,m,s));
}

// ---------------------------------------------------------------------------
// 
// ------------
static int parse_NorE(char* str){
	if((str[0]=='N')||(str[0]=='E')){
		return(1);
	}
	if((str[0]=='S')||(str[0]=='W')){
		return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// ------------
int parse_GPGGA(char* str, GPGGA* tag){
char*	p=strstr(str,"$GPGGA");

	tag->sign=kTagUndef;
	if(p==NULL){
		return(-1);
	}
	tag->sign=kTagGPGGA;
	
int		k=0;
char*	adr=strtok(str,",");
	while(adr){
		k++;
//fprintf(stderr,"%d->%s\n",k,adr);
		switch(k){
			case 2:
				tag->hms=parse_time(adr);
				break;
			case 3:
				tag->lat=parse_xory(adr);
				break;
			case 4:
				tag->lat*=(double)parse_NorE(adr);
				break;
			case 5:
				tag->lng=parse_xory(adr);
				break;
			case 6:
				tag->lng*=(double)parse_NorE(adr);
				break;
			case 7:
				tag->mk=atoi(adr);
				break;
			case 8:
				tag->nsat=atoi(adr);
				break;
			case 9:
				tag->hdop=atof(adr);
				break;
			case 10:
				tag->alt=atof(adr);
				break;
			case 11:
				strcpy(tag->ua,adr);
				break;		
		}
		adr=strtok(NULL,",");
	}
	return(0);
}

