//----------------------------------------------------------------------------
// File : wbase.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Database C wrappers
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
// 06/02/2006 creation.
//----------------------------------------------------------------------------

#include "wbase.h"
#include "mms_config.h"
#include "strget.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bStdDirectory.h"
#include "bStdFile.h"

#include "bv310LDBBaseAccessor.h"
#include "bv310PGisBaseAccessor.h"
#include "bv310GridAIASCIIBaseAccessor.h"
#include "bLDBBaseAccessor.h"
#include "bPGisBaseAccessor.h"
#include "bv310CBCProbedataBaseAccessor.h"


#include "bTrace.h"

// ---------------------------------------------------------------------------
// 
// ------------
static int upgrade(const char* hpath){
int				sgn;
bStdDirectory	root(hpath);
	if(root.status()){
		return(0);
	}

char			buff[256];
_PUSH_
bStdFile	store("Contents/Datas/storage.txt","r");
	if(store.status()){
		return(0);
	}
	store.next_line(buff,256);
	sgn=atoi(buff);
	buff[0]=0;
	store.next_line(buff,256);
_POP_
bStdFile	store("Contents/Datas/storage.txt","w");
	
	sprintf(buff,"%d\n%d",sgn,_kVersion310);
	store.write(buff);
	return(1);
}

// ---------------------------------------------------------------------------
// 
// ------------
wbase wbse_create(	int signature,
					const char* hpath,
					const char* name,
					const char* data,
					int kind,
					double reso,
					double u2m,
					int srid,
					int* status){
bStdBaseAccessor*	bse=NULL;

	switch(signature){
		case kBaseLocalDB:
		case kBaseLocalDBRaw:
		case kBaseLocalDBID:
			bse=new bv310LDBBaseAccessor(hpath,status,kind,srid,reso,u2m,name,data);
			break;
		case kBasePostGIS:
		case kBasePostGISRaw:
		case kBasePostGISID:
			bse=new bv310PGisBaseAccessor(hpath,status,kind,srid,reso,u2m,name,data);
			break;
		case kBaseArcInfoASCII:
		case kBaseArcInfoASCIIRaw:
		case kBaseArcInfoASCIIID:
			bse=new bv310GridAIASCIIBaseAccessor(hpath,status,kind,srid,reso,u2m,name,data);
			break;
        case kBaseCBCProbedata:
        case kBaseCBCProbedataRaw:
        case kBaseCBCProbedataID:
            bse=new bv310CBCProbedataBaseAccessor(hpath,status,kind,srid,reso,u2m,name,data);
            break;
		default:
			break;
	}
	return(bse);
}

// ---------------------------------------------------------------------------
// 
// ------------
wbase wbse_open(	const char* hpath,
					int* status){
bStdBaseAccessor*	bse=NULL;

int				sgn,vers;
_PUSH_
bStdDirectory	root(hpath);
	if(root.status()){
		*status=-1;
		return(NULL);
	}
char		buff[256];
bStdFile	store("Contents/Datas/storage.txt","r");
	if(store.status()){
		*status=-1;
		return(NULL);
	}
	store.next_line(buff,256);
	sgn=atoi(buff);
	buff[0]=0;
	store.next_line(buff,256);
	vers=atoi(buff);
	if(vers==0){
		vers=_kVersion300;
	}
_POP_
	if(vers<_kVersion310){
char	msg[256];
char	exp[256];
		strget(kMsgUpgradeMsg,msg);
		strget(kMsgUpgradeExp,exp);
		if(upgrade(hpath)){
			vers=_kVersion310;
		}
	}
	else if(vers>_kVersion310){
		*status=-2;
		return(NULL);
	}
	
	switch(sgn){
		case kBaseLocalDBID:
			if(vers==_kVersion310){
				bse=new bv310LDBBaseAccessor(hpath,-1,0,status);
			}
			else if(vers==_kVersion300){
				bse=new bLDBBaseAccessor(hpath,status);
			}
			break;
		case kBasePostGISID:
		case kBasePostGISRaw:
			if(vers==_kVersion310){
				bse=new bv310PGisBaseAccessor(hpath,-1,0,status);
			}
			else if(vers==_kVersion300){
				bse=new bPGisBaseAccessor(hpath,status);
			}
			break;

		case kBaseArcInfoASCII:
		case kBaseArcInfoASCIIRaw:
		case kBaseArcInfoASCIIID:
			bse=new bv310GridAIASCIIBaseAccessor(hpath,-1,0,status);
			break;

        case kBaseCBCProbedata:
        case kBaseCBCProbedataRaw:
        case kBaseCBCProbedataID:
            bse=new bv310CBCProbedataBaseAccessor(hpath,-1,0,status);
            break;

		default:
			break;
	}
	return(bse);
}

// ---------------------------------------------------------------------------
// 
// ------------
wbase wbse_openws(	const char* hpath,
					int	wanted_srid,
					double wanted_reso,
					int* status){
bStdBaseAccessor*	bse=NULL;
	
int				sgn,vers;
_PUSH_
bStdDirectory	root(hpath);
	if(root.status()){
		*status=-1;
		return(NULL);
	}
char		buff[256];
bStdFile	store("Contents/Datas/storage.txt","r");
	if(store.status()){
		*status=-1;
		return(NULL);
	}
	store.next_line(buff,256);
	sgn=atoi(buff);
	buff[0]=0;
	store.next_line(buff,256);
	vers=atoi(buff);
	if(vers==0){
		vers=_kVersion300;
	}
_POP_
	if(vers<_kVersion310){
char	msg[256];
char	exp[256];
		strget(kMsgUpgradeMsg,msg);
		strget(kMsgUpgradeExp,exp);
		if(upgrade(hpath)){
			vers=_kVersion310;
		}
	}
	else if(vers>_kVersion310){
		*status=-2;
		return(NULL);
	}
	
	switch(sgn){
		case kBaseLocalDBID:
			if(vers==_kVersion310){
				bse=new bv310LDBBaseAccessor(hpath,wanted_srid,wanted_reso,status);
			}
			else if(vers==_kVersion300){
				bse=new bLDBBaseAccessor(hpath,status);
			}
			break;
		case kBasePostGISID:
		case kBasePostGISRaw:
			if(vers==_kVersion310){
				bse=new bv310PGisBaseAccessor(hpath,wanted_srid,wanted_reso,status);
			}
			else if(vers==_kVersion300){
				bse=new bPGisBaseAccessor(hpath,status);
			}
			break;
			
		case kBaseArcInfoASCII:
		case kBaseArcInfoASCIIRaw:
		case kBaseArcInfoASCIIID:
			bse=new bv310GridAIASCIIBaseAccessor(hpath,wanted_srid,wanted_reso,status);
			break;

        case kBaseCBCProbedata:
        case kBaseCBCProbedataRaw:
        case kBaseCBCProbedataID:
            bse=new bv310CBCProbedataBaseAccessor(hpath,wanted_srid,wanted_reso,status);

		default:
			break;
	}
	return(bse);
}

// ---------------------------------------------------------------------------
// 
// ------------
void wbse_free(wbase bse){
	delete (bStdBaseAccessor*)bse;
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_read(	wbase bse, 
				int o,  
				int f,  
				void *val){
	return(((bStdBaseAccessor*)bse)->read(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_write(	wbase bse, 
				int o,  
				int f,  
				void *val){
	return(((bStdBaseAccessor*)bse)->write(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_count(	wbase bse){
	return(((bStdBaseAccessor*)bse)->count());
}
		
// ---------------------------------------------------------------------------
// 
// ------------
int wbse_add(	wbase bse, 
				int kind,  
				int len,  
				int decs,  
				const char* name){
	return(((bStdBaseAccessor*)bse)->add(kind,len,decs,name));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_modify(	wbase bse, 
					int f,  
					int kind,  
					int len,  
					int decs,  
					const char* name){
	return(((bStdBaseAccessor*)bse)->modify(f,kind,len,decs,name));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_remove(	wbase bse, 
					int f){
	return(((bStdBaseAccessor*)bse)->remove(f));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_kind(	wbase bse, 
						int f,  
						int* k){
	return(((bStdBaseAccessor*)bse)->get_kind(f,k));
}
		
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_len(	wbase bse, 
					int f,  
					int* l){
	return(((bStdBaseAccessor*)bse)->get_len(f,l));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_decs(	wbase bse, 
						int f,  
						int* d){
	return(((bStdBaseAccessor*)bse)->get_decs(f,d));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_name(	wbase bse, 
						int f,  
						char* name){
	return(((bStdBaseAccessor*)bse)->get_name(f,name));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_size(	wbase bse, 
						int f,  
						int* sz){
	return(((bStdBaseAccessor*)bse)->get_size(f,sz));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_get_id(	wbase bse, 
					int f){
	return(((bStdBaseAccessor*)bse)->get_id(f));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_get_id_with_name(	wbase bse, 
							const char* name){
	return(((bStdBaseAccessor*)bse)->get_id(name));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_get_index(	wbase bse, 
					int fid){
	return(((bStdBaseAccessor*)bse)->get_index(fid));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_get_index_with_name(	wbase bse, 
								const char* name){
	return(((bStdBaseAccessor*)bse)->get_index(name));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_is_delprotected(	wbase bse, 
							int f){
	return(((bStdBaseAccessor*)bse)->is_delprotected(f));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_delprotected(	wbase bse, 
								int f,  
								wbool b){
	return(((bStdBaseAccessor*)bse)->set_delprotected(f,b));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_is_writeprotected(	wbase bse, 
								int f){
	return(((bStdBaseAccessor*)bse)->is_writeprotected(f));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_writeprotected(	wbase bse, 
								int f,  
								wbool b){
	return(((bStdBaseAccessor*)bse)->set_writeprotected(f,b));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_is_hidden(	wbase bse, 
						int f){
	return(((bStdBaseAccessor*)bse)->is_hidden(f));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_hidden(	wbase bse, 
						int f,  
						wbool b){
	return(((bStdBaseAccessor*)bse)->set_hidden(f,b));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_is_dyn(	wbase bse, 
					int f){
	return(((bStdBaseAccessor*)bse)->is_dyn(f));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_is_dyn_ref(	wbase bse, 
						int f){
	return(((bStdBaseAccessor*)bse)->is_dyn_ref(f));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_constraint(	wbase bse, 
							int f,
							int n, 
							void *val){
	return(((bStdBaseAccessor*)bse)->get_constraint(f,n,val));
}
			
// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_constraint(	wbase bse, 
							int f,
							int n, 
							void *val){
	return(((bStdBaseAccessor*)bse)->set_constraint(f,n,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_add_constraint(	wbase bse, 
							int kind,
							int f,
							void *val){
	return(((bStdBaseAccessor*)bse)->add_constraint(f,kind,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_rmv_constraint(	wbase bse, 
							int f,
							int n){
	return(((bStdBaseAccessor*)bse)->rmv_constraint(f,n));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_get_constraint_index(	wbase bse, 
								int f,
								void *val){
	return(((bStdBaseAccessor*)bse)->get_constraint_index(f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_constraint_index(	wbase bse, 
									int f,
									int from, 
									int to){
	return(((bStdBaseAccessor*)bse)->set_constraint_index(f,from,to));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_has_default_value(	wbase bse, 
								int f){
	return(((bStdBaseAccessor*)bse)->has_default_value(f));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_default_value(	wbase bse, 
								int f,
								void* val){
	return(((bStdBaseAccessor*)bse)->get_default_value(f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_default_value(	wbase bse, 
								int f,
								void* val){
	return(((bStdBaseAccessor*)bse)->set_default_value(f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_count_records(	wbase bse){
	return(((bStdBaseAccessor*)bse)->count_records());
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_kill_record(	wbase bse,
						int r){
	return(((bStdBaseAccessor*)bse)->kill_record(r));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_unkill_record(	wbase bse,
							int r){
	return(((bStdBaseAccessor*)bse)->unkill_record(r));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_killed_record(	wbase bse,
							int r){
	return(((bStdBaseAccessor*)bse)->killed_record(r));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_h_read(	wbase bse, 
					int o,  
					int f,  
					void *val){
	return(((bStdBaseAccessor*)bse)->h_read(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_h_write(	wbase bse, 
					int o,  
					int f,  
					void *val){
	return(((bStdBaseAccessor*)bse)->h_write(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_count_param(	wbase bse, 
						const char* sub){
	return(((bStdBaseAccessor*)bse)->count_param(sub));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_param_name(	wbase bse, 
							const char* sub, 
							int idx,
							char* name){
	return(((bStdBaseAccessor*)bse)->get_param_name(sub,idx,name));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_get_param(	wbase bse, 
						const char* sub, 
						const char* name, 
						void** p,
						int* sz){
	return(((bStdBaseAccessor*)bse)->get_param(sub,name,p,sz));
}

// ---------------------------------------------------------------------------
// 
// ------------
wbool wbse_set_param(	wbase bse, 
						const char* sub, 
						const char* name, 
						void* p,
						int sz){
	return(((bStdBaseAccessor*)bse)->set_param(sub,name,p,sz));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_add_dyn(	wbase bse, 
					const char* path, 
					const char* name, 
					int tbl_f, 
					int bse_f){
	return(((bStdBaseAccessor*)bse)->add_dyn(path,name,tbl_f,bse_f));
}

// ---------------------------------------------------------------------------
// 
// ------------
int wbse_pack(	wbase bse){
	switch(((bStdBaseAccessor*)bse)->signature()){
		case kBaseLocalDB:
		case kBasePostGIS:
			if(((bStdBaseAccessor*)bse)->version()==_kVersion310){
				return(((bv310BaseAccessor*)bse)->pack());
			}
			break;
	}
	return(-100);
}
