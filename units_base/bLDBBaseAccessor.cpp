//----------------------------------------------------------------------------
// File : bLDBBaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Local database base class accessor version 3.0.0
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
// 04/07/2005 creation.
//----------------------------------------------------------------------------

#include "bLDBBaseAccessor.h"
#include "db.h"
#include "wtable.h"

#include "bTrace.h"
#include "bStdDirectory.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLDBBaseAccessor	::bLDBBaseAccessor(	const char* hpath,  
										int* status)
					:bv300BaseAccessor(hpath,status){
_bTrace_("bLDBBaseAccessor::bLDBBaseAccessor",false);
char		fpath[PATH_MAX];
char		name[FILENAME_MAX];

	for(;;){
		sprintf(fpath,"%sContents/Datas/",hpath);
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,fpath,kCFStringEncodingMacRoman);
		CFStringGetCString(cfs,fpath,PATH_MAX,kCFStringEncodingUTF8);
		CFRelease(cfs);
		
		sprintf(name,"header");
		*status=fuDontUse;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDB,fpath,name,false,_reso,_srid,status);
		if(*status<0){
			break;
		}
		
		if((*status=h_init())){
			break;
		}
// on ferme et on ré-ouvre pour reso et srid
		delete _hdr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDB,fpath,name,false,_reso,_srid,status);
		if(*status){
			break;
		}
		
		sprintf(name,"fields");
		*status=fuDontUse;
		_fld=(bStdTable*)wtbl_alloc(kTableLocalDB,fpath,name,false,_reso,_srid,status);
		if(*status<0){
			break;
		}
		sprintf(name,"constraints");
		*status=fuDontUse;
		_cnst=(bStdTable*)wtbl_alloc(kTableLocalDB,fpath,name,false,_reso,_srid,status);
		if(*status<0){
			break;
		}
		sprintf(name,"objects");
		*status=fuNeeded;
		_objs=(bStdTable*)wtbl_alloc(kTableLocalDB,fpath,name,false,_reso,_srid,status);
		if(*status<0){
			break;
		}
		if(_fld->CountRecords()<kOBJ_Dir_){
			*status=-1;
			break;
		}
		if((*status=load())){
			break;
		}
		return;
	}
_err_(_strr_+"status == "+(*status));
	_elts.reset();
	_celts.reset();
	if(_fld){	
		wtbl_free(_fld);
		_fld=NULL;
	}
	if(_objs){	
		wtbl_free(_objs);
		_objs=NULL;
	}
	if(_hdr){	
		wtbl_free(_hdr);
		_hdr=NULL;
	}
	if(_cnst){	
		wtbl_free(_cnst);
		_cnst=NULL;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLDBBaseAccessor	::bLDBBaseAccessor(	const char* hpath,  
										int* status,
										int kind,
										int srid,
										double reso,
										double u2m,
										const char* name,
										const char* data)
					:bv300BaseAccessor(hpath,status,kind,srid,reso,u2m,name,data,2){
_bTrace_("bLDBBaseAccessor::bLocalDBFieldBase",false);
char		fname[FILENAME_MAX];

	for(;;){
bStdDirectory	pkg(hpath);
	sprintf(fname,"%s.type/Contents/Datas",name);
bStdDirectory	cnt(fname);

		sprintf(fname,"header");
		*status=fuDontUse;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDB,"",fname,true,_reso,_srid,status);
		if(*status<0){
			break;
		}
		sprintf(fname,"constraints");
		*status=fuDontUse;
		_cnst=(bStdTable*)wtbl_alloc(kTableLocalDB,"",fname,true,_reso,_srid,status);
		if(*status<0){
			break;
		}
		sprintf(fname,"fields");
		*status=fuDontUse;
		_fld=(bStdTable*)wtbl_alloc(kTableLocalDB,"",fname,true,_reso,_srid,status);
		if(*status<0){
			break;
		}
		sprintf(fname,"objects");
		*status=fuNeeded;
		_objs=(bStdTable*)wtbl_alloc(kTableLocalDB,"",fname,true,_reso,_srid,status);
		if(*status<0){
			break;
		}
		if((*status=h_new(name))){
			break;
		}
		if((*status=std_add())){
			break;
		}
		
		if(_fld->CountRecords()<kOBJ_Dir_){
			*status=-1;
			break;
		}
		if((*status=load())){
			break;
		}
		return;
	}
_err_(_strr_+"status == "+(*status));
	_elts.reset();
	_celts.reset();
	if(_fld){	
		wtbl_free(_fld);
		_fld=NULL;
	}
	if(_objs){	
		wtbl_free(_objs);
		_objs=NULL;
	}
	if(_hdr){	
		wtbl_free(_hdr);
		_hdr=NULL;
	}
	if(_cnst){	
		wtbl_free(_cnst);
		_cnst=NULL;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLDBBaseAccessor::~bLDBBaseAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bLDBBaseAccessor::signature(){
	return(kBaseLocalDB);
}
