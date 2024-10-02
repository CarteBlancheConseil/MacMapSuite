//----------------------------------------------------------------------------
// File : bv310CBCProbedataBaseAccessor.cpp
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

#include "bv310CBCProbedataBaseAccessor.h"
#include "db.h"
#include "wtable.h"

#include "bTrace.h"
#include "bStdDirectory.h"
#include "bStdFile.h"
#include "vx_allocation.h"
#include "vx_utils.h"
#include "vx_measure.h"
#include "strget.h"

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>

//1710720000;235;3939486A696F463452754C476341683835564F773939486A696F463452754C47;4840217;-37095;80;212;;


// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310CBCProbedataBaseAccessor	::bv310CBCProbedataBaseAccessor(const char* hpath,
																int wanted_srid,
																double wanted_reso,
																int* status)
								:bv310BaseAccessor(hpath,wanted_srid,wanted_reso,status){
_bTrace_("bv310CBCProbedataBaseAccessor::bv310CBCProbedataBaseAccessor(simple)",true);
_tm_("wanted_reso="+wanted_reso);
                                    
    _dvs.sign=_2D_VX;
	_dvs.nv=1;
	_dvs.no=0;
	_dvs.offs=NULL;
	_dvs.vx.vx2[0].x=_dvs.vx.vx2[0].y=0;

char        fpath[PATH_MAX];
char        name[FILENAME_MAX];

        /*for(;;){
            sprintf(fpath,"%sContents/Datas/",hpath);
    CFStringRef    cfs=CFStringCreateWithCString(kCFAllocatorDefault,fpath,kCFStringEncodingMacRoman);
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
            }*/
    
    _reso=wanted_reso;

	for(;;){
		load_data(hpath);
        
        sprintf(fpath,"%sContents/Datas/",hpath);
CFStringRef    cfs=CFStringCreateWithCString(kCFAllocatorDefault,fpath,kCFStringEncodingMacRoman);
        CFStringGetCString(cfs,fpath,PATH_MAX,kCFStringEncodingUTF8);
        CFRelease(cfs);

        /*sprintf(name,"data.csv");
        *status=noErr;
        _dat=(bStdTable*)wtbl_alloc(kTableCSVText,fpath,name,false,_reso,-1,status);
        if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
            break;
        }*/

        sprintf(name,"header.txt");
        *status=noErr;
        _hdr=(bStdTable*)wtbl_alloc(kTableTabText,fpath,name,false,_reso,-1,status);
        if(*status<0){
_te_("_hdr=(bStdTable*)wtbl_alloc failed with "+(*status));
            break;
        }
        
        if((*status=h_init())){
            break;
        }

_tm_("resolution="+_reso+", srid="+_srid);

        _srid=4326;
        _reso=wanted_reso;
        
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
    if(_prbs){
        free(_prbs);
        _prbs=NULL;
    }
    _nprb=0;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310CBCProbedataBaseAccessor	::bv310CBCProbedataBaseAccessor(const char* hpath,
																int* status,
																int kind,
																int srid,
																double reso,
																double u2m,
																const char* name,
																const char* data)
								:bv310BaseAccessor(hpath,status,kind,srid,reso,u2m,name,data,2){
_bTrace_("bv310CBCProbedataBaseAccessor::bv310CBCProbedataBaseAccessor(all)",true);
char		fname[FILENAME_MAX];
                                    
    _dvs.sign=_2D_VX;
	_dvs.nv=1;
	_dvs.no=0;
	_dvs.offs=NULL;
	_dvs.vx.vx2[0].x=_dvs.vx.vx2[0].y=0;

	for(;;){
		load_data(hpath);
    
        _srid=4326;
        _reso=1;
         
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
    if(_prbs){
        free(_prbs);
        _prbs=NULL;
    }
    _nprb=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv310CBCProbedataBaseAccessor::~bv310CBCProbedataBaseAccessor(){
    if(_prbs){
        free(_prbs);
        _prbs=NULL;
    }
    _nprb=0;
}

// ---------------------------------------------------------------------------
//
// -----------
int	bv310CBCProbedataBaseAccessor::read(int o, int f, void* val){
_bTrace_("bv310CBCProbedataBaseAccessor::read",false);
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
            *((int*)val)=_prbs[o-1].timestamp;
            break;
        case kOBJ_Dir_+2:
            {
            char tmp[256],str[256];
            size_t  i;

            str[0]=0;
            for(i=0;i<32;i++){
                sprintf(tmp,"%02hX",_prbs[o-1].mobile_id[i]);
                strcat(str,tmp);
            }
            str[64]=0;
            strcpy(((char*)val),str);
            }
            break;
        case kOBJ_Dir_+3:
            *((double*)val)=((double)_prbs[o-1].lat)/100000.0;
            break;
        case kOBJ_Dir_+4:
            *((double*)val)=(double)_prbs[o-1].lon/100000.0;
            break;
        case kOBJ_Dir_+5:
            *((int*)val)=_prbs[o-1].speed;
            break;
        case kOBJ_Dir_+6:
            *((int*)val)=_prbs[o-1].heading;
            break;
    }
	
	return(noErr);
}

// ---------------------------------------------------------------------------
// On n'écrit pas
// -----------
int	bv310CBCProbedataBaseAccessor::write(int o, int f, void* val){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310CBCProbedataBaseAccessor::killed_record(int r){
	return false;
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310CBCProbedataBaseAccessor::count_records(){
	return _nprb;
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310CBCProbedataBaseAccessor::signature(){
	return(kBaseCBCProbedata);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310CBCProbedataBaseAccessor::lnk_open(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310CBCProbedataBaseAccessor::lnk_close(){
}

// ---------------------------------------------------------------------------
//
// -----------
int bv310CBCProbedataBaseAccessor::h_read(int o, int f, void *val){
    switch(f){
        case kHDR_id_:
            *((int*)val)=1;
            break;
        case kHDR_state_:
            *((int*)val)=_state;
            break;
        case kHDR_kind_:
            *((int*)val)=_kind;
            break;
        case kHDR_name_:
            sprintf((char*)val,"%s","Type CBC Probedata");
            break;
        case kHDR_precs_:
            *((double*)val)=_reso;
            break;
        case kHDR_unit2m_:
            *((double*)val)=_u2m;
            break;
        case kHDR_srid_:
            *((int*)val)=_srid;
            break;
    }
    return noErr;
}

// ---------------------------------------------------------------------------
//
// -----------
int bv310CBCProbedataBaseAccessor::h_write(int o, int f, void *val){
    return noErr;
}

// ---------------------------------------------------------------------------
//
// -----------
int bv310CBCProbedataBaseAccessor::load(int wanted_srid){
_bTrace_("bv310CBCProbedataBaseAccessor::load",false);
int        status;
    /*if((status=load_infos())){
_te_("load_infos() failed with "+status);
        return(status);
    }*/
    /*if((status=load_dyn())){
_te_("load_dyn() failed with "+status);
        return(status);
    }*/
    if((status=load_fields())){
_te_("load_fields() failed with "+status);
        return(status);
    }
    /*if((status=load_constraints())){
_te_("load_constraints() failed with "+status);
        return(status);
    }*/
    
// Projection
    _tm_("wanted_srid="+wanted_srid);
    _tm_("_srid="+_srid);
    if(wanted_srid==_srid){
        if(_top){
            delete _top;
            _top=NULL;
        }
    }
    if(_top){
        if(_srid<=0){
            delete _top;
            _top=NULL;
        }
        else{
bool        b;
            _frp=new bStdProj(_srid,&b);
            if(!b){
_te_("unknown srid (source):"+_srid);
                delete _frp;
                _frp=NULL;
                delete _top;
                _top=NULL;
            }
        }
    }
    if(_frp&&_top){
_tm_("----------- PROJECTION -----------");
    }
    return(noErr);
}

// ---------------------------------------------------------------------------
//
// -----------
/*int bv310CBCProbedataBaseAccessor::load_infos(){
    return(noErr);
}*/

// ---------------------------------------------------------------------------
// On ne stocke pas les champs et ils ne sont pas éditables
// -----------
int bv310CBCProbedataBaseAccessor::load_fields(){
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

// User
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

// Date de création
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
	
// ?
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

// Date de modification
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

// Etat
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

// Bound
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

// Géométrie
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
	
// Nom
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

// Sous-type
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
	
// Couleur
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

// Direction/Flag
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

//1710720000;235;3939486A696F463452754C476341683835564F773939486A696F463452754C47;4840217;-37095;80;212;;
	fp.offset++;
	fp.fid=9;
	fp.kind=_int;
	fp.size=sizeof(int);
	fp.len=16;
	fp.decs=0;
	strcpy(fp.name,"timestamp");
	fp.cnstrkind=0;
	strcpy(fp.defval,"0");
	fp.index=fp.offset;
	fp.tbl_id=0;
	_elts.add(&fp);

    fp.offset++;
    fp.fid=10;
    fp.kind=_char;
    fp.size=68;
    fp.len=68;
    fp.decs=0;
    strcpy(fp.name,"trace_id");
    fp.cnstrkind=0;
    strcpy(fp.defval,"0");
    fp.index=fp.offset;
    fp.tbl_id=0;
    _elts.add(&fp);

    fp.offset++;
    fp.fid=11;
    fp.kind=_double;
    fp.size=sizeof(double);
    fp.len=32;
    fp.decs=6;
    strcpy(fp.name,"latitude");
    fp.cnstrkind=0;
    strcpy(fp.defval,"0");
    fp.index=fp.offset;
    fp.tbl_id=0;
    _elts.add(&fp);

    fp.offset++;
    fp.fid=12;
    fp.kind=_double;
    fp.size=sizeof(double);
    fp.len=32;
    fp.decs=6;
    strcpy(fp.name,"longitude");
    fp.cnstrkind=0;
    strcpy(fp.defval,"0");
    fp.index=fp.offset;
    fp.tbl_id=0;
    _elts.add(&fp);
    
    fp.offset++;
    fp.fid=13;
    fp.kind=_int;
    fp.size=sizeof(int);
    fp.len=16;
    fp.decs=0;
    strcpy(fp.name,"speed");
    fp.cnstrkind=0;
    strcpy(fp.defval,"0");
    fp.index=fp.offset;
    fp.tbl_id=0;
    _elts.add(&fp);

    fp.offset++;
    fp.fid=14;
    fp.kind=_int;
    fp.size=sizeof(int);
    fp.len=16;
    fp.decs=0;
    strcpy(fp.name,"heading");
    fp.cnstrkind=0;
    strcpy(fp.defval,"0");
    fp.index=fp.offset;
    fp.tbl_id=0;
    _elts.add(&fp);


	return(noErr);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
int bv310CBCProbedataBaseAccessor::load_constraints(){
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310CBCProbedataBaseAccessor::load_dyn(){
	return(noErr);
}*/

// ---------------------------------------------------------------------------
//
// -----------
int chartoint(char c){
    if( c>= '0' && c <= '9')
        return c - '0';
    else if(c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    else if(c >= 'a' && c <= 'f')
        return 10 + c - 'a';
  return 0;
}


probe_point* prbpt_decode(const char* str, probe_point* frame){
int     i;
char*   lineParse;
char*   occur;
time_t  timestamp;
    
    if( (str==NULL)     ||
        (*str=='\0')    ){
        return NULL;
    }
    
    lineParse=strdup(str);

    occur=strtok(lineParse,";");
    if(!occur){
        return NULL;
    }
    timestamp=atoi(occur);
    
    occur=strtok(NULL,";");
    if(!occur){
        return NULL;
    }
    
    frame->timestamp=(time_t)timestamp;
          
    occur=strtok(NULL,";");
    if(occur!=NULL){
        i=0;
        while((*(occur+i*2)!='\0')&&(i<32)){
            frame->mobile_id[i]=(chartoint(*(occur+i*2))*16)|chartoint(*(occur+i*2+1));
            i++;
        }
    }
    occur=strtok(NULL,";");
    if(occur!=NULL){
        frame->lat=atoi(occur);
    }
    occur=strtok(NULL,";");
    if(occur!=NULL){
        frame->lon=atoi(occur);
    }
    occur=strtok(NULL,";");
    if(occur!=NULL){
        i=atoi(occur);
        frame->speed=i;
    }
    occur=strtok(NULL,";");
    if(occur!=NULL){
        frame->heading=atoi(occur);
    }

// New : vehicle type
    /*occur=strtok(NULL,";");
    if(occur!=NULL){
        frame->vehicle_t=atoi(occur);
//printf("veht=%d\n",frame->vehicle_t);
    }*/
    
    free(lineParse);

    return frame;
}


// ---------------------------------------------------------------------------
// 
// -----------
bool bv310CBCProbedataBaseAccessor::load_data(const char* hpath){
_bTrace_("bv310CBCProbedataBaseAccessor::load_data",true);
char		fpath[PATH_MAX];
int			k=0;
char        cmd[PATH_MAX];

    sprintf(fpath,"%sContents/Datas/data.csv",hpath);
    _nprb=0;
FILE* stream=fopen(fpath,"r");
    while(fgets(cmd,PATH_MAX,stream)!=NULL){
        _nprb++;
    }
    _prbs=(probe_point*)calloc(_nprb,sizeof(probe_point));
    fseek(stream,0,SEEK_SET);
    _nprb=0;
    while(fgets(cmd,PATH_MAX,stream)!=NULL){
        if(prbpt_decode(cmd,&_prbs[_nprb])){
            _nprb++;
        }
        else{
            break;
        }
    }
    fclose(stream);

	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bv310CBCProbedataBaseAccessor::make_vertices(long o){
//_bTrace_("bv310CBCProbedataBaseAccessor::make_vertices(all)",false);
    _dvs.vx.vx2[0].y=(_prbs[o-1].lat/100000.0);
    _dvs.vx.vx2[0].x=(_prbs[o-1].lon/100000.0);
//_tm_(_prbs[o-1].lon+":"+_prbs[o-1].lat);
	if(_top&&_frp){
		_top->transform(*_frp,&_dvs);
	}
//_tm_(_top+">"+_frp);
ivertices* vxs=NULL;
    vxs_d2i(&vxs,&_dvs,_wanted_reso);
//_tm_(vxs->vx.vx2[0].h+":"+vxs->vx.vx2[0].v);
	return(vxs);
}
