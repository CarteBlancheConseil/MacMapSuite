//----------------------------------------------------------------------------
// File : db_utils.c
// Project : MacMapSuite
// Purpose : C source file : DB_Lib utils
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
// 18/08/05 creation.
//----------------------------------------------------------------------------

#include <string.h>
#include <time.h>
#include "db_types.h"
#include "db_utils.h"

//----------------------------------------------------------------------------
// 
//------------
int SignTest(int sign){
	switch(sign){
		case _bool:
		case _int:
		case _date:
		case _time:
		case _double:
		case _ivx2:
		case _ivx3:
		case _ivxrect:
		case _ivxcube:
		case _ivxs2:
		case _ivxs3:
		case _dvx2:
		case _dvx3:
		case _dvxrect:
		case _dvxcube:
		case _dvxs2:
		case _dvxs3:
		case _char:
		case _binary:
			return(1);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
//
// ------------
time_t make_time(char* str){
char		x[256];
time_t		t=time(NULL);
struct tm	ts=*localtime(&t);
	
	t=0;
	ts.tm_hour=0;
	ts.tm_min=0;
	ts.tm_sec=0;

	strncpy(x,str,256);

int		k=0;
char*	adr=strtok(x,":");
	while(adr){
		k++;
		switch(k){
			case 1:
				ts.tm_hour=atoi(adr);
				break;
			case 2:
				ts.tm_min=atoi(adr);
				break;
			case 3:
				ts.tm_sec=atoi(adr);
				break;
		}
		adr=strtok(NULL,":");
	}
	t=mktime(&ts);
	return(t);
}

// ---------------------------------------------------------------------------
//
// ------------
time_t make_date(char* str){
char		x[256];
time_t		t=time(NULL);
struct tm	ts=*localtime(&t);
	
	t=0;
	ts.tm_year=0;
	ts.tm_mon=0;
	ts.tm_mday=1;

	strncpy(x,str,256);

int		k=0;
char*	adr=strtok(x,"-");
	while(adr){
		k++;
		switch(k){
			case 1:
				ts.tm_year=atoi(adr)-1900;
				break;
			case 2:
				ts.tm_mon=atoi(adr)-1;
				break;
			case 3:
				ts.tm_mday=atoi(adr);
				break;
		}
		adr=strtok(NULL,"-");
	}
	t=mktime(&ts);
	return(t);
}

// ---------------------------------------------------------------------------
//
// ------------
time_t make_datetime(char* str){
char		x[256],xd[256]="",xt[256]="";
time_t		t=time(NULL);
struct tm	ts=*localtime(&t);
	
	t=0;
	ts.tm_year=0;
	ts.tm_mon=0;
	ts.tm_mday=1;
	ts.tm_hour=0;
	ts.tm_min=0;
	ts.tm_sec=0;

	strncpy(x,str,256);
int		k=0;	
char*	adr=strtok(x," ");
	while(adr){
		k++;
		switch(k){
			case 1:
				strncpy(xd,adr,256);
				break;
			case 2:
				strncpy(xt,adr,256);
				break;
		}
		adr=strtok(NULL," ");
	}
	
	k=0;
	adr=strtok(xd,"-");
	while(adr){
		k++;
		switch(k){
			case 1:
				ts.tm_year=atoi(adr)-1900;
				break;
			case 2:
				ts.tm_mon=atoi(adr)-1;
				break;
			case 3:
				ts.tm_mday=atoi(adr);
				break;
		}
		adr=strtok(NULL,"-");
	}
	
	k=0;
	adr=strtok(xt,":");
	while(adr){
		k++;
		switch(k){
			case 1:
				ts.tm_hour=atoi(adr);
				break;
			case 2:
				ts.tm_min=atoi(adr);
				break;
			case 3:
				ts.tm_sec=atoi(adr);
				break;
		}
		adr=strtok(NULL,":");
	}

	t=mktime(&ts);
	return(t);
}

