//----------------------------------------------------------------------------
// File : bv310BaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Database base class version 3.1.0
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
#include "bv310BaseAccessor.h"
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
// -----------
static int constraint_comp(const void* a, const void* b){
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
static int field_comp(const void* a, const void* b){
field*	fa=(field*)a;
field*	fb=(field*)b;
	return(fa->fid-fb->fid);
}

// ---------------------------------------------------------------------------
// 
// ------------
char*	bv310BaseAccessor::_user=NULL;

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bv310BaseAccessor	::bv310BaseAccessor(const char* hpath,  
										int wanted_srid,
										double wanted_reso,
										int* status)
					:bStdBaseAccessor()
					,_elts(sizeof(field))
					,_celts(sizeof(cnst))
					,_tables(sizeof(bv310FieldAccessor*)){
_bTrace_("bv310BaseAccessor::bv310BaseAccessor (open)",_VERBOSE_);
	*status=noErr;
	_fid=0;
	_oid=0;
	_lid=1;
	_fld=NULL;
	_objs=NULL;
	_hdr=NULL;
	_cnst=NULL;
	_lnks=NULL;
	
	_kind=0;
	_srid=0;
	_reso=0;
	_u2m=0;
	_state=0;
	_vers=_kVersion310;

	_frp=NULL;
	_top=NULL;
	if(wanted_srid>0){
bool		b;
		_top=new bStdProj(wanted_srid,&b);
		if(!b){
_te_("unknown srid (wanted):"+wanted_srid);
			delete _top;
			_top=NULL;
		}
	}
	
	if(_user==NULL){
		init_user();
	}
	_wanted_reso=wanted_reso;
						
char		fn[PATH_MAX];
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
// A la création, on part du principe que l'on n'a pas de re-projection
// à effectuer
// ------------
bv310BaseAccessor	::bv310BaseAccessor(const char* hpath,  
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
					,_celts(sizeof(cnst))
					,_tables(sizeof(bStdTable*)){
_bTrace_("bv310BaseAccessor::bv310BaseAccessor (create)",_VERBOSE_);
	*status=noErr;
	_fid=0;
	_oid=0;
	_lid=1;
	_fld=NULL;
	_objs=NULL;
	_hdr=NULL;
	_cnst=NULL;
	_lnks=NULL;

	_kind=kind;
	_srid=srid;
	_reso=reso;
	_u2m=u2m;
	_state=0;
	_vers=_kVersion310;

	_frp=NULL;
	_top=NULL;

	if(_user==NULL){
		init_user();
	}
	
	if(!make_package(hkind,hpath,name,data)){
		*status=-1;
	}
	_wanted_reso=0;
	
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
bv310BaseAccessor::~bv310BaseAccessor(){
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
	if(_fd!=-1){	
		close(_fd);
		_fd=-1;
	}
	_elts.reset();
	_celts.reset();
	if(_frp){
		delete _frp;
		_frp=NULL;
	}
	if(_top){
		delete _top;
		_top=NULL;
	}
}

// ---------------------------------------------------------------------------
// DYN OK
// -----------
int	bv310BaseAccessor::read(int o, int f, void* val){
_bTrace_("bv310BaseAccessor::read",false);
int		status;
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(-1);
	}

//_tm_("f=%d, fp.index=%d",f,fp.index);
	if((status=fp.tbl->read(o,fp.index,val,this,static_read))){
//_te_("_objs->ReadVal failed with "+status+" for("+o+";"+fp.index+")");
		return(status);
	}
	if(f==kOBJ_ID_){
		if((*((int*)val))>_oid){
_te_("database corrupted : last oid "+_oid+", will be set to "+(*((int*)val)));
			if((status=_hdr->WriteVal(1,kHDR_oid_,val))){
_te_("database corrupted : _hdr->WriteVal failed with "+status);
				return(-1);
			}
			_oid=(*((int*)val));
		}
	}
double		wr=(_wanted_reso!=0)?_wanted_reso:1;
	
	if(_frp&&_top){
//_tm_("reprojection");
		switch(fp.kind){
			case _ivxs2:
				proj_vxs((*(ivertices**)val),(_reso*_u2m),_frp,_top);
				if(_wanted_reso!=0){
					scale_vxs((*(ivertices**)val),(_reso*_u2m)/wr);
				}
				break;
			case _ivxrect:
				proj_vxr((ivx_rect*)val,(_reso*_u2m),_frp,_top);
				if(_wanted_reso!=0){
					scale_vxr((ivx_rect*)val,(_reso*_u2m)/wr);
				}
				break;
		}
	}
	else if(_wanted_reso!=0){
//_tm_("mise à l'échelle "+_wanted_reso);
		switch(fp.kind){
			case _ivxs2:
				scale_vxs((*(ivertices**)val),(_reso*_u2m)/wr);
				break;
			case _ivxrect:
				scale_vxr((ivx_rect*)val,(_reso*_u2m)/wr);
				break;
		}		
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// DYN OK
// -----------
int	bv310BaseAccessor::write(int o, int f, void* val){
_bTrace_("bv310BaseAccessor::write",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(-1);
	}
	if(is_lock()){
_te_("type is lock");
		return(-2);
	}
	if(fp.state&_kFieldEditLock_){
_te_("field "+f+" protected");
		return(-2);
	}
	if(fp.state&_kFieldDyn_){// ne pas écrire dans les tables liées
_te_("field "+f+" is external");
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

//_tm_("field : %.4s, constraint : %.4s",&fp.kind,&fp.cnstrkind);
//_tm_("constraint kind : %d",fp.cnstrkind);
	if((fp.cnstrkind!=0)&&(fp.cnstrkind==fp.kind)){
		if(get_constraint_index(f,val)<=0){
			if(count_constraints(f)>0){
_te_("natural constraint not found for field "+f);
				return(-5);
			}
		}
	}
	else if((fp.cnstrkind!=0)&&(fp.cnstrkind==_bit)){
// Values could be outside of constraint range -> pass
	}
	else if((fp.cnstrkind!=0)&&(fp.cnstrkind!=fp.kind)){
		if(((*((int*)val))<1)||((*((int*)val))>count_constraints(f))){
_te_("classe constraint not found for field "+f);
			return(-6);
		}
	}
	
	
int			status;
ivx_rect	vxr;
double		wr=(_wanted_reso!=0)?_wanted_reso:1;
	
	if(_frp&&_top){
		switch(fp.kind){
			case _ivxs2:{
ivertices* vxs;
				status=ivs2ivs((*(ivertices**)val),&vxs);
				if(vxs==NULL){
_te_("ivs2ivs error "+status+" for record "+o+" field "+f);
					return(status);
				}
				if(_wanted_reso!=0){
					scale_vxs(vxs,1.0/((_reso*_u2m)/wr));
				}
				proj_vxs(vxs,(_reso*_u2m),_top,_frp);
				if((status=_objs->WriteVal(o,fp.index,&vxs))){
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					ivs_free(vxs);
					return(status);
				}
				if(f==kOBJ_Vertices_){
					ivs_bounds(vxs,&vxr);
					if((status=_objs->WriteVal(o,kOBJ_Rect_,&vxr))){
_te_("WriteVal error "+status+" for record "+o+" field kOBJ_Rect_");
						ivs_free(vxs);
						return(status);
					}
				}
				ivs_free(vxs);
				}break;
			case _ivxrect:{
				vxr=*(ivx_rect*)val;
				if(_wanted_reso!=0){
					scale_vxr(&vxr,1.0/((_reso*_u2m)/wr));
				}
				proj_vxr(&vxr,(_reso*_u2m),_top,_frp);
				if((status=_objs->WriteVal(o,fp.index,&vxr))){
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					return(status);
				}				
				}break;
			default:
				if((status=_objs->WriteVal(o,fp.index,val))){
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					return(status);
				}
				break;
		}
	}
	else if(_wanted_reso!=0){
		switch(fp.kind){
			case _ivxs2:{
				scale_vxs((*(ivertices**)val),1.0/((_reso*_u2m)/wr));
				if((status=_objs->WriteVal(o,fp.index,val))){
					scale_vxs((*(ivertices**)val),((_reso*_u2m)/wr));
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					return(status);
				}
				if(f==kOBJ_Vertices_){
					ivs_bounds((*(ivertices**)val),&vxr);
					if((status=_objs->WriteVal(o,kOBJ_Rect_,&vxr))){
						scale_vxs((*(ivertices**)val),((_reso*_u2m)/wr));
_te_("WriteVal error "+status+" for record "+o+" field kOBJ_Rect_");
						return(status);
					}
				}
				scale_vxs((*(ivertices**)val),((_reso*_u2m)/wr));
			}break;
			case _ivxrect:{
				scale_vxr((ivx_rect*)val,1.0/((_reso*_u2m)/wr));
				if((status=_objs->WriteVal(o,fp.index,&vxr))){
					scale_vxr((ivx_rect*)val,((_reso*_u2m)/wr));
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					return(status);
				}				
				scale_vxr((ivx_rect*)val,((_reso*_u2m)/wr));
			}break;
			default:
				if((status=_objs->WriteVal(o,fp.index,val))){
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
					return(status);
				}
				break;
		}
	}	
	else{
		if((status=_objs->WriteVal(o,fp.index,val))){
_te_("WriteVal failed with "+status+" for ("+o+";"+fp.index+")");
			return(status);
		}
		if(f==kOBJ_Vertices_){
			ivs_bounds((*(ivertices**)val),&vxr);
			if((status=_objs->WriteVal(o,kOBJ_Rect_,&vxr))){
_te_("WriteVal error "+status+" for record "+o+" field kOBJ_Rect_");
				return(status);
			}
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
int	bv310BaseAccessor::count(){
	return(_elts.count());
}

// ---------------------------------------------------------------------------
// pas dans les tables externes
// -----------
int	bv310BaseAccessor::add(int kind, int len, int decs,	const char *name){
_bTrace_("bv310BaseAccessor::add",false);
	if(is_lock()){
_te_("type is lock");
		return(-1);
	}
	if(get_index(name)>0){
_te_("name "+name+" found in database");
		return(-2);
	}
int status;
	if((status=_objs->AddField(name,kind,len,decs))){
_te_("_objs->AddField failed with "+status);
		return(-3);
	}
	//hdr_open();
	//fld_open();
	//cnt_open();
    status=report_add(kind,len,decs,name);
	//cnt_close();
	//fld_close();
	//hdr_close();
	return(status);
}

// ---------------------------------------------------------------------------
// USE FLD
// DYN OK
// -----------
int	bv310BaseAccessor::modify(int f, int kind, int len, int decs, const char* name){
_bTrace_("bv310BaseAccessor::modify",false);
	if(is_lock()){
_te_("type is lock");
		return(-1);
	}
	if(is_dyn(f)){// ne pas modifier les tables liées
_te_("field "+f+" is dynamic");
		return(-1);
	}
	if(is_dyn_ref(f)){// ne pas modifier un champ référence
_te_("field "+f+" is dynamic reference");
		return(-1);
	}

	if(f<=kOBJ_Dir_){
_te_("could not modify standard field");
		return(-2);
	}	
field	fp;
int		status;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(-3);
	}
	if((status=_objs->ChgField(fp.index,name,kind,len,decs))){
_te_("_objs->ChgField = "+status);
		return(-4);
	}
int		sz;
	if((status=_objs->FieldSize(fp.index,&sz))){
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
// DYN OK
// -----------
int	bv310BaseAccessor::remove(int f){
_bTrace_("bv310BaseAccessor::remove",true);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(-1);
	}
	if(is_delprotected(f)){
_te_("field "+f+" is del protected");
		return(-2);
	}
	if(is_lock()){
_te_("type is lock");
		return(-3);
	}
	if(is_dyn_ref(f)){// Au cas ou...
_te_("field "+f+" is dynamic reference");
		return(-4);
	}
bool	dyn=is_dyn(f);
	if(!dyn){
		if(fp.index<=kOBJ_Dir_){
_te_("could not remove standard field");
			return(-5);
		}
		if(_objs->RmvField(fp.index/*f*/)){
_te_("_objs->RmvField("+f+") failed");
			return(-6);
		}
	}
	f_put_date(f);
int	bkindex=fp.index;
	fp.index=-1;
	if(_fld->WriteVal(fp.offset,kFLD_index_,&fp.index)){
_te_("_fld->WriteVal("+fp.offset+",kFLD_index_"+fp.index+") failed");
		return(-7);
	}
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
		return(-8);
	}
field				fp2;
// Décrémenter les index de champs dans la même table liée
	for(int i=1;i<=_elts.count();i++){
		_elts.get(i,&fp2);
		if((fp2.tbl!=fp.tbl)||(fp2.index<=bkindex)){
			continue;
		}
		if(fp2.index==-1){
			continue;
		}
		fp2.index--;
		if(_fld->WriteVal(fp2.offset,kFLD_index_,&fp2.index)){
_te_("_fld->WriteVal("+fp2.offset+",kFLD_index_"+fp2.index+") failed");
			return(-9);
		}		
		_elts.put(i,&fp2);
	}
	if(!_elts.rmv(f)){
_te_("_elts.rmv failed");
		return(-10);
	}

bv310FieldAccessor*		facc;
	if(!dyn){
// Décrémenter les index de table liée
int	x;
		for(int i=1;i<=_tables.count();i++){
			_tables.get(i,&facc);
			if(facc->get_id()==1){
				continue;
			}
			if(facc->get_ref()>bkindex){
				facc->remove();
				x=facc->get_ref();
				lnk_open();
				_lnks->WriteVal(facc->get_offset(),kLNK_bfield_,&x);
				lnk_close();
			}
		}
	}
	else{
// Vérification tables liées, champ référence
		check_dyn();
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::get_kind(int	f,	int	*k){
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
bool bv310BaseAccessor::get_len(int	f,	int	*l){
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
bool bv310BaseAccessor::get_decs(int f,	int	*d){
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
bool bv310BaseAccessor::get_name(int f,	char* name){
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
bool bv310BaseAccessor::get_size(int f, int* sz){
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
int	bv310BaseAccessor::get_id(int f){
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
int	bv310BaseAccessor::get_id(const char* name){
	int	f=get_index(name);
	if(!f){
		return(0);
	}
	return(get_id(f));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310BaseAccessor::get_index(int fid){
int		n=count();
field	fp;	
	
	for(int i=1;i<=n;i++){
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
int	bv310BaseAccessor::get_index(const char* name){
int		n=count();
field	fp;	
char	n1[256],n2[256];
	
	strncpy(n1,name,256);
	strupper(n1);
	for(int i=1;i<=n;i++){
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
int	bv310BaseAccessor::count_constraints(int f){
_bTrace_("bv310BaseAccessor::count_constraints",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
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
int bv310BaseAccessor::get_constraints_kind(int f){
_bTrace_("bv310BaseAccessor::get_constraints_kind",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(0);
	}
	return(fp.cnstrkind);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::get_constraint(int f, int n, void* val){
_bTrace_("bv310BaseAccessor::get_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
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
bool bv310BaseAccessor::set_constraint(int f, int n, void* val){
	return(set_constraint(f,n,val,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::add_constraint(int f, int kind, void* val){
	return(add_constraint(f,kind,val,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::rmv_constraint(int f, int n){
	return(rmv_constraint(f,n,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::get_constraint_index(int f, void* val){
_bTrace_("bv310BaseAccessor::get_constraint_index",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
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
bool bv310BaseAccessor::set_constraint_index(int f, int from, int to){
_bTrace_("bv310BaseAccessor::set_constraint_index",false);
_te_("not implemented");
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::has_default_value(int f){
_bTrace_("bv310BaseAccessor::has_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(strlen(fp.defval)>0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::get_default_value(int f, void* val){
_bTrace_("bv310BaseAccessor::get_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(strlen(fp.defval)<=0){
		return(false);
	}
	if(fp.cnstrkind==0){
		if(!charToX(fp.kind,fp.decs,fp.defval,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
			return(false);
		}
	}
	else{
		if(fp.cnstrkind==_bit){
			if(!charToX(fp.kind,fp.decs,fp.defval,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
				return(false);
			}
		}
		else if(fp.cnstrkind==fp.kind){
			if(!charToX(fp.kind,fp.decs,fp.defval,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
				return(false);
			}
		}
		else{
			if(!charToX(fp.cnstrkind,fp.decs,fp.defval,val)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
				return(false);
			}
		}
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_default_value(int f, void* val){
	return(set_default_value(f,val,true));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::is_delprotected(int f){
_bTrace_("bv310BaseAccessor::is_delprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldDeleteLock_);
}

// ---------------------------------------------------------------------------
// USE _FLD
// DYN OK
// -----------
bool bv310BaseAccessor::set_delprotected(int f, bool b){
_bTrace_("bv310BaseAccessor::set_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn_ref(f)){
_te_("field "+f+" is dynamic reference");
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
bool bv310BaseAccessor::is_writeprotected(int f){
_bTrace_("bv310BaseAccessor::is_writeprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldEditLock_);
}

// ---------------------------------------------------------------------------
// USE _FLD
// DYN OK
// -----------
bool bv310BaseAccessor::set_writeprotected(int f, bool b){
_bTrace_("bv310BaseAccessor::set_writeprotected",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
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
bool bv310BaseAccessor::is_hidden(int f){
_bTrace_("bv310BaseAccessor::is_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldMasked_);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_hidden(int f, bool b){
_bTrace_("bv310BaseAccessor::set_hidden",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
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
bool bv310BaseAccessor::is_dyn(int f){
_bTrace_("bv310BaseAccessor::is_dyn",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldDyn_);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_dyn_ref(int f, bool b){
_bTrace_("bv310BaseAccessor::set_dyn_ref",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_dyn(f)){
_te_("could not set dynamic reference on dynamic field "+f);
		return(false);
	}
int	state=fp.state;
	if(b){
		if(!is_dyn_ref(f)){
			state|=_kFieldDynRef_;
		}
	}
	else{
		if(is_dyn_ref(f)){
			state^=_kFieldDynRef_;
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
bool bv310BaseAccessor::is_dyn_ref(int f){
_bTrace_("bv310BaseAccessor::is_dyn_ref",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	return(fp.state&_kFieldDynRef_);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::count_records(){
	return(_objs->CountRecords());
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::kill_record(int r){
	if(is_lock()){
		return(false);
	}
	if(!_objs->KillRecord(r)){
		return(false);
	}
	o_put_date(r);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::unkill_record(int r){
	if(is_lock()){
		return(false);
	}
	return(_objs->UnkillRecord(r));
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::killed_record(int r){
	return(_objs->RecordKilled(r));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310BaseAccessor::h_read(int	o,	int	f,	void	*val){
	//hdr_open();
int	status=_hdr->ReadVal(o,f,val);
	//hdr_close();
	if(f==kHDR_bounds_){
double		wr=(_wanted_reso!=0)?_wanted_reso:1;
		if(_frp&&_top){
			proj_vxr((ivx_rect*)val,(_reso*_u2m),_frp,_top);
			if(_wanted_reso!=0){
				scale_vxr((ivx_rect*)val,(_reso*_u2m)/wr);
			}
		}
		else if(_wanted_reso!=0){
			scale_vxr((ivx_rect*)val,(_reso*_u2m)/wr);
		}
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310BaseAccessor::h_write(int	o,	int	f,	void	*val){	
	//hdr_open();
	if((is_lock())&&(f!=kHDR_state_)){
		return(-1);
	}
int	status;
	if(f==kHDR_bounds_){
ivx_rect	vxr=*(ivx_rect*)val;
double		wr=(_wanted_reso!=0)?_wanted_reso:1;
		if((_frp&&_top)&&(f==kHDR_bounds_)){
			if(_wanted_reso!=0){
				scale_vxr(&vxr,1.0/((_reso*_u2m)/wr));
			}
			proj_vxr(&vxr,(_reso*_u2m),_frp,_top);
		}
		else if(_wanted_reso!=0){
			scale_vxr(&vxr,1.0/((_reso*_u2m)*wr));
		}
		status=_hdr->WriteVal(o,f,&vxr);
	}
	else{
		status=_hdr->WriteVal(o,f,val);
		if(f==kHDR_state_){
			_state=*((int*)val);
		}
	}
	//hdr_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::count_param(const char* sub){
_bTrace_("bv310BaseAccessor::count_param",false);
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
_tw_("rsub.status() 1");
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
bool bv310BaseAccessor::get_param_name(const char* sub, int idx, char* name){	
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
bool bv310BaseAccessor::get_param(const char* sub, const char* name, void** p, int* sz){	
_bTrace_("bv310BaseAccessor::get_param",false);
bStdDirectory	root(_fd);
	if(root.status()){
_te_("root.status() "+sub+" : "+name);
		return(false);
	}
bStdDirectory	rsrc("Contents/Resources");
	if(rsrc.status()){
_te_("rsrc.status() "+sub+" : "+name);
		return(false);
	}
bStdDirectory	rsub(sub);
	if(rsub.status()){
_tw_("rsub.status() "+sub+" : "+name);
		return(false);
	}
bStdFile		f(name,"r");
	if(f.status()){
_tw_("f.status() 1 "+sub+" : "+name);
		return(false);
	}
	f.mount((char**)p,sz);
	if(f.status()){
_te_("f.status() 2 "+sub+" : "+name);
		return(false);
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_param(const char* sub, const char* name, void* p, int sz){	
	if(is_lock()){
		return(false);
	}
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
int bv310BaseAccessor::add_dyn(const char* path, const char* name, int tbl_f, int bse_f){
_bTrace_("bv310BaseAccessor::add_link",true);
	if(is_lock()){
_te_("type is lock ");
		return(-1);
	}
int						status=noErr;
bv310DynFieldAccessor*	acc;
char					lnam[FILENAME_MAX],ext[FILENAME_MAX],fnam[_FIELDNAME_MAX_];
char					cwd[PATH_MAX],rpt[PATH_MAX];
char*					p;

_tm_("path="+path);
	if((status=lnk_open())){
_te_("lnk_open failed with "+status);
		return(-1);
	}

bStdDirectory	bdr(_fd);
	if(!getcwd(cwd,PATH_MAX)){
_te_("getcwd failed");
		strcpy(rpt,path);
	}
	else{
_tm_("cwd path="+cwd);
char	tpt[PATH_MAX];
		strcat(cwd,"/");
		sprintf(tpt,"%s%s",path,name);
		relpath(cwd,tpt,rpt);
		p=strrchr(rpt,'/');
		if(p){
			p[1]=0;
		}
	}

_tm_("relative path="+rpt);

	if(is_dyn(bse_f)){
_te_("could not add dynamic reference on dynamic field "+bse_f);
		lnk_close();
		return(-1);
	}

	strcpy(lnam,name);
	p=strrchr(lnam,'.');
	if(!p){
_te_("no extension for table "+name);
		lnk_close();
		return(-1);
	}
	strcpy(ext,&p[1]);
	p[0]=0;
	if(strlen(lnam)==0){
_te_("bad table name "+name);
		lnk_close();
		return(-1);
	}
	
	for(int i=1;i<=_tables.count();i++){
		_tables.get(i,&acc);
		if(strcmp(acc->get_name(),lnam)==0){
_te_("table already linked "+name);
			lnk_close();
			return(-1);
		}
	}
	
wtable	tbl=wtbl_alloc(	ext2sign(ext),
						rpt,
						name,
						false, 
						1,
						-1,
						&status);
	if(status<0){
_te_("wtbl_alloc failed with "+status);
_te_("path : "+rpt);
_te_("name : "+name);
		lnk_close();
		return(status);
	}
	
int	lid=_lid+1;
	if((status=_hdr->WriteVal(1,kHDR_lid_,&lid))){
_te_("_hdr->WriteVal(1,kHDR_lid_,&lid) failed with "+status);
		lnk_close();
		return(status);
	}
	_lid++;
	
int nx=_lnks->CountRecords()+1;
	
	if((status=_lnks->WriteVal(nx,kLNK_id_,&_lid))){
_te_("_lnks->WriteVal(nx,kLNK_id_,_lid) failed with "+status);
		_lnks->KillRecord(nx);
		lnk_close();
		return(status);
	}
	if((status=_lnks->WriteVal(nx,kLNK_path_,rpt))){
_te_("_lnks->WriteVal(nx,kLNK_path_,rpt) failed with "+status);
		_lnks->KillRecord(nx);
		lnk_close();
		return(status);
	}
	if((status=_lnks->WriteVal(nx,kLNK_name_,(void*)name))){
_te_("_lnks->WriteVal(nx,kLNK_name_,name) failed with "+status);
		_lnks->KillRecord(nx);
		lnk_close();
		return(status);
	}
	tbl->FieldName(tbl_f,fnam);
	if((status=_lnks->WriteVal(nx,kLNK_tfield_,fnam))){
_te_("_lnks->WriteVal(nx,kLNK_tfield_,fnam) failed with "+status);
		_lnks->KillRecord(nx);
		lnk_close();
		return(status);
	}
	if((status=_lnks->WriteVal(nx,kLNK_bfield_,&bse_f))){
_te_("_lnks->WriteVal(nx,kLNK_bfield_,bse_f) failed with "+status);
		_lnks->KillRecord(nx);
		lnk_close();
		return(status);
	}
	
int	k,d,sz;
	
	get_kind(bse_f,&k);
	get_size(bse_f,&sz);
	
_tm_("base ref field ="+bse_f);

bv310DynFieldAccessor*	dyn=new bv310DynFieldAccessor(tbl,lnam,_lid,bse_f,nx,tbl_f,k,sz);	
	_tables.add(&dyn);

	
int		siz,len;
char	fullnam[_FIELDNAME_MAX_];

	
	if(!is_dyn_ref(bse_f)){
		set_dyn_ref(bse_f,true);
	}

CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,lnam,kCFStringEncodingUTF8);
	CFStringGetCString(cfs,lnam,FILENAME_MAX,kCFStringEncodingMacRoman);
	CFRelease(cfs);

	for(int i=1;i<=tbl->CountFields();i++){
		if(i==tbl_f){
			continue;
		}
		tbl->FieldSign(i,&k);
		tbl->FieldName(i,fnam);
		tbl->FieldDecimals(i,&d);
		tbl->FieldLength(i,&len);
		tbl->FieldSize(i,&siz);
		strcpy(fullnam,lnam);
		strcat(fullnam,".");
		strcat(fullnam,fnam);
		report_linked(_lid,i,siz,k,len,d,fullnam);
_tm_(fullnam);
	}
	
	
	lnk_close();
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::pack(){
_bTrace_("bv310BaseAccessor::pack",true);
//_tm_("packing _fld");	
//	_fld->Pack();
//_tm_("packing _objs");
int	x;
_tm_("old n="+_objs->CountRecords());
	if(_objs->Pack()==0){
		x=0;
		h_write(1,kHDR_killed_,&x);
		x=count_records();
		h_write(1,kHDR_alive_,&x);
	}
_tm_("new n="+_objs->CountRecords());
//_tm_("packing _hdr");	
//	_hdr->Pack();
//_tm_("packing _cnst");	
//	_cnst->Pack();
//	lnk_open();
//_tm_("packing _lnks");	
//	_lnks->Pack();
//	lnk_close();	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::signature(){
	return('NULL');
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::f_put_date(int f){
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
void bv310BaseAccessor::o_put_date(int o){
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
int bv310BaseAccessor::std_add(){
_bTrace_("bv310BaseAccessor::std_add",_VERBOSE_);
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
	if(_fld->AddField("tbl_id",_int,_kIntLen,0)==-1){
		return(-17);
	}
	
	if(_cnst->AddField("fid",_int,_kIntLen,0)==-1){
		return(-18);
	}
	if(_cnst->AddField("cindex",_int,_kIntLen,0)==-1){
		return(-19);
	}
	if(_cnst->AddField("cvalue",_char,256,0)==-1){
		return(-20);
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
				0,
				1);
	
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
				0,
				1);
	
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
				0,
				1);
				
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
				0,
				1);
	
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
				0,
				1);
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
				0,
				1);
	
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
				0,
				1);

	state=0;
	state|=_kFieldDeleteLock_;

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
				0,
				1);
		
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
				0,
				1);	
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
				0,
				1);
	
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
				0,
				1);
	
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
				0,
				1);
	
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
int bv310BaseAccessor::obj_add(){
_bTrace_("bv310BaseAccessor::obj_adds",false);
	if(_objs->AddField("mmid",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for MMID");
		return(-1);
	}
	if(_objs->AddField("mmcrid",_char,30,0)==-1){
_te_("_objs->AddField failed for CRID");
		return(-1);
	}
	if(_objs->AddField("mmcrdate",_date,_kDateTimeLen,0)==-1){
_te_("_objs->AddField failed for CRDATE");
		return(-1);	
	}
	if(_objs->AddField("mmmdid",_char,30,0)==-1){
_te_("_objs->AddField failed for MDID");
		return(-1);
	}
	if(_objs->AddField("mmmddate",_date,_kDateTimeLen,0)==-1){
_te_("_objs->AddField failed for MDDATE");
		return(-1);
	}
	if(_objs->AddField("mmstate",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for STATE");
		return(-1);
	}
	if(_objs->AddField("mmbounds",_ivxrect,_kIntLen*4,0)==-1){
_te_("_objs->AddField failed for BOUNDS");
		return(-1);
	}
	if(_objs->AddField("mmvertices",_ivxs2,0,_kind)==-1){
_te_("_objs->AddField failed for VERTICES");
		return(-1);
	}
	if(_objs->AddField("mmname",_char,50,0)==-1){
_te_("_objs->AddField failed for NAME");
		return(-1);
	}
	if(_objs->AddField("mmsubtype",_int,50,0)==-1){
_te_("_objs->AddField failed for SUBTYPE");
		return(-1);
	}
	if(_objs->AddField("mmcolor",_int,_kIntLen,0)==-1){
_te_("_objs->AddField failed for COLOR");
		return(-1);
	}
	if(	(_kind==kBaseKindPolyline)	||
		(_kind==kBaseKindPolygon)	||
		(_kind==kBaseKindRaster)	){
		if(_objs->AddField("mmdirection",_int,2,0)==-1){
_te_("_objs->AddField failed for DIRECTION");
			return(-1);
		}
	}
	else{
		if(_objs->AddField("mmflag",_int,2,0)==-1){
_te_("_objs->AddField failed for FLAG");
			return(-1);
		}
	}
	/*if(_objs->AddField("crid",_char,30,0)==-1){
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
	}*/
	return(0);
}

// ---------------------------------------------------------------------------
// _hdr, _flds, _cnst not NULL
// -----------
int	bv310BaseAccessor::report_add(int kind, int len, int decs, const char *name){
_bTrace_("bv310BaseAccessor::report_add",false);
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
int	tid=1;
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
	if(_fld->WriteVal(kf,kFLD_index_,&ko)){
_te_("_fld->WriteVal(kf,kFLD_index_,&state)");
		return(-20);
	}
	if(_fld->WriteVal(kf,kFLD_tbl_id_,&tid)){
_te_("_fld->WriteVal(kf,kFLD_index_,&state)");
		return(-21);
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
	f.index=ko;
	f.tbl_id=tid;
	f.tbl=get_facc(tid);
	
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
// _hdr, _flds, _cnst not NULL
// -----------
int	bv310BaseAccessor::report_linked(int tid, int idx, int siz, int kind, int len, int decs, const char *name){
_bTrace_("bv310BaseAccessor::report_add",false);
	switch(kind){
		case _ivxs2:
		case _ivxs3:
		case _dvxs2:
		case _dvxs3:
			return(-1);
			break;
	}
bv310FieldAccessor*	acc=get_facc(tid);
	if(acc==NULL){
		return(-2);
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
	if(_fld->WriteVal(kf,kFLD_size_,&siz)){
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
int	state=_kFieldEditLock_+_kFieldDyn_;
	if(_fld->WriteVal(kf,kFLD_state_,&state)){
_te_("_fld->WriteVal(kf,kFLD_state_,&state)");
		return(-16);
	}
	if(_fld->WriteVal(kf,kFLD_name_,(void*)name)){
_te_("_fld->WriteVal(kf,kFLD_name_,name)");
		return(-17);
	}
int cnstrkind=0;
	if(_fld->WriteVal(kf,kFLD_constKind_,&cnstrkind)){
_te_("_fld->WriteVal(kf,kFLD_constKind_,&cnstrkind)");
		return(-18);
	}
	if(_fld->WriteVal(kf,kFLD_format_,(void*)"")){
_te_("_fld->WriteVal(kf,kFLD_format_,(void*)"")");
		return(-19);
	}
	if(_fld->WriteVal(kf,kFLD_index_,&idx)){
_te_("_fld->WriteVal(kf,kFLD_index_,&state)");
		return(-20);
	}
	if(_fld->WriteVal(kf,kFLD_tbl_id_,&tid)){
_te_("_fld->WriteVal(kf,kFLD_tbl_id_,&tid)");
		return(-21);
	}

field	f;

	f.offset=kf;
	f.fid=fid;
	f.kind=kind;
	f.size=siz;
	f.len=len;
	f.decs=decs;
	f.state=state;
	strncpy(f.name,name,256);
	f.cnstrkind=0;
	f.defval[0]=0;
#if _HAS_FORMAT_
	f.format[0]=0;
#endif
	f.index=idx;
	f.tbl_id=tid;
	f.tbl=acc;
	
	if(!_elts.add(&f)){
_te_("_elts.add(&f)");
		return(-22);
	}
		
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::load(int wanted_srid){
_bTrace_("bv310BaseAccessor::load",false);
int		status;
	if((status=load_infos())){
_te_("load_infos() failed with "+status);
		return(status);
	}
	if((status=load_dyn())){
_te_("load_dyn() failed with "+status);
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
	
// Verification : cohérence valeur par défaut, contraintes de saisie
field	f;
cnst	c;
int		k;
bool	fnd;

	for(int i=1;i<=_elts.count();i++){
		_elts.get(i,&f);
		fnd=false;
		if((f.cnstrkind!=0)&&(f.cnstrkind!=_bit)){
			if(count_constraints(i)==0){
				f.cnstrkind=0;
				_elts.put(i,&f);
				_fld->WriteVal(f.offset,kFLD_constKind_,&f.cnstrkind);
			}
			else{
				c.fid=f.fid;
				c.idx=1;
				k=_celts.search(&c,constraint_comp);
				if(!k){
					continue;
				}
				_celts.get(k,&c);
				if(strlen(f.defval)==0){
					strcpy(f.defval,c.val);
					_elts.put(i,&f);
					_fld->WriteVal(f.offset,kFLD_default_,f.defval);
				}
				else{
					for(int j=k;j<=_celts.count();j++){
						_celts.get(j,&c);
						if(c.fid!=f.fid){
							break;
						}
						if(strcmp(c.val,f.defval)==0){
							fnd=true;
							break;
						}
					}
					if(!fnd){
						_celts.get(k,&c);
						strcpy(f.defval,c.val);
						_elts.put(i,&f);
						_fld->WriteVal(f.offset,kFLD_default_,f.defval);
					}
				}
			}
		}
	}
	
// Verification : tables liées, champs liés, champs référence
	check_dyn();

// Projection
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
bool		b;
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
int bv310BaseAccessor::load_fields(){
_bTrace_("bv310BaseAccessor::load_fields",false);
int		status=noErr;
field	f;
int		k=1,n=_fld->CountRecords();
char	str[256];

// 1) on ajoute la table initiale
bv310StaFieldAccessor*	acc=new bv310StaFieldAccessor(_objs,0);
	_tables.add(&acc);
	f.tbl=acc;
	

#pragma mark upgrade 3.0-3.1
	if(_fld->CountFields()<kFLD_tbl_id_){
//_tm_("(_fld->CountFields()<kFLD_tbl_id_), upgrade 3.0-3.1");
		if(_fld->AddField("tbl_id",_int,_kIntLen,0)==-1){
			return(-1);
		}
char	cVal[_FIELDNAME_MAX_];
		strget(kMsgID,cVal);
		_fld->WriteVal(kOBJ_ID_,kFLD_name_,cVal);
		strget(kMsgCreator,cVal);
		_fld->WriteVal(kOBJ_CrID_,kFLD_name_,cVal);
		strget(kMsgDTOC,cVal);
		_fld->WriteVal(kOBJ_CrDate_,kFLD_name_,cVal);
		strget(kMsgModifier,cVal);
		_fld->WriteVal(kOBJ_MdID_,kFLD_name_,cVal);
		strget(kMsgDTOM,cVal);
		_fld->WriteVal(kOBJ_MdDate_,kFLD_name_,cVal);
		strget(kMsgState,cVal);
		_fld->WriteVal(kOBJ_State_,kFLD_name_,cVal);
		strget(kMsgBounds,cVal);
		_fld->WriteVal(kOBJ_Rect_,kFLD_name_,cVal);
		strget(kMsgGeom,cVal);
		_fld->WriteVal(kOBJ_Vertices_,kFLD_name_,cVal);
		
		f.tbl_id=1;
		for(int i=1;i<=n;i++){
			if((status=_fld->WriteVal(i,kFLD_tbl_id_,&f.tbl_id))){
_te_("_fld->WriteVal(i,kFLD_tbl_id,&f.tbl_id) failed");
				return(-2);
			}
		}
	}
		
#pragma mark upgrade 3.1-3.4	
	_objs->FieldName(kOBJ_Vertices_,str);
	if(strcmp(str,"vertices")==0){
_tm_("UPGRADE INFO : Modifying objects table names");
		if(_objs->ChgField(kOBJ_CrID_,"mmcrid",_char,30,0)!=0){
_te_("_objs->ChgField failed for CRID");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_CrDate_,"mmcrdate",_date,_kDateTimeLen,0)!=0){
_te_("_objs->ChgField failed for CRDATE");
			return(-3);	
		}
		if(_objs->ChgField(kOBJ_MdID_,"mmmdid",_char,30,0)!=0){
_te_("_objs->ChgField failed for MDID");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_MdDate_,"mmmddate",_date,_kDateTimeLen,0)!=0){
_te_("_objs->ChgField failed for MDDATE");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_State_,"mmstate",_int,_kIntLen,0)!=0){
_te_("_objs->ChgField failed for STATE");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_Rect_,"mmbounds",_ivxrect,_kIntLen*4,0)!=0){
_te_("_objs->ChgField failed for BOUNDS");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_Vertices_,"mmvertices",_ivxs2,0,_kind)!=0){
_te_("_objs->ChgField failed for VERTICES");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_Name_,"mmname",_char,50,0)!=0){
_te_("_objs->ChgField failed for NAME");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_SubType_,"mmsubtype",_int,50,0)!=0){
_te_("_objs->ChgField failed for SUBTYPE");
			return(-3);
		}
		if(_objs->ChgField(kOBJ_Color_,"mmcolor",_int,_kIntLen,0)!=0){
_te_("_objs->ChgField failed for COLOR");
			return(-3);
		}
		if(	(_kind==kBaseKindPolyline)	||
			(_kind==kBaseKindPolygon)	||
			(_kind==kBaseKindRaster)	){
			if(_objs->ChgField(kOBJ_Dir_,"mmdirection",_int,2,0)!=0){
_te_("_objs->ChgField failed for DIRECTION");
				return(-3);
			}
		}
		else{
			if(_objs->ChgField(kOBJ_Dir_,"mmflag",_int,2,0)!=0){
_te_("_objs->ChgField failed for FLAG");
				return(-3);
			}
		}
	}
	
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
_te_("database corrupted : last fid ("+_fid+"), will be set to "+f.fid);
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
		if(f.index==0){
//_tm_("(f.index==0), upgrade 3.0-3.1");
			f.index=k;
			if((status=_fld->WriteVal(i,kFLD_index_,&f.index))){
_te_("_fld->WriteVal(i,kFLD_index_,&f.index) failed");
				status=0;
			}
		}
		else if(f.index==-1){
_tm_("erreur, f.index==-1 for field "+f.name);
#warning TEMPORAIRE->pour régler un pb de dev
/**/
			f.index=k;
			if((status=_fld->WriteVal(i,kFLD_index_,&f.index))){
_te_("_fld->WriteVal(i,kFLD_index_,&f.index) failed");
				status=0;
			}
/**/
		}
		
		if((status=_fld->ReadVal(i,kFLD_tbl_id_,&f.tbl_id))){
			break;
		}
		if(f.tbl_id==0){
_te_("(f.tbl_id==0), upgrade 3.0-3.1");
			f.tbl_id=1;
			if((status=_fld->WriteVal(i,kFLD_index_,&f.index))){
_te_("_fld->WriteVal(i,kFLD_index_,&f.index) failed");
				status=0;
			}
		}

		f.tbl=get_facc(f.tbl_id);
		
		if(f.tbl==NULL){
_te_("linked table "+f.tbl_id+" not found for field "+f.name);
		//	continue;
		}
		
		if(!_elts.add(&f)){
			status=-1;
			break;
		}
		if(_VERBOSE_){
/*_tm_("field "+i+" ->"+f.name+" ("+(UInt32*)&f.kind+") id="+f.fid);
_tm_("field "+i+" ->size="+f.size+" len="+f.len+" decs="+f.decs);
_tm_("field "+i+" ->default value="+f.defval);
_tm_("field "+i+" ->tbl index="+f.index+", tbl name="+(char*)f.tbl->get_name());*/
		}
		k++;
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::load_constraints(){
_bTrace_("bv310BaseAccessor::load_constraints",false);
int		status=noErr;
cnst	c;
int		k,n=_cnst->CountRecords();
field	f;

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
		f.fid=c.fid;
		k=_elts.search(&f,field_comp);
		if(!k){
_tm_("field "+c.fid+" not found, constraint deleted");
			_cnst->KillRecord(i);
			continue;
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
//_tm_("-> id="+c.fid+" index="+c.idx+" value="+c.val);
		}
	}
	_celts.sort(constraint_comp);
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::load_infos(){
_bTrace_("bv310BaseAccessor::load_infos",false);
int		status;
	if((status=_hdr->ReadVal(1,kHDR_fid_,&_fid))){
_te_("_hdr->ReadVal(1,kCNST_fid_,&_fid) failed with "+status);
		return(-1);
	}
	if((status=_hdr->ReadVal(1,kHDR_oid_,&_oid))){
_te_("_hdr->ReadVal(1,kHDR_oid_,&_oid) failed with "+status);
		return(-2);
	}

//3.1
#pragma mark upgrade 3.0-3.1
	if(_hdr->CountFields()<kHDR_lid_){
_tm_("(_hdr->CountFields()<kHDR_lid_), upgrade 3.0-3.1");
		if(_hdr->AddField("mlid",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for LID");
			return(-3);
		}
		if((status=_hdr->WriteVal(1,kHDR_lid_,&_lid))){
_te_("_hdr->WriteVal(1,kHDR_lid_,&_lid) failed with "+status);
		return(-4);
		}
	}
	if((status=_hdr->ReadVal(1,kHDR_lid_,&_lid))){
_te_("_hdr->ReadVal(1,kHDR_lid_,&_lid) failed with "+status);
		return(-5);
	}
	if((status=_hdr->ReadVal(1,kHDR_state_,&_state))){
_te_("_hdr->ReadVal(1,kHDR_state_,&_state) failed with "+status);
		return(-6);
	}
	return(noErr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::load_dyn(){
_bTrace_("bv310BaseAccessor::load_dyn",false);
int		status=noErr;
	if((status=lnk_open())){
_te_("lnk_open failed with "+status);
	}
	if(_lnks->CountFields()<4){
//_te_("adding fields to _lnks");
		_lnks->AddField("tbl_id",_int,_kIntLen,0);
		_lnks->AddField("path",_char,FILENAME_MAX,0);
		_lnks->AddField("name",_char,PATH_MAX,0);
		_lnks->AddField("tbl_fld_name",_char,_FIELDNAME_MAX_,0);
		_lnks->AddField("bse_fld_id",_int,_kIntLen,0);
		lnk_close();
		return(0);
	}


wtable	tbl;
char	ext[FILENAME_MAX];
char	name[FILENAME_MAX+4];
char	path[PATH_MAX+4];
char	fname[_FIELDNAME_MAX_+4];
int		tid,fbidx,ftidx,k,sz,fbref;
char*	p;
int		sgn;

bv310FieldAccessor*	facc;

bStdDirectory	bdr(_fd);

	for(int i=1;i<=_lnks->CountRecords();i++){
		if(_lnks->ReadVal(i,kLNK_id_,&tid)){
_te_("_lnks->ReadVal failed for table id");
			continue;
		}
		if(tid==1){
_te_("table id could not be 1");
			continue;
		}
		if(_lid<tid){
_te_("database corrupted : last lid ("+_lid+"), will be set to "+tid);
			if((status=_hdr->WriteVal(1,kHDR_lid_,&tid))){
_te_("database corrupted : _hdr->WriteVal failed with "+status);
				break;
			}
			_lid=tid;
		}
		
		if(_lnks->RecordKilled(i)){
			continue;
		}
		if(_lnks->ReadVal(i,kLNK_name_,name)){
_te_("_lnks->ReadVal failed for name");
			continue;
		}

		if(_lnks->ReadVal(i,kLNK_path_,path)){
_te_("_lnks->ReadVal failed for path");
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		if(_lnks->ReadVal(i,kLNK_tfield_,fname)){
_te_("_lnks->ReadVal failed for field name");
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		if(_lnks->ReadVal(i,kLNK_bfield_,&fbidx)){
_te_("_lnks->ReadVal failed for field idx");
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}

// retrouver l'index table du champ
// a cause des enregistrements effacès
		fbref=0;
		for(int j=1;j<=_fld->CountRecords();j++){
			if(_fld->RecordKilled(j)){
				continue;
			}
			fbref++;
			if(fbref==fbidx){
				fbref=j;
				break;
			}
		}
_tm_("table ref field index is "+fbref+" (from "+fbidx+")");

		p=strrchr(name,'.');
		if(!p){
#pragma mark IL FAUDRAIT TRAITER LE CAS DES TABLES POSTGRESS
_te_("no extension for table "+name);
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		else{
			strcpy(ext,&p[1]);
			sgn=ext2sign(ext);
		}
		tbl=wtbl_alloc(	sgn,
						path,
						name,
						false, 
						_reso,
						_srid,
						&status);
//_tm_("wtbl_alloc passed");
		if(status<0){
_te_("wtbl_alloc failed for "+path+" "+name);
			status=0;
			if(tbl){
				wtbl_free(tbl);
			}
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		
		for(ftidx=1;ftidx<=tbl->CountFields();ftidx++){
			tbl->FieldName(ftidx,path);
			if(!strcmp(path,fname)){
				break;
			}
		}
		
		if(ftidx>tbl->CountFields()){
_te_("field "+fname+" not found");
			status=0;
			wtbl_free(tbl);
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		if((status=_fld->ReadVal(fbref,kFLD_kind_,&k))){
_te_("_fld->ReadVal(fbidx,kFLD_kind_,&k) failed with "+status);
			status=0;
			wtbl_free(tbl);
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}
		if((status=_fld->ReadVal(fbref,kFLD_size_,&sz))){
_te_("_fld->ReadVal(fbidx,kFLD_size_,&d) failed with "+status);
			status=0;
			wtbl_free(tbl);
			facc=new bv310FieldAccessor(NULL,name,tid,0,i);
			_tables.add(&facc);
			continue;
		}

//_tm_("%s/%d->%d (%.4s)",name,tid,fbidx,&k);
		facc=new bv310DynFieldAccessor(tbl,name,tid,fbidx,i,ftidx,k,sz);
		_tables.add(&facc);
	}
	
	lnk_close();

	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::check_dyn(){
_bTrace_("bv310BaseAccessor::check_dyn",false);
	
field	f;
int		k;

// Verification : tables liées et champs liés
bv310FieldAccessor*	facc;
	lnk_open();
	for(int i=_tables.count();i>0;i--){
//_tm_("%d",i);
		k=0;
		_tables.get(i,&facc);
		if(facc->get_id()==1){
			continue;
		}
		for(int j=1;j<=_elts.count();j++){
			_elts.get(j,&f);
			if(facc->get_id()==f.tbl_id){
				k++;
			}
		}
		if(k==0){
_tm_("removing table "+(char*)facc->get_name()+" (no fields)");
			_lnks->KillRecord(facc->get_offset());
			_tables.rmv(i);
			delete facc;
		}
	}
	lnk_close();
	
// Verification : champ référence et tables liées
	for(int i=1;i<=_elts.count();i++){
		k=0;
		if(!is_dyn_ref(i)){
			continue;
		}
		_elts.get(i,&f);
		for(int j=1;j<=_tables.count();j++){
			_tables.get(j,&facc);
			if(facc->get_ref()==f.index){
				k++;
			}
		}
		if(k==0){
_tm_("setting no dynamic reference to field "+f.name+" (no more reference)");
			set_dyn_ref(i,false);
		}
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::fld_write(	int k, 
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
									int index,
									int tbl_id){
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
	(void)_fld->WriteVal(k,kFLD_tbl_id_,&tbl_id);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::h_new(const char* name){
_bTrace_("bv310BaseAccessor::h_new",false);
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
	k--;
	if(_hdr->AddField("mlid",_int,_kIntLen,0)==-1){
_te_("hdr->AddField failed for LID");
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
	if(_hdr->WriteVal(1,kHDR_shortLib_,buff)){
_te_("_hdr->WriteVal(1,kHDR_shortLib_,buff) failed");
		return(k);
	}
	k--;
	if(_hdr->WriteVal(1,kHDR_longLib_,buff)){
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
	if(_hdr->WriteVal(1,kHDR_lid_,&_lid)){
_te_("_hdr->WriteVal(1,kHDR_lid_,&_lid) failed");
		return(k);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::h_init(){
_bTrace_("bv310BaseAccessor::h_init",false);
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
_tm_("_u2m="+_u2m);
	if(_hdr->ReadVal(1,kHDR_srid_,&_srid)){
_te_("_hdr->ReadVal(1,kHDR_srid_,&_srid) failed");
		return(-4);
	}
_tm_("_srid="+_srid);
	
	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::read_name(const char* path, char* name){
_bTrace_("bv310BaseAccessor::read_name",false);
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
void bv310BaseAccessor::read_data(	char* dbname,
									char* host,
									int* port,
									char* user,
									char* password){
_bTrace_("bv310BaseAccessor::read_data()",false);
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
void bv310BaseAccessor::read_data(	char* dbname,
									char* host,
									int* port,
									char* user,
									char* password,
									const char* path){
_bTrace_("bv310BaseAccessor::read_data(path)",false);
bStdDirectory	d(path);
bStdDirectory	d2("Contents/Datas");
	return(read_data(dbname,host,port,user,password));
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::append(){
_bTrace_("bv310BaseAccessor::append",false);
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
				if(_objs->WriteVal(o,fp.index/*i*/,buff)){
_te_("_objs->WriteVal (default value) error for field "+i);					
					return(-3);
				}
			}
			else if(fp.cnstrkind==_bit){
// Bit is not a real constraint : values could be outside of the
// constraints range : write and pass
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
				if(_objs->WriteVal(o,fp.index/*i*/,&idx)){
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
bool bv310BaseAccessor::set_constraint(int f, int n, void* val, bool check){
_bTrace_("bv310BaseAccessor::set_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
		return(false);
	}
	if(check){
		if((fp.kind==_bool)&&check){
_te_("could not set constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not add constraint on standard fields color or direction");
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
_te_("could not set constraint : table not empty");
				return(false);
			}
		}
		if(!xToChar(fp.cnstrkind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp. cnstrkind);
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
bool bv310BaseAccessor::add_constraint(int f, int kind, void* val, bool check){
_bTrace_("bv310BaseAccessor::add_constraint(int,int,void*,bool)",false);
field	fp;
//_tm_("check="+check);
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
		return(false);
	}
	switch(fp.kind){
		case _binary:
		case _ivx2:
		case _ivx3:
		case _ivxs2:
		case _ivxs3:
		case _ivxrect:
		case _ivxcube:
		case _dvx2:
		case _dvx3:
		case _dvxs2:
		case _dvxs3:
		case _dvxrect:
		case _dvxcube:
_te_("add_constraint not yet supported for complex fields");			
			return(false);
			break;
	}
	if(check){
		if(fp.kind==_bool){
_te_("could not add constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not add constraint on standard fields color or direction");
			return(false);
		}
	}

cnst	c;
	c.fid=fp.fid;
	c.idx=count_constraints(f)+1;
	c.offset=_cnst->CountRecords()+1;
	c.val[0]=0;
	
	if(c.idx==1){// first time
//_tm_("c.idx="+c.idx);
		if(kind!=0){
			if(kind==_bit){
				if((fp.kind!=_char)&&(fp.kind!=_int)){
_te_("bad field kind for bit constraint"+(UInt32*)&fp.cnstrkind);
					return(false);					
				}
			}
			fp.cnstrkind=kind;
		}
		else{
			fp.cnstrkind=fp.kind;
		}
_tm_("nb_live="+nb_live());
		if((nb_live()>0)&&(fp.cnstrkind!=_bit)){
			if(fp.cnstrkind!=fp.kind){// Classe
_tm_("classe");
int		ival=1;
double	dval=1;
				switch(fp.kind){
					case _int:
					case _bool:
						if(!check_for_val(f,&ival)){
_te_("could not set constraint : table not empty");
							return(false);
						}	
						break;
					case _double:
					case _date:
					case _time:
						if(!check_for_val(f,&dval)){
_te_("could not set constraint : table not empty");
							return(false);
						}						
						break;
					default:
_te_("could not set constraint for kind "+(UInt32*)&fp.kind);
						return(false);
						break;
				}
			}
			else{// Natural
_tm_("natural");
				if(!check_for_val(f,val)){
_te_("could not set constraint : table not empty");
					return(false);
				}
			}
		}
		if(_fld->WriteVal(fp.offset,kFLD_constKind_,&fp.cnstrkind)){
_te_("_fld->WriteVal(fp.offset,kFLD_constKind_,&fp.kind) failed");
			return(false);
		}
		if(!_elts.put(f,&fp)){
_te_("_elts.put failed");
			return(false);
		}
		if(fp.cnstrkind!=_bit){
			if(!set_default_value(f,val,false)){
_te_("set_default_value failed");
				return(false);
			}
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
_te_("unsupported conversion for "+(UInt32*)&fp. cnstrkind);
			return(false);
		}
	}
	else{
		strcpy(c.val,(char*)val);
	}
_tm_("adding "+c.val+" constraint");

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
bool bv310BaseAccessor::rmv_constraint(int f, int n, bool check){
_bTrace_("bv310BaseAccessor::rmv_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
		return(false);
	}
	if((nb_live()>0)&&(fp.index!=-1)){//= field deleted
_te_("could not remove constraint : table not empty");
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
bool bv310BaseAccessor::check_for_val(int f, void* val){
_bTrace_("bv310BaseAccessor::check_for_val",false);
	if(_objs->CountRecords()==0){
		return(true);
	}
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
void* buff=malloc(fp.size);	
//_tm_("size=%d",fp.size);
	for(int i=1;i<=_objs->CountRecords();i++){
//_tm_("%d",i);
		if(_objs->RecordKilled(i)){
//_tm_("killed");
			continue;
		}
		_objs->ReadVal(i,f,buff);
		switch(fp.kind){
			case _binary:{
void*	bbuff=(void*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					free(bbuff);
					return(false);
				}
//				free(bbuff);
			}	break;
			case _ivxs2:
			case _ivxs3:{
ivertices*	bbuff=(ivertices*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					ivs_free(bbuff);
//					free(buff);
					return(false);
				}
//				ivs_free(bbuff);
			}	break;
			case _dvxs2:
			case _dvxs3:{
dvertices*	bbuff=(dvertices*)(*(int*)buff);
				if(memcmp(val,bbuff,fp.size)){
					dvs_free(bbuff);
//					free(buff);
					return(false);
				}
//				dvs_free(bbuff);
			}	break;
			case _char:{
char*		abuf=(char*)val;
char*		bbuf=(char*)buff;
				if(strcmp(abuf,bbuf)){
					free(buff);
					return(false);
				}
			}	break;
			default:
//_tm_("default");
				if(fp.kind==_int){
//					int a,b;
//					a=(*(int*)val);
//					b=(*(int*)buff);
//_tm_("%d->%d",a,b);

				}
				if(memcmp(val,buff,fp.size)){
_tm_("default, memcmp failed");
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
bool bv310BaseAccessor::clear_default_value(int f){
_bTrace_("bv310BaseAccessor::clear_default_value",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
		return(false);
	}
	if((count_constraints(f)>0)&&(fp.cnstrkind!=_bit)){
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
// USE _FLD
// -----------
bool bv310BaseAccessor::set_default_value(int f, void* val, bool check){
_bTrace_("bv310BaseAccessor::set_default_value",false);
	if(is_lock()){
_te_("type is lock");
		return(false);
	}
	if(val==NULL){
		return(clear_default_value(f));
	}
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(is_dyn(f)){
_te_("field "+f+" is dynamic");
		return(false);
	}
	switch(fp.kind){
		case _binary:
		case _ivx2:
		case _ivx3:
		case _ivxs2:
		case _ivxs3:
		case _ivxrect:
		case _ivxcube:
		case _dvx2:
		case _dvx3:
		case _dvxs2:
		case _dvxs3:
		case _dvxrect:
		case _dvxcube:
_te_("set_default_value not yet supported for complex fields");			
			return(false);
			break;
	}
	if(check){
		if((count_constraints(f)>0)&&(fp.cnstrkind!=_bit)){
			if(!get_constraint_index(f,val)){
_te_("default value not found in constraints");
				return(false);
			}
		}		
	}
char	tmp[256];
	if(fp.cnstrkind!=0){
		if(fp.cnstrkind==_bit){ // multiple values, natural field
			if(!xToChar(fp.kind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
				return(false);
			}	
		}
		else if(fp.cnstrkind!=fp.kind){// Class constraint field
			if(!xToChar(fp.cnstrkind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.cnstrkind);
				return(false);
			}				
		}
		else{
			if(!xToChar(fp.kind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
				return(false);
			}	
		}
	}
	else{
		if(!xToChar(fp.kind,fp.decs,val,tmp)){
_te_("unsupported conversion for "+(UInt32*)&fp.kind);
			return(false);
		}	
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
// 
// -----------
bool bv310BaseAccessor::is_lock(){
/*int		x=0;
	if(_hdr->ReadVal(1,kHDR_state_,&x)){
_te_("_hdr->ReadVal(1,kHDR_state_,&x) failed");
		return(true);
	}*/
	return((_state&_kBaseLock_)==_kBaseLock_);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::nb_live(){
_bTrace_("bv310BaseAccessor::nb_live",false);
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
void bv310BaseAccessor::init_user(){
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
bool bv310BaseAccessor::make_package(int sign, const char* path, const char* name, const char* data){
_bTrace_("bv310BaseAccessor::make_package",_VERBOSE_);
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
	_strr_+sign+"\n"+_kVersion310;
	/*store.write("%d\n",sign);
	store.write("%d",_kVersion310);*/
	store.write((char*)__str__.string());
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

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::lnk_open(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::lnk_close(){
}

// ---------------------------------------------------------------------------
// 
// -----------
bv310FieldAccessor*	bv310BaseAccessor::get_facc(int tbl_id){
//_bTrace_("bv310BaseAccessor::get_facc",false);
bv310FieldAccessor* acc;
	for(int i=1;i<=_tables.count();i++){
		_tables.get(i,&acc);
//_tm_("%s",acc->get_name());
		if(tbl_id==acc->get_id()){
			return(acc);
		}
	}
	return(NULL);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bv310BaseAccessor::static_read(void* prm, int o, int f, void* val){
bv310BaseAccessor* bse=(bv310BaseAccessor*)prm;
	return(bse->read(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
void proj_vxs(ivertices* ivs, double reso, bStdProj* frp, bStdProj* top){
dvertices* dvs;
	vxs_i2d(&dvs,ivs,reso);
	top->transform(*frp,dvs);
	for(long i=0;i<dvs->nv;i++){
		vx_d2i(&ivs->vx.vx2[i],&dvs->vx.vx2[i],reso);
	}
	dvs_free(dvs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void proj_vxr(ivx_rect* vr, double reso, bStdProj* frp, bStdProj* top){
ivertices* ivs=ivs_new(_2D_VX,4,0);
	ivs->vx.vx2[0].h=vr->left;
	ivs->vx.vx2[0].v=vr->top;
	ivs->vx.vx2[1].h=vr->left;
	ivs->vx.vx2[1].v=vr->bottom;
	ivs->vx.vx2[2].h=vr->right;
	ivs->vx.vx2[2].v=vr->bottom;
	ivs->vx.vx2[3].h=vr->right;
	ivs->vx.vx2[3].v=vr->top;
dvertices* dvs;
	vxs_i2d(&dvs,ivs,reso);
	top->transform(*frp,dvs);
	for(long i=0;i<dvs->nv;i++){
		vx_d2i(&ivs->vx.vx2[i],&dvs->vx.vx2[i],reso);
	}
	dvs_free(dvs);
	ivs_bounds(ivs,vr);
	ivs_free(ivs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void scale_vxs(ivertices* ivs, double reso){
//_bTrace_("scale_vxs2",true);
//_tm_(reso);
	if(ivs_good(ivs)){
		return;
	}
	for(long i=0;i<ivs->nv;i++){
//_tm_(ivs->vx.vx2[i].h+":"+ivs->vx.vx2[i].v);
		ivs->vx.vx2[i].h=round(ivs->vx.vx2[i].h*reso);
		ivs->vx.vx2[i].v=round(ivs->vx.vx2[i].v*reso);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void scale_vxr(ivx_rect* vr, double reso){
	vr->left=round(vr->left*reso);
	vr->right=round(vr->right*reso);
	vr->top=round(vr->top*reso);
	vr->bottom=round(vr->bottom*reso);
}

/*
// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::add_constraint(int f, void* val, bool check){
_bTrace_("bv310BaseAccessor::add_constraint",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(check){
		if((fp.kind==_bool)&&check){
_te_("could not add constraint on bool field");
			return(false);
		}
		if((f==kOBJ_Color_)||(f==kOBJ_Dir_)){
_te_("could not add constraint on standard fields color or direction");
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
_te_("could not set constraint : table not empty");
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
_te_("unsupported conversion for "+(UInt32*)&fp. cnstrkind);
			return(false);
		}
	}
	else{
		sprintf(c.val,(char*)val);
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
// DYN OK
// -----------
bool bv310BaseAccessor::set_kind(int f, int k){
	if(is_dyn(f)){// ne pas modifier les tables liées
		return(false);
	}
	return(false);
}

// ---------------------------------------------------------------------------
// DYN OK
// -----------
bool bv310BaseAccessor::set_len(int	f,	int	l){
	if(is_dyn(f)){// ne pas modifier les tables liées
		return(false);
	}
	return(false);
}

// ---------------------------------------------------------------------------
// DYN OK
// -----------
bool bv310BaseAccessor::set_decs(int f,	int	d){
	if(is_dyn(f)){// ne pas modifier les tables liées
		return(false);
	}
field	fp;
	
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	fp.decs=d;
	_elts.put(f,&fp);
	// Ecrire
	return(true);
}

// ---------------------------------------------------------------------------
// USE _FLD
// DYN OK
// -----------
bool bv310BaseAccessor::set_name(int f,	char* name){
	if(is_dyn(f)){// ne pas modifier les tables liées
		return(false);
	}
field	fp;
	
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);
	if(_objs->ChgField(fp.index,name,fp.kind,fp.len,fp.decs)){
		return(false);
	}	
	if(_fld->WriteVal(fp.offset,kFLD_name_,name)){
		return(false);
	}
	strncpy(fp.name,name,256);
	_elts.put(f,&fp);
	f_put_date(f);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_constraints_kind(int f, int kind){
_bTrace_("bv310BaseAccessor::set_constraints_kind",false);
field	fp;
	if(!_elts.get(f,&fp)){
_te_("field "+f+" out of range [1.."+(int)_elts.count()+"]");
		return(false);
	}
	if(fp.cnstrkind!=0){
_te_("constraint kind already set to %.4s",&fp.cnstrkind);
		return(false);		
	}
	if(kind==_bit){
		if((fp.kind!=_char)&&(fp.kind!=_int)){
_te_("bad field kind for bit constraint",&fp.cnstrkind);
			return(false);					
		}
	}
	if(_fld->WriteVal(fp.offset,kFLD_constKind_,&fp.kind)){
_te_("_fld->WriteVal failed");
		return(false);
	}
	return(true);
}


// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::rmv_link(char* name){
bTrace					trc("bv310BaseAccessor::rmv_link",true);
bv310DynFieldAccessor*	acc;
bv310DynFieldAccessor*	acc2;
int						x;

	if(strlen(name)==0){
_te_("bad table name [%s]",name);
		return(-1);
	}
	for(int i=1;i<=_tables.count();i++){
		_tables.get(i,&acc);
		if(strcmp(acc->get_name(),name)==0){
			x=i;
			break;
		}
		acc=NULL;
	}
	if(acc==NULL){
_te_("table [%s] not found",name);
		return(-1);
	}
	if(acc->get_id()==1){
_te_("could not remove static table (table id==1)");
		return(-1);
	}

field	fp;

// 1 on retire les champs de la table
	for(int i=_elts.count();i>0;i--){
		_elts.get(i,&fp);
		if(fp.tbl==acc){
			if(remove(i)){
_te_("remove failed for field [%s]",fp.name);
				return(-1);
			}
		}
	}
	
// 2 on replace le dyn ref du champ cible à 0 si nécessaire
int	k=0;
	for(int i=1;i<=_tables.count();i++){
		_elts.get(i,&acc2);
		if(acc2->get_ref()==acc->get_ref()){
			k++;
		}
	}
	if(k<2){
		set_dyn_ref(acc->get_ref(),false);
	}
	
	_tables.rmv(x);
	delete acc;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::get_format(int f, char* fmt){
#if _HAS_FORMAT_
field	fp;
	strcpy(fmt,"");
	if((f<1)||(f>count())){
		return(false);
	}
	_elts.get(f,&fp);	
	strcpy(fmt,fp.format);
	return(true);
#else
	return(false);
#endif
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bv310BaseAccessor::set_format(int f, char* fmt){
#if _HAS_FORMAT_
field	fp;
	if((f<1)||(f>count())){
		return(false);
	}
	strncpy(fp.format,fmt,256);
	_elts.put(f,&fp);
	return(true);
#else
	return(false);
#endif
}

*/

/*// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::fld_open(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bv310BaseAccessor::cnt_open(){
	return(0);
}*/

/*// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::fld_close(){
}

// ---------------------------------------------------------------------------
// 
// -----------
void bv310BaseAccessor::cnt_close(){
}*/
