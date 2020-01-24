//----------------------------------------------------------------------------
// File : bString.cpp
// Project : MacMapSuite
// Purpose : CPP source file : String utily class
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
// 29/06/2006 creation.
//----------------------------------------------------------------------------

#include "bString.h"
#include "db.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bString::bString(const char* str){
	_str=strdup(str);
	if(_str==NULL){
		_str=(char*)malloc(1);
		_str[0]=0;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bString::~bString(){
	if(_str){
		free((void*)_str);
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::reset(){
	if(_str){
        free((void*)_str);
		_str=NULL;
	}
	_str=(char*)malloc(1);
	_str[0]=0;
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (const char* str){
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (char x){
char	str[4];
	sprintf(str,"%c",x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (int x){
char	str[64];
	sprintf(str,"%d",x);
	cat(str);
	return(*this);
}

// 
// ------------
bString& bString::operator + (int* x){
char	str[8];
	sprintf(str,"%.4s",(char*)x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (unsigned int x){
char	str[64];
	sprintf(str,"%u",(unsigned int)x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (unsigned int* x){
char	str[8];
UInt32	lx=*x;
#ifdef __LITTLE_ENDIAN__
	swapword(sizeof(UInt32),&lx);
#endif		
	sprintf(str,"%.4s",(char*)&lx);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (double x){
char	str[256];
	sprintf(str,"%g",x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (float x){
char	str[256];
	sprintf(str,"%g",x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (void* x){
char	str[256];
	sprintf(str,"%lx",(long)x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (long x){
//char*	str=asctime(localtime(&x));
//	cat(str);
char	str[128];
	sprintf(str,"%ld",x);
	cat(str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
bString& bString::operator + (bString& x){
	cat(x._str);
	return(*this);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bString::cat(const char* str){
	if(!_str){
		_str=strdup(str);
	}
	else{
int		len=strlen(_str);
char*	bf=(char*)realloc(_str,len+strlen(str)+1);
		if(bf){
			_str=bf;
			memmove(&_str[len],str,strlen(str));
			_str[len+strlen(str)]=0;
		}
	}
}

