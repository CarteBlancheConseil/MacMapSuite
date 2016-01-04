//----------------------------------------------------------------------------
// File : valconv.c
// Project : MacMapSuite
// Purpose : C source file : Value conversion utils
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
// 28/01/04 creation.
//----------------------------------------------------------------------------

#include "valconv.h"
#include "db_types.h"
#include "db_utils.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------

int xToChar(int sign, int prm, void *val, char *conv){
int	flg=1;
	switch(sign){
		case _int:
		case _bool:
			sprintf(conv,"%d",(*((int*)val)));
			break;
		case _double:
			sprintf(conv,"%.*f",prm,(*((double*)val)));
			break;
		case _date:{
time_t		t=(*((double*)val));				
struct tm*	ts=localtime(&t);
			(void)strftime((char*)conv,256,"%Y-%m-%d",ts);
			}
			break;
		case _time:{
time_t		t=(*((double*)val));				
struct tm*	ts=localtime(&t);
			(void)strftime((char*)conv,256,"%H:%M:%S",ts);
			}		
			break;
		case _char:
			if(((char*)val)==NULL){
				return(0);
			}
			sprintf(conv,"%s",(char*)val);
			break;
			
		default:
			conv[0]=0;
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int xToInt(int sign, int prm, void *val, int *conv){
int	flg=1;
	switch(sign){
		case _int:
			(*conv)=(*((int*)val));
			break;
		case _bool:
			(*conv)=(*((int*)val))?1:0;
			break;
		case _date:
		case _time:
		case _double:
			(*conv)=round((*((double*)val)));
			break;
		case _char:
			if(((char*)val)==NULL){
				return(0);
			}
			(*conv)=atoi((char*)val);
			break;
			
		default:
			(*conv)=LONG_MIN;
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int xToDouble(int sign, int prm, void *val, double *conv){
int	flg=1;
	switch(sign){
		case _int:
		case _bool:
			(*conv)=(*((int*)val));
			break;
		case _date:
		case _time:
		case _double:
			(*conv)=(*((double*)val));
			break;
		case _char:
			if(((char*)val)==NULL){
				return(0);
			}
			strrep((char*)val,",",".");
			(*conv)=atof((char*)val);
			break;
		default:
			(*conv)=NAN;
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int xToDate(int sign, int prm, void *val, double *conv){
int	flg=1;
	switch(sign){
		case _int:
		case _bool:
			(*conv)=(*((int*)val));
			break;
		case _date:
		case _time:
		case _double:
			(*conv)=(*((double*)val));
			break;
		case _char:
			if(((char*)val)==NULL){
				return(0);
			}
			(*conv)=make_date((char*)val);
			break;
			
		default:
			(*conv)=NAN;
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int xToTime(int sign, int prm, void *val, double *conv){
int	flg=1;
	switch(sign){
		case _int:
		case _bool:
			(*conv)=(*((int*)val));
			break;
		case _date:
		case _time:
		case _double:
			(*conv)=(*((double*)val));
			break;
		case _char:
			if(((char*)val)==NULL){
				return(0);
			}
			(*conv)=make_time((char*)val);
			break;
			
		default:
			(*conv)=NAN;
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int charToX(int sign, int prm, char *val, void *conv){
int	flg=1;
	if(val==NULL){
		return(0);
	}
	switch(sign){
		case _int:
			(*((int*)conv))=atoi(val);
			break;
		case _bool:
			(*((int*)conv))=(atoi(val))?1:0;
			break;
		case _double:
			strrep(val,",",".");
			(*(double*)conv)=atof(val);
			break;
		case _char:
			strcpy((char*)conv,val);
			break;
		case _date:
			(*((double*)conv))=make_date(val);
			break;
		case _time:
			(*((double*)conv))=make_time(val);
			break;
		default:
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int intToX(int sign, int prm, int val, void *conv){
int	flg=1;
	switch(sign){
		case _int:
			(*((int*)conv))=val;
			break;
		case _bool:
			(*((int*)conv))=(val)?1:0;
			break;
		case _date:
		case _time:
		case _double:
			(*((double*)conv))=val;
			break;
		case _char:
			sprintf((char*)conv,"%d",val);
			break;
			
		default:
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int doubleToX(int sign, int prm, double val, void *conv){
int	flg=1;
	switch(sign){
		case _int:
			(*((int*)conv))=round(val);
			break;
		case _bool:
			(*((int*)conv))=(val!=0)?1:0;
			break;
		case _date:
		case _time:
		case _double:
			(*((double*)conv))=val;
			break;
		case _char:
			sprintf((char*)conv,"%.*f",prm,val);
			break;
			
		default:
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int dateToX(int sign, int prm, double val, void *conv){
int	flg=1;
	switch(sign){
		case _int:
			(*((int*)conv))=round(val);
			break;
		case _bool:
			(*((int*)conv))=(val!=0)?1:0;
			break;
		case _date:
		case _time:
		case _double:
			(*((double*)conv))=val;
			break;
		case _char:{
time_t		t=val;				
struct tm*	ts=localtime(&t);
			(void)strftime((char*)conv,256,"%Y-%m-%d",ts);
			}
			break;
			
		default:
			flg=0;
			break;
	}
	return(flg);
}

//----------------------------------------------------------------------------

int timeToX(int sign, int prm, double val, void *conv){
int	flg=1;
	switch(sign){
		case _int:
			(*((int*)conv))=round(val);
			break;
		case _bool:
			(*((int*)conv))=(val!=0)?1:0;
			break;
		case _date:
		case _time:
		case _double:
			(*((double*)conv))=val;
			break;
		case _char:{
time_t		t=val;				
struct tm*	ts=localtime(&t);
			(void)strftime((char*)conv,256,"%H:%M:%S",ts);
			}
			break;
			
		default:
			flg=0;
			break;
	}
	return(flg);
}


