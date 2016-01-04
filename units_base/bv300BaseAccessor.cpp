//----------------------------------------------------------------------------
// File : bv300BaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Database base class version 3.0.0
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
#include "bv300BaseAccessor.h"
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
#include <sys/fcntl.h>

// ---------------------------------------------------------------------------
// 
// ------------
char*	bv300BaseAccessor::_user=NULL;

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv300BaseAccessor	::bv300BaseAccessor(const char* hpath,  
										int* status)
					:bStdBaseAccessor()
					,_elts(sizeof(field))
					,_celts(sizeof(cnst)){
_bTrace_("bv300BaseAccessor::bv300BaseAccessor (open)",_VERBOSE_);
	*status=noErr;
	_fid=0;
	_oid=0;
	_fld=NULL;
	_objs=NULL;
	_hdr=NULL;
	_cnst=NULL;
	
	_kind=0;
	_srid=0;
	_reso=0;
	_u2m=0;

	_vers=_kVersion300;
	
	if(_user==NULL){
		init_user();
	}

char			fn[PATH_MAX];
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,hpath,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,fn,PATH_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	_fd=open(fn,O_RDONLY);
	if(_fd==-1){
_te_("open failed for O_RDONLY");
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv300BaseAccessor	::bv300BaseAccessor(const char* hpath,  
										int* status,
										int kind,
										int srid,
										double reso,
										double u2m,
										const char* name,
										const char* data,
										int hkind)
					:bStdBaseAccessor()
					,_elts(sizeof(field))
					,_celts(sizeof(cnst)){
_bTrace_("bv300BaseAccessor::bv300BaseAccessor (create)",_VERBOSE_);
	*status=noErr;
	_fid=0;
	_oid=0;
	_fld=NULL;
	_objs=NULL;
	_hdr=NULL;
	_cnst=NULL;

	_kind=kind;
	_srid=srid;
	_reso=reso;
	_u2m=u2m;

	_vers=_kVersion300;

	if(_user==NULL){
		init_user();
	}
	
	if(!make_package(hkind,hpath,name,data)){
		*status=-1;
	}
	
bStdDirectory	rt(hpath);
char			fn[FILENAME_MAX];
	sprintf(fn,"%s.type",name);
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,fn,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,fn,FILENAME_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	_fd=open(fn,O_RDONLY);
	if(_fd==-1){
_te_("open failed for O_RDONLY");
	}

}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bv300BaseAccessor::~bv300BaseAccessor(){
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
	if(_fd!=-1){	
		close(_fd);
		_fd=-1;
	}
	_elts.reset();
	_celts.reset();
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::read(int o, int f, void* val){
_bTrace_("bv300BaseAccessor::read",false);
int		status;
	if((status=_objs->ReadVal(o,f,val))){
_te_("_objs->ReadVal failed with "+status+" for("+o+";"+f+")");
		return(status);
	}
	if(f==kOBJ_ID_){
		if((*((int*)val))>_oid){
_te_("database corrupted : last oid ("+_oid+"), will be set to "+(*((int*)val)));
			if((status=_hdr->WriteVal(1,kHDR_oid_,val))){
_te_("database corrupted : _hdr->WriteVal failed with "+status);
				return(-1);
			}
			_oid=(*((int*)val));
		}
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::write(int o, int f, void* val){
_bTrace_("bv300BaseAccessor::write",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(-1);
	}
	if(fp.state&_kFieldEditLock_){
_te_("field "+f+" protected");
		return(-2);
	}
	
	if(o>_objs->CountRecords()){
		if(f!=kOBJ_ID_){
_te_("could not append record with init field "+f);
			return(-3);
		}
		if(append()){
_te_("append failed");
			return(-4);
		}
		(*((int*)val))=_oid;
		return(noErr);
	}	
	
	if((fp.cnstrkind!=0)&&(fp.cnstrkind==fp.kind)){
		if(get_constraint_index(f,val)<=0){
			if(count_constraints(f)>0){
_te_("constraint not found for field "+f);
				return(-5);
			}
		}
	}
	else if((fp.cnstrkind!=0)&&(fp.cnstrkind!=fp.kind)){
		if(((*((int*)val))<1)||((*((int*)val))>count_constraints(f))){
_te_("constraint not found for field "+f);
			return(-6);
		}
	}
int status;
	if((status=_objs->WriteVal(o,f,val))){
_te_("WriteVal failed with "+status+" for ("+o+";"+f+")");
		return(status);
	}
	if(f==kOBJ_Vertices_){
ivx_rect	vxr;
		ivs_bounds((*(ivertices**)val),&vxr);
		if((status=_objs->WriteVal(o,kOBJ_Rect_,&vxr))){
_te_("WriteVal error "+status+" for record "+o+" field kOBJ_Rect_");
			return(status);
		}
	}

	if((f!=kOBJ_MdID_)&&(f!=kOBJ_MdDate_)){
		o_put_date(o);
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::count(){
	return(_elts.count());
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::add(int kind, int len, int decs,	const char *name){
_bTrace_("bv300BaseAccessor::add",false);
	if(is_lock()){
_te_("type is lock");
		return(-1);
	}
	if(_objs->AddField(name,kind,len,decs)){
_te_("_objs->AddField failed");
		return(-2);
	}
	//hdr_open();
	//fld_open();
	//cnt_open();
int status=report_add(kind,len,decs,name);
	//cnt_close();
	//fld_close();
	//hdr_close();
	return(status);
}

// ---------------------------------------------------------------------------
// USE FLD
// -----------
int	bv300BaseAccessor::modify(int f, int kind, int len, int decs, const char* name){
_bTrace_("bv300BaseAccessor::modify",false);
	if(is_lock()){
		return(-1);
	}
	if(f<=kOBJ_Dir_){
_te_("could not modify standard field");
		return(-2);
	}	
field	fp;
int		status;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(-3);
	}
	if((status=_objs->ChgField(f,name,kind,len,decs))){
_te_("_objs->ChgField = "+status);
		return(-4);
	}
int		sz;
	if((status=_objs->FieldSize(f,&sz))){
_te_("_objs->FieldSize = "+status);
		return(-5);
	}
	if(fp.kind!=kind){
		//fld_open();
		if((status=_fld->WriteVal(fp.offset,kFLD_kind_,&kind))){
_te_("_fld->WriteVal(fp.offset,kFLD_kind_,&kind) failed with "+status);
			//fld_close();
			return(-6);
		}
		if(fp.cnstrkind==fp.kind){
			if((status=_fld->WriteVal(fp.offset,kFLD_constKind_,&kind))){
_te_("_fld->WriteVal(fp.offset,kFLD_constKind_,&kind) failed with "+status);
				//fld_close();
				return(-6);
			}
			fp.cnstrkind=kind;
		}
		//fld_close();
		fp.kind=kind;
	}
	if(fp.len!=len){
		//fld_open();
		if((status=_fld->WriteVal(fp.offset,kFLD_len_,&len))){
_te_("_fld->WriteVal(fp.offset,kFLD_len_,&len) failed with "+status);
			//fld_close();
			return(-7);
		}
		//fld_close();
		fp.len=len;
	}
	if(fp.decs!=decs){
		//fld_open();
		if((status=_fld->WriteVal(fp.offset,kFLD_decs_,&decs))){	
_te_("_fld->WriteVal(fp.offset,kFLD_decs_,&decs) failed with "+status);
			//fld_close();
			return(-8);
		}
		//fld_close();
		fp.decs=decs;
	}
	if(strcmp(fp.name,name)){
		//fld_open();
		if((status=_fld->WriteVal(fp.offset,kFLD_name_,(void*)name))){
_te_("_fld->WriteVal(fp.offset,kFLD_name_,&name) failed with "+status);
			//fld_close();
			return(-9);
		}
		//fld_close();
		strncpy(fp.name,name,256);
	}
	if(fp.size!=sz){
		//fld_open();
		if((status=_fld->WriteVal(fp.offset,kFLD_size_,&sz))){
_te_("_fld->WriteVal(fp.offset,kFLD_size_,&sz) failed with "+status);
			//fld_close();
			return(-11);
		}
		//fld_close();
		fp.size=sz;
	}
	_elts.put(f,&fp);
	f_put_date(f);
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::remove(int f){
_bTrace_("bv300BaseAccessor::remove",false);
	if(is_lock()){
_te_("type is lock");
		return(-1);
	}
	if(f<=kOBJ_Dir_){
_te_("could not remove standard field");
		return(-2);
	}	
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(-3);
	}
	if(is_delprotected(f)){
_te_("field "+f+" is del protected");
		return(-4);
	}
	if(_objs->RmvField(f)){
_te_("_objs->RmvField("+f+") failed");
		return(-5);
	}
	f_put_date(f);
	fp.index=-1;
	_elts.put(f,&fp);
	if(fp.cnstrkind!=0){
int n=count_constraints(f);
		for(int i=n;i>0;i--){
			if(!rmv_constraint(f,i,false)){
_te_("rmv_constraint "+i+" failed");
			}
		}
	}
	if(_fld->KillRecord(fp.offset)){
_te_("KillRecord failed");
		return(-6);
	}
	if(!_elts.rmv(f)){
_te_("_elts.rmv failed");
		return(-7);
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_kind(int	f,	int	*k){
field	fp;
	
	(*k)=0;
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	(*k)=fp.kind;
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_len(int	f,	int	*l){
field	fp;
	
	(*l)=0;
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	(*l)=fp.len;
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_decs(int f,	int	*d){
field	fp;
	
	(*d)=0;
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	(*d)=fp.decs;
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_name(int f,	char* name){
field	fp;
	
	strcpy(name,"");
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);	
	strcpy(name,fp.name);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_size(int f, int* sz){
field	fp;
	
	(*sz)=0;
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	(*sz)=fp.size;
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::get_id(int f){
field	fp;
	if((f<1)||(f>count())){
		return(0);
	}
	_elts.get(f,&fp);
	return(fp.fid);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::get_id(const char* name){
	int	f=get_index(name);
	if(!f){
		return(0);
	}
	return(get_id(f));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::get_index(int fid){
int		i,n=count();
field	fp;	
	
	for(i=1;i<=n;i++){
		_elts.get(i,&fp);
		if(fp.fid==fid){
			return(i);
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::get_index(const char* name){
int		i,n=count();
field	fp;	
char	n1[256],n2[256];
	
	strncpy(n1,name,256);
	strupper(n1);
	for(i=1;i<=n;i++){
		_elts.get(i,&fp);
		strncpy(n2,fp.name,256);
		strupper(n2);
		if(!strcmp(n1,n2)){
			return(i);
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::count_constraints(int f){
_bTrace_("bv300BaseAccessor::count_constraints",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(0);
	}
cnst	c;
	c.fid=fp.fid;
	c.idx=1;
int		k=_celts.search(&c,constraint_comp);
	if(!k){
		return(0);
	}
int		r=0;
	for(int i=k;i<=_celts.count();i++){
		_celts.get(i,&c);
		if(c.fid!=fp.fid){
			break;
		}
		r++;
	}
	return(r);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::get_constraints_kind(int f){
_bTrace_("bv300BaseAccessor::get_constraints_kind",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(0);
	}
	return(fp.cnstrkind);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_constraint(int f, int n, void* val){
_bTrace_("bv300BaseAccessor::get_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
cnst	c;
	c.fid=fp.fid;
	c.idx=n;
int		k=_celts.search(&c,constraint_comp);
	if(!k){
_te_("constraint "+n+" not found for field "+f);
		return(false);
	}
	if(!_celts.get(k,&c)){
_te_("_celts.get for "+k);
		return(false);
	}
	
	if(fp.kind==fp.cnstrkind){
		if(!charToX(fp.kind,fp.decs,c.val,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy((char*)val,c.val);
	}
	
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::set_constraint(int f, int n, void* val){
	return(set_constraint(f,n,val,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::add_constraint(int f, int kind, void* val){
	return(add_constraint(f,kind,val,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::rmv_constraint(int f, int n){
	return(rmv_constraint(f,n,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::get_constraint_index(int f, void* val){
_bTrace_("bv300BaseAccessor::get_constraint_index",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(0);
	}
cnst	c;
	c.fid=fp.fid;
	c.idx=1;
int		k=_celts.search(&c,constraint_comp);
	if(!k){
_te_("field "+f+" has no constraint");
		return(0);
	}
char tmp[256];
	if(fp.kind==fp.cnstrkind){
		if(!xToChar(fp.cnstrkind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy(tmp,(char*)val);
	}
int		idx=0;
	for(int i=k;i<=_celts.count();i++){
		_celts.get(i,&c);
		if(c.fid!=fp.fid){
			break;
		}
		if(strcmp(c.val,tmp)==0){
			idx=c.idx;
			break;
		}
	}
	return(idx);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::set_constraint_index(int f, int from, int to){
_bTrace_("bv300BaseAccessor::set_constraint_index",false);
_te_("not implemented");
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::has_default_value(int f){
_bTrace_("bv300BaseAccessor::has_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	return(strlen(fp.defval)>0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_default_value(int f, void* val){
_bTrace_("bv300BaseAccessor::get_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if(strlen(fp.defval)<=0){
		return(false);
	}
	if((fp.kind==fp.cnstrkind)||(fp.cnstrkind==0)){
		if(!charToX(fp.kind,fp.decs,fp.defval,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy((char*)val,fp.defval);
	}
	return(true);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bv300BaseAccessor::set_default_value(int f, void* val){
_bTrace_("bv300BaseAccessor::set_default_value",false);
	if(val==NULL){
		return(clear_default_value(f));
	}
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if(count_constraints(f)>0){
		if(get_constraint_index(f,val)<=0){
_te_("default value not in constraints");
			return(false);
		}
	}
int		k=(count_constraints(f)>0)?get_constraints_kind(f):fp.kind;
char	tmp[256];
	if((fp.kind==fp.cnstrkind)||(fp.cnstrkind==0)){
		if(!xToChar(k,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy(tmp,(char*)val);
	}

	if(_fld->WriteVal(fp.offset,kFLD_default_,tmp)){
_te_("_fld->WriteVal failed");
		return(false);
	}
	strncpy(fp.defval,tmp,256);
	if(!_elts.put(f,&fp)){
_te_("_elts.put failed");
		return(false);		
	}
	return(true);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bv300BaseAccessor::is_delprotected(int f){
_bTrace_("bv300BaseAccessor::is_delprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldDeleteLock_);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bv300BaseAccessor::set_delprotected(int f, bool b){
_bTrace_("bv300BaseAccessor::set_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
int	state=fp.state;
	if(b){
		if(!is_delprotected(f)){
			state|=_kFieldDeleteLock_;
		}
	}
	else{
		if(is_delprotected(f)){
			state^=_kFieldDeleteLock_;
		}
	}
	if(_fld->WriteVal(fp.offset,kFLD_state_,&state)){
_te_("_fld->WriteVal(kf,kFLD_state_,&state)");
		return(false);
	}
	fp.state=state;
	_elts.put(f,&fp);
	f_put_date(f);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::is_writeprotected(int f){
_bTrace_("bv300BaseAccessor::is_writeprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldEditLock_);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bv300BaseAccessor::set_writeprotected(int f, bool b){
_bTrace_("bv300BaseAccessor::set_writeprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
int	state=fp.state;
	if(b){
		if(!is_writeprotected(f)){
			state|=_kFieldEditLock_;
		}
	}
	else{
		if(is_writeprotected(f)){
			state^=_kFieldEditLock_;
		}
	}
	if(_fld->WriteVal(fp.offset,kFLD_state_,&state)){
_te_("_fld->WriteVal(kf,kFLD_state_,&state)");
		return(false);
	}
	fp.state=state;
	_elts.put(f,&fp);
	f_put_date(f);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::is_hidden(int f){
_bTrace_("bv300BaseAccessor::is_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldMasked_);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::set_hidden(int f, bool b){
_bTrace_("bv300BaseAccessor::set_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
int	state=fp.state;
	if(b){
		if(!is_hidden(f)){
			state|=_kFieldMasked_;
		}
	}
	else{
		if(is_hidden(f)){
			state^=_kFieldMasked_;
		}
	}
	if(_fld->WriteVal(fp.offset,kFLD_state_,&state)){
_te_("_fld->WriteVal(kf,kFLD_state_,&state)");
		return(false);
	}
	fp.state=state;
	_elts.put(f,&fp);
	f_put_date(f);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::is_dyn(int f){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::count_records(){
	return(_objs->CountRecords());
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::kill_record(int r){
	if(!_objs->KillRecord(r)){
		return(false);
	}
	o_put_date(r);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::unkill_record(int r){
	return(_objs->UnkillRecord(r));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::killed_record(int r){
	return(_objs->RecordKilled(r));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::h_read(int	o,	int	f,	void	*val){
	//hdr_open();
int	status=_hdr->ReadVal(o,f,val);
	//hdr_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv300BaseAccessor::h_write(int	o,	int	f,	void	*val){	
	//hdr_open();
int	status=_hdr->WriteVal(o,f,val);
	//hdr_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::count_param(const char* sub){
_bTrace_("bv300BaseAccessor::count_param",false);
bStdDirectory	root(_fd);
	if(root.status()){
_te_("root.status()");
		return(0);
	}
bStdDirectory	rsrc("Contents/Resources");
	if(rsrc.status()){
_te_("rsrc.status()");
		return(0);
	}
bStdDirectory	rsub(sub);
	if(rsub.status()){
_te_("rsub.status() 1");
		return(0);
	}
	rsub.mount();
	if(rsub.status()){
_te_("rsub.status() 2");
		return(0);
	}
int	k=rsub.count();
	return(k);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_param_name(const char* sub, int idx, char* name){	
bStdDirectory	root(_fd);
	if(root.status()){
		return(false);
	}
bStdDirectory	rsrc("Contents/Resources");
	if(rsrc.status()){
		return(false);
	}
bStdDirectory	rsub(sub);
	if(rsub.status()){
		return(false);
	}
	rsub.mount();
	if(rsub.status()){
		return(true);
	}
	rsub.name(idx,name);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::get_param(const char* sub, const char* name, void** p, int* sz){	
_bTrace_("bv300BaseAccessor::get_param",false);
bStdDirectory	root(_fd);
	if(root.status()){
_te_("root.status()");
		return(false);
	}
bStdDirectory	rsrc("Contents/Resources");
	if(rsrc.status()){
_te_("rsrc.status()");
		return(false);
	}
bStdDirectory	rsub(sub);
	if(rsub.status()){
_te_("rsub.status()");
		return(false);
	}
bStdFile		f(name,"r");
	if(f.status()){
_te_("f.status() 1");
		return(false);
	}
	f.mount((char**)p,sz);
	if(f.status()){
_te_("f.status() 2");
		return(false);
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::set_param(const char* sub, const char* name, void* p, int sz){	
bStdDirectory	root(_fd);
	if(root.status()){
		return(false);
	}
bStdDirectory	rsrc("Contents/Resources");
	if(rsrc.status()){
		return(false);
	}
_PUSH_
bStdDirectory	rsub(sub);
	if(rsub.status()){
mode_t			msk=S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH;
		if(mkdir(sub,msk)){
			return(false);
		}
	}
_POP_
bStdDirectory	rsub(sub);
	if(rsub.status()){
		return(false);
	}
	if((p!=NULL)&&(sz>0)){
bStdFile		f(name,"w");
		if(f.status()){
			return(false);
		}
		f.write(p,sz);
	}
	else{
bStdFile		f(name,"r");
		if(f.status()){
			return(false);
		}
		f.erase();
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::signature(){
	return('NULL');
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::f_put_date(int f){
field	fp;
	_elts.get(f,&fp);
	//fld_open();
	if(_fld->WriteVal(fp.offset,kFLD_MdID_,_user)){
		//fld_close();
		return;
	}
double	d=time(NULL);
	if(_fld->WriteVal(fp.offset,kFLD_MdDate_,&d)){
	}
	//fld_close();
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::o_put_date(int o){
	if(_objs->WriteVal(o,kOBJ_MdID_,_user)){
		return;
	}
double	d=time(NULL);
	if(_objs->WriteVal(o,kOBJ_MdDate_,&d)){
	}
}

// ---------------------------------------------------------------------------
// Adding Std fields to new table
// _hdr, _flds, _cnst not NULL
// -----------
int bv300BaseAccessor::std_add(){
_bTrace_("bv300BaseAccessor::std_add",_VERBOSE_);
	if(obj_add()){
		return(-50);
	}
	
	if(_fld->AddField("fid",_int,_kIntLen,0)==-1){
		return(-1);
	}
	if(_fld->AddField("crid",_char,30,0)==-1){
		return(-2);
	}
	if(_fld->AddField("crdate",_date,_kDateTimeLen,0)==-1){
		return(-3);
	}
	if(_fld->AddField("mdid",_char,30,0)==-1){
		return(-4);
	}
	if(_fld->AddField("mddate",_date,_kDateTimeLen,0)==-1){
		return(-5);
	}
	if(_fld->AddField("kind",_int,_kIntLen,0)==-1){
		return(-6);
	}
	if(_fld->AddField("size",_int,_kIntLen,0)==-1){
		return(-7);
	}
	if(_fld->AddField("length",_int,_kIntLen,0)==-1){
		return(-8);
	}
	if(_fld->AddField("decimals",_int,_kIntLen,0)==-1){
		return(-9);
	}
	if(_fld->AddField("state",_int,_kIntLen,0)==-1){
		return(-10);
	}
	if(_fld->AddField("name",_char,256,0)==-1){
		return(-11);
	}
	if(_fld->AddField("cnst_kind",_int,_kIntLen,0)==-1){
		return(-12);
	}
	if(_fld->AddField("def_val",_char,256,0)==-1){
		return(-14);
	}
	if(_fld->AddField("format",_char,256,0)==-1){
		return(-15);
	}
	if(_fld->AddField("index",_int,_kIntLen,0)==-1){
		return(-16);
	}
	
	if(_cnst->AddField("fid",_int,_kIntLen,0)==-1){
		return(-16);
	}
	if(_cnst->AddField("cindex",_int,_kIntLen,0)==-1){
		return(-17);
	}
	if(_cnst->AddField("cvalue",_char,256,0)==-1){
		return(-18);
	}	
	
char	cVal[256];
char	dVal[256];
int 	state=0;
	state|=_kFieldDeleteLock_;
	state|=_kFieldMasked_;
	
	// Identifiant
int 	k=1;
	strget(kMsgID,cVal);
	fld_write(	k, 
				-1,
				_int,
				sizeof(int),
				15,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
	
	k++;
	strget(kMsgCreator,cVal);
	fld_write(	k, 
				-2,
				_char,
				32,
				30,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
	
	k++;
	strget(kMsgDTOC,cVal);
	fld_write(	k, 
				-3,
				_date,
				sizeof(double),
				30,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
				
	k++;
	strget(kMsgModifier,cVal);
	fld_write(	k, 
				-4,
				_char,
				32,
				30,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
	
	k++;
	strget(kMsgDTOM,cVal);
	fld_write(	k, 
				-5,
				_date,
				sizeof(double),
				30,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
	// Flags
	k++;
	strget(kMsgState,cVal);
	fld_write(	k, 
				-6,
				_int,
				sizeof(int),
				15,
				0,
				state,
				cVal,
				0,
				"0",
				"",
				0);
	
	// Contour
	k++;
	strget(kMsgBounds,cVal);
	fld_write(	k, 
				-7,
				_ivxrect,
				sizeof(ivx_rect),
				15,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);

	// Segmentation
	k++;
	strget(kMsgGeom,cVal);
	fld_write(	k, 
				-8,
				_ivxs2,
				sizeof(DB_usz),
				15,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);
	
	state=0;
	state|=_kFieldDeleteLock_;
	
	// Nom
	k++;
	strget(kMsgName,cVal);
	fld_write(	k, 
				1,
				_char,
				64,
				50,
				0,
				state,
				cVal,
				0,
				"",
				"",
				0);	
	// SousType
	k++;
	strget(kMsgSubType,cVal);
	strget(kMsgSubTypeDefault,dVal);
	fld_write(	k, 
				2,
				_int,
				sizeof(int),
				50,
				0,
				state,
				cVal,
				_char,
				dVal,
				"",
				0);
	
	// Couleur
	k++;
	strget(kMsgColor,cVal);
	strget(kMsgBlack,dVal);
	fld_write(	k, 
				3,
				_int,
				sizeof(int),
				50,
				0,
				state,
				cVal,
				_char,
				dVal,
				"",
				0);
	
// Direction/Drapeau
	k++;
	if(	(_kind==kBaseKindPolyline)	||
		(_kind==kBaseKindPolygon)	||
		(_kind==kBaseKindRaster)	){
		strget(kMsgDirection,cVal);
	}
	else{
		strget(kMsgFlag,cVal);
	}
	fld_write(	k, 
				8,
				_int,
				sizeof(int),
				2,
				0,
				state,
				cVal,
				_int,
				"0",
				"",
				0);
	
int x;	
	k=1;
	x=2;
	(void)_cnst->WriteVal(k,kCNST_fid_,&x);
	x=1;
	(void)_cnst->WriteVal(k,kCNST_idx_,&x);
	strget(kMsgSubTypeDefault,cVal);
	(void)_cnst->WriteVal(k,kCNST_value_,cVal);

	
	k++;
	for(int i=k;i<k+8;i++){
		switch(i-k){
			case 0:
				strget(kMsgBlack,cVal);
				break;
			case 1:
				strget(kMsgWhite,cVal);
				break;
			case 2:
				strget(kMsgRed,cVal);
				break;
			case 3:
				strget(kMsgGreen,cVal);
				break;
			case 4:
				strget(kMsgBlue,cVal);
				break;
			case 5:
				strget(kMsgCyan,cVal);
				break;
			case 6:
				strget(kMsgMagenta,cVal);
				break;
			case 7:
				strget(kMsgYellow,cVal);
				break;
		}
		x=3;
		(void)_cnst->WriteVal(i,kCNST_fid_,&x);
		x=(i-k)+1;
		(void)_cnst->WriteVal(i,kCNST_idx_,&x);
		(void)_cnst->WriteVal(i,kCNST_value_,cVal);
	}
	
	k+=8;
	if(	(_kind==kBaseKindPolyline)	||
		(_kind==kBaseKindPolygon)	||
		(_kind==kBaseKindRaster)	){
		for(int i=k;i<k+3;i++){
			sprintf(cVal,"%d",i-(k+1));
			x=8;
			(void)_cnst->WriteVal(i,kCNST_fid_,&x);
			x=(i-k)+1;
			(void)_cnst->WriteVal(i,kCNST_idx_,&x);
			(void)_cnst->WriteVal(i,kCNST_value_,cVal);
		}
	}
	else{
		for(int i=k;i<k+2;i++){
			sprintf(cVal,"%d",i-k);
			x=8;
			(void)_cnst->WriteVal(i,kCNST_fid_,&x);
			x=(i-k)+1;
			(void)_cnst->WriteVal(i,kCNST_idx_,&x);
			(void)_cnst->WriteVal(i,kCNST_value_,cVal);
		}
	}
	
	_fid=8;
	if(_hdr->WriteVal(1,kHDR_fid_,&_fid)){
_te_("_hdr->WriteVal(1,kHDR_fid_,&_fid) failed");
		return(k);
	}

	return(0);
}

// ---------------------------------------------------------------------------
// Adding Std fields to new obj table
// -----------
int bv300BaseAccessor::obj_add(){
_bTrace_("bv300BaseAccessor::obj_adds",false);
	if(_objs->AddField("mmid",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for MMID");
		return(-1);
	}
	if(_objs->AddField("crid",_char,30,0)==-1){
_te_("_objs->AddField failed for CRID");
		return(-1);
	}
	if(_objs->AddField("crdate",_date,_kDateTimeLen,0)==-1){
_te_("_objs->AddField failed for CRDATE");
		return(-1);	
	}
	if(_objs->AddField("mdid",_char,30,0)==-1){
_te_("_objs->AddField failed for MDID");
		return(-1);
	}
	if(_objs->AddField("mddate",_date,_kDateTimeLen,0)==-1){
_te_("_objs->AddField failed for MDDATE");
		return(-1);
	}
	if(_objs->AddField("state",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for STATE");
		return(-1);
	}
	if(_objs->AddField("bounds",_ivxrect,_kIntLen*4,0)==-1){
_te_("_objs->AddField failed for BOUNDS");
		return(-1);
	}
	if(_objs->AddField("vertices",_ivxs2,0,_kind)==-1){
_te_("_objs->AddField failed for VERTICES");
		return(-1);
	}
	if(_objs->AddField("name",_char,50,0)==-1){
_te_("_objs->AddField failed for NAME");
		return(-1);
	}
	if(_objs->AddField("subtype",_int,50,0)==-1){
_te_("_objs->AddField failed for SUBTYPE");
		return(-1);
	}
	if(_objs->AddField("color",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for COLOR");
		return(-1);
	}
	if(	(_kind==kBaseKindPolyline)	||
		(_kind==kBaseKindPolygon)	||
		(_kind==kBaseKindRaster)	){
		if(_objs->AddField("direction",_int,2,0)==-1){
_te_("_objs->AddField failed for DIRECTION");
			return(-1);
		}
	}
	else{
		if(_objs->AddField("flag",_int,2,0)==-1){
_te_("_objs->AddField failed for FLAG");
			return(-1);
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// _hdr, _flds, _cnst not NULL
// -----------
int	bv300BaseAccessor::report_add(int kind, int len, int decs, const char *name){
_bTrace_("bv300BaseAccessor::report_add",false);
int	cnstrkind=0;
	switch(kind){
		case _ivxs2:
		case _ivxs3:
		case _dvxs2:
		case _dvxs3:
			if((decs<kBaseKindPoint)||(decs>kBaseKindRaster)){
				decs=_kind;
			}
			break;
		case _bool:
			cnstrkind=_bool;
			break;
		default:
			break;
	}
int	ko=_objs->CountFields();
int	sz;
	if(_objs->FieldSize(ko,&sz)){
_te_("_objs->FieldSize");	
		return(-4);
	}
int	kf=_fld->CountRecords()+1;
	if(kf<1){
_te_("_fld->CountRecords < 1");	
		return(-5);
	}
int	fid=_fid+1;
	if(_hdr->WriteVal(1,kHDR_fid_,&fid)){
_te_("_hdr->WriteVal(1,kHDR_fid_,&fid)");	
		return(-6);
	}
	if(_fld->WriteVal(kf,kFLD_id_,&fid)){
_te_("_fld->WriteVal(kf,kFLD_id,&fid)");	
		return(-7);
	}
	_fid++; 
	if(_fld->WriteVal(kf,kFLD_CrID_,_user)){
_te_("_fld->WriteVal(kf,kFLD_id,_user)");	
		return(-8);
	}
double	d=time(NULL);
	if(_fld->WriteVal(kf,kFLD_CrDate_,&d)){
_te_("_fld->WriteVal(kf,kFLD_id,&d)");	
		return(-9);
	}
	if(_fld->WriteVal(kf,kFLD_MdID_,_user)){
_te_("_fld->WriteVal(kf,kFLD_id,_user)");	
		return(-10);
	}
	if(_fld->WriteVal(kf,kFLD_MdDate_,&d)){
_te_("_fld->WriteVal(kf,kFLD_id,\"\")");	
		return(-11);
	}
	if(_fld->WriteVal(kf,kFLD_kind_,&kind)){
_te_("_fld->WriteVal(kf,kFLD_kind_,&kind)");	
		return(-12);
	}
	if(_fld->WriteVal(kf,kFLD_size_,&sz)){
_te_("_fld->WriteVal(kf,kFLD_size_,&sz)");	
		return(-13);
	}
	if(_fld->WriteVal(kf,kFLD_len_,&len)){
_te_("_fld->WriteVal(kf,kFLD_len_,&len)");
		return(-14);
	}
	if(_fld->WriteVal(kf,kFLD_decs_,&decs)){
_te_("_fld->WriteVal(kf,kFLD_decs_,&decs)");
		return(-15);
	}
int	state=0;
	if(_fld->WriteVal(kf,kFLD_state_,&state)){
_te_("_fld->WriteVal(kf,kFLD_state_,&state)");
		return(-16);
	}
	if(_fld->WriteVal(kf,kFLD_name_,(void*)name)){
_te_("_fld->WriteVal(kf,kFLD_name_,name)");
		return(-17);
	}
	if(_fld->WriteVal(kf,kFLD_constKind_,&cnstrkind)){
_te_("_fld->WriteVal(kf,kFLD_constKind_,&cnstrkind)");
		return(-18);
	}
	if(_fld->WriteVal(kf,kFLD_format_,(void*)"")){
_te_("_fld->WriteVal(kf,kFLD_format_,(void*)"")");
		return(-19);
	}
	if(_fld->WriteVal(kf,kFLD_index_,&state)){
_te_("_fld->WriteVal(kf,kFLD_index_,&state)");
		return(-20);
	}

field	f;

	/*switch(kind){
		case _bool:
		case _int:
		case _double:
			sprintf(f.defval,"0");
			if(_fld->WriteVal(kf,kFLD_default_,f.defval)){
_te_("_fld->WriteVal(kf,kFLD_default_,f.defval)");
				return(-21);
			}
			break;
		default:
			sprintf(f.defval,"");
			break;
	}*/

	f.offset=kf;
	f.fid=fid;
	f.kind=kind;
	f.size=sz;
	f.len=len;
	f.decs=decs;
	f.state=state;
	strncpy(f.name,name,256);
	f.cnstrkind=cnstrkind;
	f.defval[0]=0;
#if _HAS_FORMAT_
	f.format[0]=0;
#endif
	f.index=0;
	
	if(!_elts.add(&f)){
_te_("_elts.add(&f)");
		return(-22);
	}
		
	if(f.kind==_bool){
		kf=_elts.count();
		ko=0;
		if(!add_constraint(kf,_bool,&ko,false)){
_te_("add_constraint 0");
			return(-23);
		}
		ko=1;
		if(!add_constraint(kf,_bool,&ko,false)){
_te_("add_constraint 1");
			return(-24);
		}
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::load(){
_bTrace_("bv300BaseAccessor::load",false);
int		status;
	if((status=load_infos())){
_te_("load_infos() failed with "+status);
		return(status);
	}
	if((status=load_fields())){
_te_("load_fields() failed with "+status);
		return(status);
	}
	if((status=load_constraints())){
_te_("load_constraints() failed with "+status);
		return(status);
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::load_fields(){
_bTrace_("bv300BaseAccessor::load_fields",false);
int		status=noErr;
field	f;
int		n=_fld->CountRecords();

	for(int i=1;i<=n;i++){
		f.name[0]=0;
		f.cnstrkind=0;
		f.defval[0]=0;
#if _HAS_FORMAT_
		f.format[0]=0;
#endif
		if((status=_fld->ReadVal(i,kFLD_id_,&f.fid))){
			break;
		}
		if(_fid<f.fid){
_te_("database corrupted : last fid "+_fid+", will be set to "+f.fid);
			if((status=_hdr->WriteVal(1,kHDR_fid_,&f.fid))){
_te_("database corrupted : _hdr->WriteVal failed with "+status);
				break;
			}
			_fid=f.fid;
		}
		if(_fld->RecordKilled(i)){
			continue;
		}
		f.offset=i;
		if((status=_fld->ReadVal(i,kFLD_kind_,&f.kind))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_size_,&f.size))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_len_,&f.len))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_decs_,&f.decs))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_state_,&f.state))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_name_,f.name))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_constKind_,&f.cnstrkind))){
			break;
		}
		if((status=_fld->ReadVal(i,kFLD_default_,f.defval))){
			break;
		}
#if _HAS_FORMAT_
		if((status=_fld->ReadVal(i,kFLD_format_,f.format))){
			break;
		}
#endif
		if((status=_fld->ReadVal(i,kFLD_index_,&f.index))){
			//break;
			status=0;
		}
		if(!_elts.add(&f)){
			status=-1;
			break;
		}
		if(_VERBOSE_){
_tm_("field "+i+" ->"+f.name+" ("+(UInt32*)&f.kind+") id="+f.fid);
_tm_("field "+i+" ->size="+f.size+" len="+f.len+" decs="+f.decs);
_tm_("field "+i+" ->default value="+f.defval);
		}
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::load_constraints(){
_bTrace_("bv300BaseAccessor::load_constraints",false);
int		status=noErr;
cnst	c;
int		n=_cnst->CountRecords();
	for(int i=1;i<=n;i++){
		c.offset=i;
		c.idx=0;
		c.fid=0;
		c.val[0]=0;
		if(_cnst->RecordKilled(i)){
			continue;
		}
		if((status=_cnst->ReadVal(i,kCNST_fid_,&c.fid))){
_te_("_cnst->ReadVal("+i+","+kCNST_fid_+",x)");
			return(-1);
		}
		if((status=_cnst->ReadVal(i,kCNST_idx_,&c.idx))){
_te_("_cnst->ReadVal("+i+","+kCNST_idx_+",x)");
			return(-1);
		}
		if((status=_cnst->ReadVal(i,kCNST_value_,c.val))){
_te_("_cnst->ReadVal("+i+","+kCNST_value_+",x)");
			return(-1);
		}
		if(!_celts.add(&c)){
_te_("_celts.add for "+i);
			return(-1);
		}
		if(_VERBOSE_){
_tm_("-> id="+c.fid+" index="+c.idx+" value="+c.val);
		}
	}
	_celts.sort(constraint_comp);
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::load_infos(){
_bTrace_("bv300BaseAccessor::load_infos",false);
int		status;
	if((status=_hdr->ReadVal(1,kHDR_fid_,&_fid))){
_te_("_hdr->ReadVal(1,kCNST_fid_,&_fid) failed with "+status);
		return(-1);
	}
	if((status=_hdr->ReadVal(1,kHDR_oid_,&_oid))){
_te_("_hdr->ReadVal(1,kHDR_oid_,&_oid) failed with "+status);
		return(-2);
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::fld_write(	int k, 
									int id,
									int kind,
									int size,
									int len,
									int decs,
									int state,
									const char* name,
									int cntKind,
									const char* def,
									const char* fmt,
									int index){
double	d=time(NULL);
	(void)_fld->WriteVal(k,kFLD_id_,&id);
	(void)_fld->WriteVal(k,kFLD_CrID_,_user);
	(void)_fld->WriteVal(k,kFLD_CrDate_,&d);
	(void)_fld->WriteVal(k,kFLD_MdID_,_user);
	d=0;
	(void)_fld->WriteVal(k,kFLD_MdDate_,&d);
	(void)_fld->WriteVal(k,kFLD_kind_,&kind);
	(void)_fld->WriteVal(k,kFLD_size_,&size);
	(void)_fld->WriteVal(k,kFLD_len_,&len);
	(void)_fld->WriteVal(k,kFLD_decs_,&decs);
	(void)_fld->WriteVal(k,kFLD_state_,&state);
	(void)_fld->WriteVal(k,kFLD_name_,(void*)name);
	(void)_fld->WriteVal(k,kFLD_constKind_,&cntKind);
	(void)_fld->WriteVal(k,kFLD_default_,(void*)def);
	(void)_fld->WriteVal(k,kFLD_format_,(void*)fmt);
	(void)_fld->WriteVal(k,kFLD_index_,&index);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::h_new(const char* name){
_bTrace_("bv300BaseAccessor::h_new",false);
int k=0;
	k--;
	if(_hdr->AddField("tid",_double,_kDoubleLen,0)==-1){
_te_("hdr->AddField failed for ID");
		return(k);
	}
	k--;
	if(_hdr->AddField("crid",_char,30,0)==-1){
_te_("hdr->AddField failed for CRID");
		return(k);
	}
	k--;
	if(_hdr->AddField("crdate",_date,_kDateTimeLen,0)==-1){
_te_("hdr->AddField failed for CRDATE");
		return(k);
	}
	k--;
	if(_hdr->AddField("state",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for STATE");
		return(k);
	}
	k--;
	if(_hdr->AddField("kind",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for KIND");
		return(k);
	}
	k--;
	if(_hdr->AddField("bounds",_ivxrect,_kIntLen*4,0)==-1){
_te_("hdr->AddField failed for BOUNDS");
		return(k);
	}
	k--;
	if(_hdr->AddField("name",_char,50,0)==-1){
_te_("hdr->AddField failed for NAME");
		return(k);
	}
	k--;
	if(_hdr->AddField("precision",_double,_kDoubleLen,10)==-1){
_te_("hdr->AddField failed for PRECISION");
		return(k);
	}
	k--;
	if(_hdr->AddField("unit2m",_double,_kDoubleLen,10)==-1){
_te_("hdr->AddField failed for UNIT2M");
		return(k);
	}
	k--;
	if(_hdr->AddField("unitshortlib",_char,11,0)==-1){
_te_("hdr->AddField failed for UNITSHORTLIB");
		return(k);	}
	k--;
	if(_hdr->AddField("unitlonglib",_char,31,0)==-1){
_te_("hdr->AddField failed for UNITLONGLIB");
		return(k);
	}
	k--;
	if(_hdr->AddField("nolive",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for NOLIVE");
		return(k);
	}
	k--;
	if(_hdr->AddField("nokilled",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for NOKILLED");
		return(k);
	}
	k--;
	if(_hdr->AddField("srid",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for SRID");
		return(k);
	}
	k--;
	if(_hdr->AddField("mfid",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for FID");
		return(k);
	}
	k--;
	if(_hdr->AddField("moid",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for OID");
		return(k);
	}

double			d=time(NULL);
int				x=0;
ivx_rect		vxr={-__BOUNDS_MAX__,-__BOUNDS_MAX__,__BOUNDS_MAX__,__BOUNDS_MAX__};
char			buff[256]="UNDEF";

	k--;
	if(_hdr->WriteVal(1,kHDR_id_,&d)){
_te_("_hdr->WriteVal(1,kHDR_id_,&d) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_CrID_,_user)){
_te_("_hdr->WriteVal(1,kHDR_CrID_,_user) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_CrDate_,&d)){
_te_("_hdr->WriteVal(1,kHDR_CrDate_,&d) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_state_,&x)){
_te_("_hdr->WriteVal(1,kHDR_state_,&d) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_kind_,&_kind)){
_te_("_hdr->WriteVal(1,kHDR_kind_,&_kind) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_bounds_,&vxr)){
_te_("_hdr->WriteVal(1,kHDR_bounds_,&vxr) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_name_,(void*)name)){
_te_("_hdr->WriteVal(1,kHDR_name_,name) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_precs_,&_reso)){
_te_("_hdr->WriteVal(1,kHDR_precs_,&_reso) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_unit2m_,&_u2m)){
_te_("_hdr->WriteVal(1,kHDR_unit2m_,&_u2m) failed");
		return(k);
	}
	sprintf(buff,"UNDEF");
	k--;
	if(_hdr->WriteVal(1,kHDR_shortLib_,(void*)buff)){
_te_("_hdr->WriteVal(1,kHDR_shortLib_,buff) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_longLib_,(void*)buff)){
_te_("_hdr->WriteVal(1,kHDR_longLib_,buff) failed");
		return(k);
	}
	x=0;
	k--;
	if(_hdr->WriteVal(1,kHDR_alive_,&x)){
_te_("_hdr->WriteVal(1,kHDR_alive_,&x) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_killed_,&x)){
_te_("_hdr->WriteVal(1,kHDR_killed_,&x) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_srid_,&_srid)){
_te_("_hdr->WriteVal(1,kHDR_srid_,&_srid) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_fid_,&_fid)){
_te_("_hdr->WriteVal(1,kHDR_fid_,&_fid) failed");
		return(k);
	}
	if(_hdr->WriteVal(1,kHDR_oid_,&_oid)){
_te_("_hdr->WriteVal(1,kHDR_oid_,&_oid) failed");
		return(k);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::h_init(){
_bTrace_("bv300BaseAccessor::h_init",false);
	if(_hdr->ReadVal(1,kHDR_kind_,&_kind)){
_te_("_hdr->ReadVal(1,kHDR_kind_,&_kind) failed");
		return(-1);
	}
	if(_hdr->ReadVal(1,kHDR_precs_,&_reso)){
_te_("_hdr->ReadVal(1,kHDR_precs_,&_reso) failed");
		return(-2);
	}
_tm_("_reso="+_reso);
	if(_hdr->ReadVal(1,kHDR_unit2m_,&_u2m)){
_te_("_hdr->ReadVal(1,kHDR_unit2m_,&_u2m) failed");
		return(-3);
	}
	if(_hdr->ReadVal(1,kHDR_srid_,&_srid)){
_te_("_hdr->ReadVal(1,kHDR_srid_,&_srid) failed");
		return(-4);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::read_name(const char* path, char* name){
_bTrace_("bv300BaseAccessor::read_name",false);
int		last=strlen(path)-1;
int		i;

	for(i=last;i>0;i--){
		if(path[i-1]=='/'){
			break;
		}
	}
	if((i==0)&&(path[0]=='/')){
		i=1;
	}
	
/*char* l=strrchr(path,'/');
	l=l?++l:path;*/
	memmove(name,&path[i],last-i);
	name[last-i]=0;
	//strcpy(name,l);
char* buf=strstr(name,".typ");
	if(buf){
		name[buf-name]=0;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::read_data(	char* dbname,
									char* host,
									int* port,
									char* user,
									char* password){
_bTrace_("bv300BaseAccessor::read_data()",false);
	dbname[0]=0;
	host[0]=0;
	*port=0;
	user[0]=0;
	password[0]=0;
bStdFile		fp("data.txt","r");
	if(fp.status()){
_te_("fp.status=="+fp.status()+" (data.txt)");
		return;
	}
char	dat[256];
	
	if(fp.next_line(dat,256)){
		strncpy(dbname,dat,256);
	}
	if(fp.next_line(dat,256)){
		strncpy(host,dat,256);
	}
	if(fp.next_line(dat,256)){
		*port=atoi(dat);
	}
	if(fp.next_line(dat,256)){
		strncpy(user,dat,256);
	}
	if(fp.next_line(dat,256)){
		strncpy(password,dat,256);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::read_data(	char* dbname,
									char* host,
									int* port,
									char* user,
									char* password,
									const char* path){
bTrace			trc("bPGisBaseAccessor::read_data(path)",false);
bStdDirectory	d(path);
bStdDirectory	d2("Contents/Datas");
	return(read_data(dbname,host,port,user,password));
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::append(){
_bTrace_("bv300BaseAccessor::append",false);
int	o=_objs->CountRecords()+1;
int	oid=_oid+1;
int status=0;

	if((status=_objs->WriteVal(o,kOBJ_ID_,&oid))){
_te_("_objs->WriteVal error "+status+" for record "+o+" field "+kOBJ_ID_);
		return(status);
	}
	_oid++;
	if((status=_hdr->WriteVal(1,kHDR_oid_,&_oid))){
_te_("_hdr->WriteVal error "+status+" for oid");
		return(status);
	}
double t=time(NULL);
	if(_objs->WriteVal(o,kFLD_CrID_,_user)){
_te_("_objs->WriteVal error for creation id field");					
	}
	if(_objs->WriteVal(o,kFLD_CrDate_,&t)){
_te_("_objs->WriteVal error for creation date field");					
	}
	o_put_date(o);

char	buff[256];
int		idx;
field	fp;
	for(int i=kOBJ_State_;i<=_elts.count();i++){
		_elts.get(i,&fp);
		if(get_default_value(i,buff)){
			if((fp.kind==fp.cnstrkind)||(fp.cnstrkind==0)){
				if(_objs->WriteVal(o,i,buff)){
_te_("_objs->WriteVal (default value) error for field "+i);					
					return(-3);
				}
			}
			else{
				idx=get_constraint_index(i,buff);
				if(idx<=0){
_te_("default value not in constraints for field "+i);
					return(-3);
				}
				if(_objs->WriteVal(o,i,&idx)){
_te_("WriteVal (default value) error for field "+i);
					return(-3);
				}
			}
		}
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::set_constraint(int f, int n, void* val, bool check){
_bTrace_("bv300BaseAccessor::set_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if(check){
		if((fp.kind==_bool)&&check){
_te_("could not set constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not set constraint on standard field");
			return(false);
		}
	}
	if(get_constraint_index(f,val)>0){
_te_("constraint already exist");
		return(false);
	}
cnst	c;
	c.fid=fp.fid;
	c.idx=n;
int		k=_celts.search(&c,constraint_comp);
	if(!k){
_te_("constraint not found for field="+f+" and index="+n);
		return(false);
	}
	if(!_celts.get(k,&c)){
_te_("_celts->get failed for "+k);
		return(false);
	}
char	prev[256];
	strncpy(prev,c.val,255);
char	tmp[256];
	if(fp.kind==fp.cnstrkind){
		if(nb_live()>0){
			if(!check_for_val(f,val)){
_te_("could not set constraint : type not empty");
				return(false);
			}
		}
		if(!xToChar(fp.cnstrkind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy(tmp,(char*)val);
	}
	
	if(_cnst->WriteVal(c.offset,kCNST_value_,tmp)){
_te_("_cnst->WriteVal failed");
		return(false);
	}

	strncpy(c.val,tmp,256);
	if(!_celts.put(k,&c)){
_te_("_celts->put failed for "+k);
		return(false);
	}
	if(_VERBOSE_){
_tm_(prev+"->"+fp.defval);
	}
	if(strcmp(prev,fp.defval)==0){
		if(!set_default_value(f,val)){
_te_("set_default_value failed : database corrupted");
			return(false);
		}
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::add_constraint(int f, int kind, void* val, bool check){
_bTrace_("bv300BaseAccessor::add_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if((nb_live()>0)&&(fp.index!=-1)){//= field deleted
_te_("could not remove constraint : type not empty");
		return(false);
	}

	if(check){
		if((fp.kind==_bool)&&(fp.index!=-1)){//= field deleted
_te_("could not remove constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not remove constraint on standard field");
			return(false);
		}
		if((f==kOBJ_SubType_)&&(count_constraints(f)==1)){
_te_("subtype must have one constraint");
			return(false);
		}
	}

cnst	c;
	c.fid=fp.fid;
	c.idx=count_constraints(f)+1;
	c.offset=_cnst->CountRecords()+1;
	c.val[0]=0;
	
	if(c.idx==1){// first time
		if(nb_live()>0){
			if(!check_for_val(f,val)){
_te_("could not set constraint : type not empty");
				return(false);
			}
		}
		if(_fld->WriteVal(fp.offset,kFLD_constKind_,&fp.kind)){
_te_("_fld->WriteVal failed");
			return(false);
		}
		fp.cnstrkind=fp.kind;
		if(!_elts.put(f,&fp)){
_te_("_elts.put failed");
			return(false);
		}
		if(!set_default_value(f,val)){
_te_("set_default_value failed");
			return(false);
		}
	}
	else{
		if(get_constraint_index(f,val)>0){
_te_("constraint already exist");
			return(false);
		}
	}
	
	if(fp.kind==fp.cnstrkind){
		if(!xToChar(fp.cnstrkind,fp.decs,val,c.val)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
			return(false);
		}
	}
	else{
		strcpy(c.val,(char*)val);
	}

	if(_cnst->WriteVal(c.offset,kCNST_fid_,&c.fid)){
_te_("_cnst->WriteVal failed at fid");
		return(false);
	}
	if(_cnst->WriteVal(c.offset,kCNST_idx_,&c.idx)){
_te_("_cnst->WriteVal failed at idx");
		return(false);
	}
	if(_cnst->WriteVal(c.offset,kCNST_value_,c.val)){
_te_("_cnst->WriteVal failed at value");
		return(false);
	}

	if(!_celts.add(&c)){
_te_("_celts->add failed");
		return(false);
	}
	_celts.sort(constraint_comp);
	
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::rmv_constraint(int f, int n, bool check){
_bTrace_("bv300BaseAccessor::rmv_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if((nb_live()>0)&&(fp.index!=-1)){//= field deleted
_te_("could not remove constraint : type not empty");
		return(false);
	}
	if(check){
		if((fp.kind==_bool)&&(fp.index!=-1)){//= field deleted
_te_("could not remove constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not remove constraint on standard field");
			return(false);
		}
		if((f==kOBJ_SubType_)&&(count_constraints(f)==1)){
_te_("subtype must have one constraint");
			return(false);
		}
	}
cnst	c;
	c.fid=fp.fid;
	c.idx=n;
int		k=_celts.search(&c,constraint_comp);
	if(!k){
_te_("constraint not found for field="+f+" and index="+n);
		return(false);
	}
	if(!_celts.get(k,&c)){
_te_("_celts.get failed");
		return(false);
	}
	if(!_celts.rmv(k)){
_te_("_celts.rmv failed");
		return(false);
	}
	if(_cnst->KillRecord(c.offset)){
_te_("_cnst->KillRecord failed");
		return(false);
	}
cnst	c2=c;
	for(int i=k;i<=_celts.count();i++){
		_celts.get(i,&c2);
		if(c2.fid!=fp.fid){
			break;
		}
		c2.idx--;
		if(_cnst->WriteVal(c2.offset,kCNST_idx_,&c2.idx)){
_te_("_cnst->WriteVal failed at idx");
			return(false);
		}
		_celts.put(i,&c2);
	}
char	buff[256];
	if(get_constraint(f,1,buff)){
		if(strcmp(c.val,fp.defval)){
			if(!set_default_value(f,buff)){
_te_("set_default_value failed : database corrupted");
				return(false);
			}
		}
	}
	else{
		fp.cnstrkind=0;
		if(_fld->WriteVal(fp.offset,kFLD_constKind_,&fp.cnstrkind)){
_te_("_field->WriteVal failed at kFLD_constKind_");
			return(false);
		}
		if(!_elts.put(f,&fp)){
_te_("_elts.put failed for "+f);
			return(false);
		}
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::check_for_val(int f, void* val){
_bTrace_("bv300BaseAccessor::check_for_val",false);
	if(_objs->CountRecords()==0){
		return(true);
	}
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
void* buff=malloc(fp.size);
//trc.msg("size=%d",fp.size);
	for(int i=1;i<=_objs->CountRecords();i++){
		if(_objs->RecordKilled(i)){
			continue;
		}
		_objs->ReadVal(i,f,buff);
		switch(fp.kind){
			case _binary:{
void*	bbuff=(void*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					free(bbuff);
					free(buff);
					return(false);
				}
				free(bbuff);
			}	break;
			case _ivxs2:
			case _ivxs3:{
ivertices*	bbuff=(ivertices*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					ivs_free(bbuff);
					free(buff);
					return(false);
				}
				ivs_free(bbuff);
			}	break;
			case _dvxs2:
			case _dvxs3:{
dvertices*	bbuff=(dvertices*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					dvs_free(bbuff);
					free(buff);
					return(false);
				}
				dvs_free(bbuff);
			}	break;
			default:
				if(memcmp(val,buff,fp.size)){
					free(buff);
					return(false);
				}
				break;
		}
		
	}
	free(buff);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::clear_default_value(int f){
_bTrace_("bv300BaseAccessor::clear_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+_elts.count()+"]");
		return(false);
	}
	if(count_constraints(f)>0){
_te_("could not remove default value : field "+f+" has constraints");
		return(false);
	}
char	tmp[1]="";
	if(_fld->WriteVal(fp.offset,kFLD_default_,tmp)){
_te_("_fld->WriteVal failed");
		return(false);
	}
	fp.defval[0]=0;
	if(!_elts.put(f,&fp)){
_te_("_elts.put failed");
		return(false);		
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv300BaseAccessor::is_lock(){
_bTrace_("bv300BaseAccessor::is_lock",false);
int		x=0;
	if(_hdr->ReadVal(1,kHDR_state_,&x)){
_te_("_hdr->ReadVal(1,kHDR_state_,&x) failed");
		return(true);
	}
// A faire	
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::nb_live(){
_bTrace_("bv300BaseAccessor::nb_live",false);
int	x=0;
	if(_hdr->ReadVal(1,kHDR_alive_,&x)){
_te_("_hdr->ReadVal(1,kHDR_alive_,&x) failed");
		return(-1);
	}
	return(x);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::constraint_comp(const void* a, const void* b){
cnst*	ca=(cnst*)a;
cnst*	cb=(cnst*)b;
	if(ca->fid!=cb->fid){
		return(ca->fid-cb->fid);
	}
	return(ca->idx-cb->idx);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::init_user(){
	if(_user){
		return;
	}
struct passwd *pwp=getpwuid(getuid());
	if(pwp){
		_user=strdup(pwp->pw_name);
	}
	else{
		_user=strdup("");
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bv300BaseAccessor::make_package(int sign, const char* path, const char* name, const char* data){
_bTrace_("bv300BaseAccessor::make_package",_VERBOSE_);
mode_t	msk=S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH;
char	hname[FILENAME_MAX];

bStdDirectory	root(path);

// Convertir en UTF8 pour mkdir
	sprintf(hname,"%s.type",name);
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,hname,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,hname,FILENAME_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	if(mkdir(hname,msk)){
_te_("mkdir failed for "+hname);
		return(false);
	}

// Reprendre en MacRoman pour bStdDirectory
	sprintf(hname,"%s.type",name);
bStdDirectory bse(hname);
		
	if(mkdir("Contents",msk)){
_te_("mkdir failed for Contents");
		return(false);
	}
bStdDirectory cnt("Contents");
	if(mkdir("Datas",msk)){
_te_("mkdir failed for Datas");
		return(false);
	}
	if(mkdir("Resources",msk)){
_te_("mkdir failed for Resources");
		return(false);
	}
	if(mkdir("Resources disabled",msk)){
_te_("mkdir failed for Resources disabled");
		return(false);
	}
_PUSH_
bStdFile	store("Datas/storage.txt","w");
char		buf[256];
	sprintf(buf,"%d",sign);
	store.write(buf);
_POP_
_PUSH_
bStdFile	info("PkgInfo","w");
	info.write("dataMap™");
_POP_

	if(data){
bStdFile dt("Datas/data.txt","w");
		if(dt.status()){
_te_("status = "+dt.status()+" at bStdFile");
			return(false);
		}
		dt.write(data);
	}
	
// Dossiers standards
bStdDirectory rsrc("Resources");
	if(mkdir("styles",msk)){
_te_("mkdir failed for styles");
		return(false);
	}
	if(mkdir("patterns",msk)){
_te_("mkdir failed for patterns");
		return(false);
	}
	if(mkdir("icons",msk)){
_te_("mkdir failed for icons");
		return(false);
	}
	if(mkdir("dashes",msk)){
_te_("mkdir failed for dashes");
		return(false);
	}
	return(true);
}
/*
// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::fld_open(){
_bTrace_("bv300BaseAccessor::fld_open",true);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv300BaseAccessor::cnt_open(){
_bTrace_("bv300BaseAccessor::cnt_open",true);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::fld_close(){
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv300BaseAccessor::cnt_close(){
}*/
