//----------------------------------------------------------------------------
// File : bv310LDBBaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Local database base class accessor version 3.1.0
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

#include "bv310LDBBaseAccessor.h"
#include "db.h"
#include "wtable.h"

#include "bTrace.h"
#include "bStdDirectory.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310LDBBaseAccessor	::bv310LDBBaseAccessor(	const char* hpath,  
												int wanted_srid,
												double wanted_reso,
												int* status)
						:bv310BaseAccessor(hpath,wanted_srid,wanted_reso,status){
_bTrace_("bv310LDBBaseAccessor::bv310LDBBaseAccessor",false);
char		fpath[PATH_MAX];
char		name[FILENAME_MAX];

	for(;;){
		sprintf(fpath,"%sContents/Datas/",hpath);
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,fpath,kCFStringEncodingMacRoman);
		CFStringGetCString(cfs,fpath,PATH_MAX,kCFStringEncodingUTF8);
		CFRelease(cfs);
		
		sprintf(name,"header");
		*status=noErr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,false,_reso,-1,status);
		if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		
		if((*status=h_init())){
			break;
		}
// on ferme et on ré-ouvre pour reso et srid
		delete _hdr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,false,_reso,-1,status);
		if(*status){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		
		sprintf(name,"links");
		*status=_fd;
		_lnks=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,true,_reso,-1,status);// true pour upgrade 3.0->3.1
		if(*status<0){
_te_("_lnks=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		wtbl_free(_lnks);
		_lnks=NULL;
		
		sprintf(name,"fields");
		*status=_fd;
		_fld=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,false,_reso,-1,status);
		if(*status<0){
_te_("_fld=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		sprintf(name,"constraints");
		*status=_fd;
		_cnst=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,false,_reso,-1,status);
		if(*status<0){
_te_("_cnst=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		sprintf(name,"objects");
		*status=noErr;
		_objs=(bStdTable*)wtbl_alloc(kTableLocalDBFU,fpath,name,false,_reso,_srid,status);
		if(*status<0){
_te_("_objs=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		
		if(_fld->CountRecords()<kOBJ_Dir_){
_te_("_fld->CountRecords()<kOBJ_Dir_");
			*status=-1;
			break;
		}
		if((*status=load(wanted_srid))){
_te_("(*status=load()) failed with "+(*status));
			break;
		}
_tm_("wanted reso="+wanted_reso);
		if(_reso*_u2m==_wanted_reso){
			_wanted_reso=0;
_tm_("wanted reso new="+_wanted_reso);
		}
		return;
	}
_te_("status == "+(*status));
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
	if(_lnks){	
		wtbl_free(_lnks);
		_lnks=NULL;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310LDBBaseAccessor	::bv310LDBBaseAccessor(	const char* hpath,  
												int* status,
												int kind,
												int srid,
												double reso,
												double u2m,
												const char* name,
												const char* data)
						:bv310BaseAccessor(hpath,status,kind,srid,reso,u2m,name,data,2){
_bTrace_("bv310LDBBaseAccessor::bv310LDBBaseAccessor",true);
char    fname[FILENAME_MAX];

	for(;;){
bStdDirectory	pkg(hpath);
	sprintf(fname,"%s.type/Contents/Datas",name);
bStdDirectory	cnt(fname);

		sprintf(fname,"header");
		*status=noErr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
        
		sprintf(fname,"links");
		*status=_fd;
		_lnks=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		wtbl_free(_lnks);
		_lnks=NULL;
		
		sprintf(fname,"constraints");
		*status=_fd;
		_cnst=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_cnst=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
        
		sprintf(fname,"fields");
		*status=_fd;
		_fld=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_fld=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
        
		sprintf(fname,"objects");
		*status=noErr;
		_objs=(bStdTable*)wtbl_alloc(kTableLocalDBFU,"",fname,true,_reso,_srid,status);
		if(*status<0){
_te_("_objs=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
        
		if((*status=h_new(name))){
_te_("h_new failed with "+(*status));
			break;
		}
		if((*status=std_add())){
_te_("std_add failed with "+(*status));
			break;
		}
		
		if(_fld->CountRecords()<kOBJ_Dir_){
_te_("_(_fld->CountRecords()<kOBJ_Dir_)");
			*status=-1;
			break;
		}
		if((*status=load(srid))){
_te_("load failed with "+(*status));
			break;
		}
		return;
	}
_te_("status == "+(*status));
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
	if(_lnks){	
		wtbl_free(_lnks);
		_lnks=NULL;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310LDBBaseAccessor::~bv310LDBBaseAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310LDBBaseAccessor::signature(){
	return(kBaseLocalDB);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
int bv310LDBBaseAccessor::fld_open(){
_bTrace_("bv310LDBBaseAccessor::fld_open",true);
bStdDirectory	root(_fd);
	if(root.status()){
		return(-1);
	}
int		status;
	_fld=(bStdTable*)wtbl_alloc(kTableLocalDB,"Contents/Datas/","fields",false,_reso,_srid,&status);
	if(status){
		return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310LDBBaseAccessor::cnt_open(){
_bTrace_("bv310LDBBaseAccessor::cnt_open",true);
bStdDirectory	root(_fd);
	if(root.status()){
		return(-1);
	}
int		status;
	_cnst=(bStdTable*)wtbl_alloc(kTableLocalDB,"Contents/Datas/","constraints",false,_reso,_srid,&status);
	if(status){
		return(-1);
	}
	return(0);
}*/

// ---------------------------------------------------------------------------
// 
// -----------
int bv310LDBBaseAccessor::lnk_open(){
_bTrace_("bv310LDBBaseAccessor::lnk_open",false);
    if(_lnks){
_te_((void*)_lnks+" already open");
        return(0);
    }
bStdDirectory	root(_fd);
	if(root.status()){
		return(-1);
	}
int		status=0;
	_lnks=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU/*kTableLocalDB*/,"Contents/Datas/","links",false,_reso,_srid,&status);
	if(status<0){
		return(-1);
	}
//_tm_("open "+(void*)_lnks);
	return(0);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
void bv310LDBBaseAccessor::fld_close(){
	delete _fld;
	_fld=NULL;
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310LDBBaseAccessor::cnt_close(){
	delete _cnst;
	_cnst=NULL;
}*/

// ---------------------------------------------------------------------------
// 
// -----------
void bv310LDBBaseAccessor::lnk_close(){
//_bTrace_("bv310LDBBaseAccessor::lnk_close",true);
	if(_lnks){
//_tm_("close "+(void*)_lnks);
		delete _lnks;
		_lnks=NULL;
	}
}

