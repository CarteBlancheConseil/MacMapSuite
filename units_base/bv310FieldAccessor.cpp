//----------------------------------------------------------------------------
// File : bv310FieldAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Database base field accessor class version 3.1.0
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
// 13/10/2006 creation.
//----------------------------------------------------------------------------

#include "wtable.h"
#include "bv310FieldAccessor.h"
#include "mms_config.h"

#include "bTrace.h"
#include "C_Utils.h"

#include "valconv.h"

#include "db.h"
#include "vx_lib.h"

#include "strget.h"

#include "bStdDirectory.h"
#include "bStdFile.h"

#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310FieldAccessor	::bv310FieldAccessor(	bStdTable* tbl,
											const char* name,
											int tid,
											int rfield,
											int off){
	_tbl=tbl;
	strcpy(_name,name);
	_tid=tid;
	_rfield=rfield;
	_off=off;
}


// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310FieldAccessor::~bv310FieldAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
const char*	bv310FieldAccessor::get_name(){
	return(_name);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310FieldAccessor::get_id(){
	return(_tid);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310FieldAccessor::get_ref(){
	return(_rfield);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310FieldAccessor::get_offset(){
	return(_off);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310FieldAccessor::remove(){
	_rfield--;
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310FieldAccessor::read(	int o, 
								int f, 
								void *val,
								void* prm,
								int(*rd_proc)(void*,int,int,void*)){
	return(-1);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310StaFieldAccessor	::bv310StaFieldAccessor(bStdTable* tbl,
												int off)
						:bv310FieldAccessor(tbl,"",1,0,off){
}


// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310StaFieldAccessor::~bv310StaFieldAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310StaFieldAccessor::read(	int o, 
									int f, 
									void *val,
									void* prm,
									int(*rd_proc)(void*,int,int,void*)){
	return(_tbl->ReadVal(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
static int comp_int(const void* a, const void* b){
tbl_index* ga=(tbl_index*)a;
tbl_index* gb=(tbl_index*)b;
	return((*((int*)ga->val))-(*((int*)gb->val)));
}

// ---------------------------------------------------------------------------
// 
// -----------
static int comp_double(const void* a, const void* b){
tbl_index* ga=(tbl_index*)a;
tbl_index* gb=(tbl_index*)b;
double	r=(*((double*)ga->val))-(*((double*)gb->val));
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
static int comp_str(const void* a, const void* b){
tbl_index* ga=(tbl_index*)a;
tbl_index* gb=(tbl_index*)b;
	return(strcmp((char*)(ga->val),(char*)(gb->val)));
}


// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310DynFieldAccessor	::bv310DynFieldAccessor(bStdTable* tbl,
												const char* name,
												int tid,
												int rfield,
												int off,
												int tfield,
												int rsign,
												int rsize)
						:bv310FieldAccessor(tbl,name,tid,rfield,off)
						,_field_index(sizeof(tbl_index)){
_bTrace_("bv310DynFieldAccessor::bv310DynFieldAccessor",false);
	_tfield=tfield;
	_rsign=rsign;
	_rsign=rsign;
	_buff.val=NULL;

int		flg,d,sz;

_tm_("x");
	if(tbl->FieldSize(_tfield,&sz)){
_te_("bad field size !");
		return;
	}

_tm_("x");
void*	rb=malloc(sz);
	if(!rb){
_te_("malloc("+sz+") failed for rb");
		return;	
	}
	
_tm_("x");
	tbl->FieldSign(_tfield,&_tsign);
_tm_("x");
	tbl->FieldDecimals(_tfield,&d);

_tm_("x");
	if(_tsign!=_rsign){
_te_("signs -> "+(UInt32*)&_rsign+":"+(UInt32*)&_tsign);
	}

tbl_index	rx;
	
_tm_("x");
	switch(_rsign){
		case _bool:
		case _int:
			_comp=comp_int;
			break;
		case _double:
		case _date:
		case _time:
			_comp=comp_double;
			break;
		case _char:
			_comp=comp_str;
			break;
		default:
_te_("bad field sign "+(UInt32*)&_rsign+" in base");
            free(rb);
			return;
			break;
	}
_tm_("x");
	_buff.val=malloc(rsize);
	if(!_buff.val){
_te_("malloc("+sz+") failed for _buff.val");
        free(rb);
		return;
	}
	_buff.o=0;


_tm_("ready");
	for(int i=1;i<=tbl->CountRecords();i++){
//_tm_("%d",i);
		if(tbl->RecordKilled(i)){
			continue;
		}
		rx.val=malloc(rsize);
		rx.o=i;
//		if(tbl->ReadVal(i,_tfield,rx.val)){
		if(tbl->ReadVal(i,_tfield,rb)){
_te_("tbl->ReadVal");
			continue;
		}
		if(tbl->ReadVal(i,_tfield,rb)){
_te_("tbl->ReadVal");
			continue;
		}
		switch(_tsign){
			case _bool:
			case _int:
				flg=intToX(_rsign,d,(*((int*)rb)),rx.val);
				break;
			case _double:
				flg=doubleToX(_rsign,d,(*((double*)rb)),rx.val);
				break;
			case _date:
				flg=dateToX(_rsign,d,(*((double*)rb)),rx.val);
				break;
			case _time:
				flg=timeToX(_rsign,d,(*((double*)rb)),rx.val);
				break;
			case _char:
				flg=charToX(_rsign,d,(char*)rb,rx.val);
				break;
			default:
				flg=0;
				break;
		}
		if(!flg){
			continue;
		}
		_field_index.add(&rx);
	}
	free(rb);
	_field_index.sort(_comp);
}


// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310DynFieldAccessor::~bv310DynFieldAccessor(){
_bTrace_("bv310DynFieldAccessor::~bv310DynFieldAccessor",false);
tbl_index	rx;
	
_tm_("deleting "+_name);	
	for(int i=1;i<=_field_index.count();i++){
		_field_index.get(i,&rx);
		if(rx.val){
			free(rx.val);
		}
	}
	_field_index.reset();
	if(_buff.val){
		free(_buff.val);
	}
	if(_tbl){
		delete _tbl;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310DynFieldAccessor::read(	int o, 
									int f, 
									void *val,
									void* prm,
									int(*rd_proc)(void*,int,int,void*)){
_bTrace_("bv310DynFieldAccessor::read",false);
	if(!_buff.val){
_te_("buffer not allocated");
		return(-1);
	}
tbl_index	rx;

	_buff.o=0;
	memset(_buff.val,_rsize,0);
	if(rd_proc(prm,o,_rfield,_buff.val)){
_te_("rd_proc failed");
		return(-1);
	}
int k=_field_index.search(&_buff,_comp);
	if(k==0){
_te_("search failed");
		return(-2);
	}
	_field_index.get(k,&rx);
	if(_tbl->ReadVal(rx.o,f,val)){
_te_("ReadVal failed");
		return(-3);
	}
	return(0);
}

