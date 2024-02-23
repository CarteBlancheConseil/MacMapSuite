//----------------------------------------------------------------------------
// File : bv310GridAIASCIIBaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Arcinfo ASCII Grid database base class accessor version 3.1.0
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

#include "bv310GridAIASCIIBaseAccessor.h"
#include "db.h"
#include "wtable.h"

#include "bTrace.h"
#include "bStdDirectory.h"
#include "bStdFile.h"
#include "vx_allocation.h"
#include "vx_utils.h"
#include "vx_measure.h"
#include "strget.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310GridAIASCIIBaseAccessor	::bv310GridAIASCIIBaseAccessor(	const char* hpath,  
																int wanted_srid,
																double wanted_reso,
																int* status)
								:bv310BaseAccessor(hpath,wanted_srid,wanted_reso,status){
_bTrace_("bv310GridAIASCIIBaseAccessor::bv310GridAIASCIIBaseAccessor",true);
char		fpath[PATH_MAX];
char		name[FILENAME_MAX];

	_nc=_nl=_ox=_oy=_csz=_ndv=0;
	_arr=NULL;
	_dvs.sign=_2D_VX;
	_dvs.nv=1;
	_dvs.no=0;
	_dvs.offs=NULL;
	_dvs.vx.vx2[0].x=_dvs.vx.vx2[0].y=0;

	for(;;){
		load_data(hpath);
		
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
        
        _srid=4326;
        _reso=1;
        //_u2m=1;
        
// on ferme et on ré-ouvre pour reso et srid
		delete _hdr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,fpath,name,false,_reso,-1,status);
		if(*status){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
		
		/*sprintf(name,"links");
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
		}*/
		if((*status=load(wanted_srid))){
_te_("(*status=load()) failed with "+(*status));
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
	if(_arr){
		delete _arr;
		_arr=NULL;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310GridAIASCIIBaseAccessor	::bv310GridAIASCIIBaseAccessor(	const char* hpath,  
																int* status,
																int kind,
																int srid,
																double reso,
																double u2m,
																const char* name,
																const char* data)
								:bv310BaseAccessor(hpath,status,kind,srid,reso,u2m,name,data,2){
_bTrace_("bv310GridAIASCIIBaseAccessor::bv310GridAIASCIIBaseAccessor",true);
char		fname[FILENAME_MAX];
	_nc=_nl=_ox=_oy=_csz=_ndv=0;
	_arr=NULL;
	_dvs.sign=_2D_VX;
	_dvs.nv=1;
	_dvs.no=0;
	_dvs.offs=NULL;
	_dvs.vx.vx2[0].x=_dvs.vx.vx2[0].y=0;

	for(;;){
		load_data(hpath);
		
		sprintf(fname,"header");
		*status=noErr;
		_hdr=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
/*		sprintf(fname,"links");
		*status=_fd;
		_lnks=(bStdTable*)wtbl_alloc(kTableLocalDBNoFU,"",fname,true,_reso,-1,status);
		if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
			break;
		}
//		wtbl_free(_lnks);
//		_lnks=NULL;
		
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
		}*/
		
		if((*status=h_new(name))){
_te_("h_new failed with "+(*status));
			break;
		}
/*		if((*status=std_add())){
_te_("std_add failed with "+(*status));
			break;
		}
		
		if(_fld->CountRecords()<kOBJ_Dir_){
_te_("_(_fld->CountRecords()<kOBJ_Dir_)");
			*status=-1;
			break;
		}*/
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
	if(_arr){
		delete _arr;
		_arr=NULL;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310GridAIASCIIBaseAccessor::~bv310GridAIASCIIBaseAccessor(){
	if(_arr){
		delete _arr;
		_arr=NULL;
	}
}

// ---------------------------------------------------------------------------
// DYN OK
// -----------
int	bv310GridAIASCIIBaseAccessor::read(int o, int f, void* val){
_bTrace_("bv310GridAIASCIIBaseAccessor::read",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(-1);
	}

	switch(f){
		case kOBJ_ID_:
			*((int*)val)=o;
			break;
		case kOBJ_CrID_:
			*((double*)val)=0;
			break;
		case kOBJ_CrDate_:
			*((double*)val)=0;
			break;
		case kOBJ_MdID_:
			*((double*)val)=0;
			break;
		case kOBJ_MdDate_:
			*((double*)val)=0;
			break;
		case kOBJ_State_:
			*((double*)val)=0;
			break;
		case kOBJ_Rect_:{
			ivertices* vxs=make_vertices(o);
			ivs_bounds(vxs,((ivx_rect*)val));
			ivs_free(vxs);
			}break;
		case kOBJ_Vertices_:{
			(*(ivertices**)val)=make_vertices(o);
			}break;
		case kOBJ_Name_:
			((char*)val)[0]=0;
			break;
		case kOBJ_SubType_:
			*((int*)val)=1;
			break;
		case kOBJ_Color_:
			*((int*)val)=1;
			break;
		case kOBJ_Dir_:
			*((int*)val)=0;
			break;
		case kOBJ_Dir_+1:
			*((double*)val)=_arr[o-1];
			break;
	}
	
	return(noErr);
}

// ---------------------------------------------------------------------------
// On n'écrit pas
// -----------
int	bv310GridAIASCIIBaseAccessor::write(int o, int f, void* val){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310GridAIASCIIBaseAccessor::killed_record(int r){
	return(_arr[r-1]==_ndv);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310GridAIASCIIBaseAccessor::count_records(){
	return(_nc*_nl);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310GridAIASCIIBaseAccessor::signature(){
	return(kBaseArcInfoASCII);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
int bv310GridAIASCIIBaseAccessor::fld_open(){
_bTrace_("bv310GridAIASCIIBaseAccessor::fld_open",true);
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
int bv310GridAIASCIIBaseAccessor::cnt_open(){
_bTrace_("bv310GridAIASCIIBaseAccessor::cnt_open",true);
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
int bv310GridAIASCIIBaseAccessor::lnk_open(){
	return(0);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
void bv310GridAIASCIIBaseAccessor::fld_close(){
	delete _fld;
	_fld=NULL;
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310GridAIASCIIBaseAccessor::cnt_close(){
	delete _cnst;
	_cnst=NULL;
}*/

// ---------------------------------------------------------------------------
// 
// -----------
void bv310GridAIASCIIBaseAccessor::lnk_close(){
}

// ---------------------------------------------------------------------------
// On ne stocke pas les champs et ils ne sont pas éditables
// -----------
int bv310GridAIASCIIBaseAccessor::load_fields(){
field	fp;
	
	fp.state=0;
	fp.state|=_kFieldEditLock_;
	fp.state|=_kFieldDeleteLock_;
	fp.state|=_kFieldMasked_;
	
// Identifiant
	fp.offset=1;
	fp.fid=-1;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=15;
	fp.decs=0;
	strget(kMsgID,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	fp.offset++;
	fp.fid=-2;
	fp.kind=_char;
	fp.size=32;
	fp.len=30;
	fp.decs=0;
	strget(kMsgCreator,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=-3;
	fp.kind=_date;
	fp.size=sizeof(double);
	fp.len=30;
	fp.decs=0;
	strget(kMsgDTOC,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	fp.offset++;
	fp.fid=-4;
	fp.kind=_char;
	fp.size=32;
	fp.len=30;
	fp.decs=0;
	strget(kMsgModifier,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=-5;
	fp.kind=_date;
	fp.size=sizeof(double);
	fp.len=30;
	fp.decs=0;
	strget(kMsgDTOM,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=-6;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=15;
	fp.decs=0;
	strget(kMsgState,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"0");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=-7;
	fp.kind=_ivxrect;
	fp.size=sizeof(ivx_rect);
	fp.len=15;
	fp.decs=0;
	strget(kMsgBounds,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.state=0;
	fp.state|=_kFieldEditLock_;
	fp.state|=_kFieldDeleteLock_;

	fp.offset++;
	fp.fid=-8;
	fp.kind=_ivxs2;
	fp.size=sizeof(DB_usz);
	fp.len=15;
	fp.decs=0;
	strget(kMsgGeom,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	fp.offset++;
	fp.fid=1;
	fp.kind=_char;
	fp.size=64;
	fp.len=50;
	fp.decs=0;
	strget(kMsgName,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=2;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=15;
	fp.decs=0;
	strget(kMsgSubType,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"1");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	fp.offset++;
	fp.fid=3;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=15;
	fp.decs=0;
	strget(kMsgColor,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"1");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

	fp.offset++;
	fp.fid=8;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=2;
	fp.decs=0;
	strget(kMsgFlag,fp.name);
	fp.cnstrkind=0;
	strcpy(fp.defval,"0");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	fp.offset++;
	fp.fid=9;
	fp.kind=_double;
	fp.size=sizeof(double);
	fp.len=30;
	fp.decs=6;
	strcpy(fp.name,"VALUE");
	fp.cnstrkind=0;
	strcpy(fp.defval,"0");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);
	
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310GridAIASCIIBaseAccessor::load_constraints(){
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310GridAIASCIIBaseAccessor::load_dyn(){
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310GridAIASCIIBaseAccessor::load_data(const char* hpath){
_bTrace_("bv310GridAIASCIIBaseAccessor::load_data",true);
char		fpath[PATH_MAX];
	sprintf(fpath,"%sContents/Datas/data.txt",hpath);
bStdFile	f(fpath,"r");
char*		buff=NULL;
int			k=0,sz=0;
	
	f.mount(&buff,&sz);
	buff=(char*)realloc(buff,sz+1);
	buff[sz]=0;
	
char*	adr=strtok(buff,"\n");
	while(adr){
//_tm_(adr);
		if(strstr(adr,"ncols")!=NULL){
			_nc=atoi(adr+strlen("ncols"));
			_tm_("_nc="+(int)_nc);
		}
		else if(strstr(adr,"nrows")!=NULL){
			_nl=atoi(adr+strlen("nrows"));
			_tm_("_nl="+(int)_nl);
		}
		else if(strstr(adr,"xllcorner")!=NULL){
			_ox=atof(adr+strlen("xllcorner"));
			_tm_("_ox="+_ox);
		}
		else if(strstr(adr,"yllcorner")!=NULL){
			_oy=atof(adr+strlen("yllcorner"));
			_tm_("_oy="+_oy);
		}
		else if(strstr(adr,"cellsize")!=NULL){
			_csz=atof(adr+strlen("cellsize"));
			_tm_("_csz="+_csz);
		}
		else if(strstr(adr,"NODATA_value")!=NULL){
			_ndv=atof(adr+strlen("NODATA_value"));
			_tm_("_ndv="+_ndv);
		}
		else{
			if(	(_nc<=0)	||
				(_nl<=0)	||
				(_csz<=0)	){
				   break;
			}
			if(_arr==NULL){
				_arr=new double[_nc*_nl];
_tm_("new double["+(_nc*_nl)+"] ok");
				if(_arr==NULL){
					break;
				}
			}
			
char*		adrl=adr;
            if(adrl[0]==' '){
                adrl++;
            }
			_arr[k]=atof(adrl);

			k++;
			while((adrl=strchr(adrl,' '))){
				adrl++;
				if(adrl[0]=='\n'){
					break;
				}
				if(adrl[0]=='\r'){
					break;
				}
				if(adrl[0]==0){
					break;
				}
				_arr[k]=atof(adrl);
				k++;
				if(k>_nc*_nl){
_te_("depassement ici "+k+">"+(_nc*_nl));
				}
			}
			
		}
		
		adr=strtok(NULL,"\n");
	}
	
	free(buff);
			   
	return(true);
}


// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bv310GridAIASCIIBaseAccessor::make_vertices(long o){
long    i=(o-1L)%_nc;
long    j=(_nl-1L)-floor((o-1L)/_nc);

    _dvs.vx.vx2[0].x=_ox+((double)i)*_csz;
    _dvs.vx.vx2[0].y=_oy+((double)j)*_csz;
	
	if(_top&&_frp){
		_top->transform(*_frp,&_dvs);
	}
	
ivertices* vxs=NULL;
//	vxs_d2i(&vxs,&_dvs,_reso*_u2m);
    vxs_d2i(&vxs,&_dvs,_wanted_reso);
	return(vxs);
}
