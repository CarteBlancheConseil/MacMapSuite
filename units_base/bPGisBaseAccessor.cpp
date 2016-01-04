//----------------------------------------------------------------------------
// File : bPGisBaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Postgis database base class accessor version 3.0.0
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
// 05/07/2005 creation.
//----------------------------------------------------------------------------

#include "bPGisBaseAccessor.h"
#include "wtable.h"

#include "bTrace.h"
#include "bStdDirectory.h"
#include "bStdFile.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bPGisBaseAccessor	::bPGisBaseAccessor(	const char* hpath,  
											int* status)
					:bv300BaseAccessor(hpath,status){
_bTrace_("bPGisBaseAccessor::bPGisBaseAccessor",false);
char		name[512];
char		_name[FILENAME_MAX];
char		path[SHRT_MAX];
char		data[1024];
	
	for(;;){		
		strcpy(path,hpath);
		make_data(data,path);
		
		_name[0]=0;
		read_name(path,_name);		
		
		sprintf(name,"%s_%s",_name,"header");
		_hdr=(bStdTable*)wtbl_alloc(kTablePostGIS,data,name,false,_reso,_srid,status);
		if(*status){
			break;
		}

		if((*status=h_init())){
			break;
		}

// on ferme et on ré-ouvre pour reso et srid
		delete _hdr;
		_hdr=(bStdTable*)wtbl_alloc(kTablePostGIS,data,name,false,_reso,_srid,status);
		if(*status){
			break;
		}
		
		sprintf(name,"%s_%s",_name,"fields");
		_fld=(bStdTable*)wtbl_alloc(kTablePostGIS,data,name,false,_reso,_srid,status);
		if(*status){
			break;
		}
		sprintf(name,"%s_%s",_name,"constraints");
		_cnst=(bStdTable*)wtbl_alloc(kTablePostGIS,data,name,false,_reso,_srid,status);
		if(*status){
			break;
		}
		sprintf(name,"%s_%s",_name,"objects");
		_objs=(bStdTable*)wtbl_alloc(kTablePostGIS,data,name,false,_reso,_srid,status);
		if(*status){
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
bPGisBaseAccessor	::bPGisBaseAccessor(	const char* hpath,  
											int* status,
											int kind,
											int srid,
											double reso,
											double u2m,
											const char* name,
											const char* data)
					:bv300BaseAccessor(hpath,status,kind,srid,reso,u2m,name,data,10){
_bTrace_("bPGisBaseAccessor::bPGisBaseAccessor",false);
char		fname[512];
char		ldata[1024];

	for(;;){
bStdDirectory	pkg(hpath);
	sprintf(fname,"%s.type/Contents/Datas",name);
bStdDirectory	cnt(fname);

		make_data(ldata);		
		
		sprintf(fname,"%s_%s",name,"header");
		_hdr=(bStdTable*)wtbl_alloc(kTablePostGIS,ldata,fname,true,reso,srid,status);
		if(*status){
			break;
		}
		sprintf(fname,"%s_%s",name,"fields");
		_fld=(bStdTable*)wtbl_alloc(kTablePostGIS,ldata,fname,true,reso,srid,status);
		if(*status){
			break;
		}
		sprintf(fname,"%s_%s",name,"constraints");
		_cnst=(bStdTable*)wtbl_alloc(kTablePostGIS,ldata,fname,true,reso,srid,status);
		if(*status){
			break;
		}
		sprintf(fname,"%s_%s",name,"objects");
		_objs=(bStdTable*)wtbl_alloc(kTablePostGIS,ldata,fname,true,reso,srid,status);
		if(*status){
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
bPGisBaseAccessor::~bPGisBaseAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bPGisBaseAccessor::signature(){
	return(kBasePostGIS);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisBaseAccessor::make_data(char* data){
char	d[256];
char	h[256];
char	u[256];
char	p[256];
int		port;
	read_data(d,h,&port,u,p);
	if(port==0){
		port=5432;
	}
	if(strlen(u)>0){
		sprintf(data,"dbname=%s hostaddr=%s port=%d user=%s password=%s",d,h,port,u,p);
	}
	else{
		sprintf(data,"dbname=%s hostaddr=%s port=%d",d,h,port);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisBaseAccessor::make_data(char* data, const char* path){
char	d[256];
char	h[256];
char	u[256];
char	p[256];
int		port;
	read_data(d,h,&port,u,p,path);
	if(port==0){
		port=5432;
	}
	if(strlen(u)>0){
		sprintf(data,"dbname=%s hostaddr=%s port=%d user=%s password=%s",d,h,port,u,p);
	}
	else{
		sprintf(data,"dbname=%s hostaddr=%s port=%d",d,h,port);
	}
}
