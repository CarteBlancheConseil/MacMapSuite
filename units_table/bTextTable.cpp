//----------------------------------------------------------------------------
// File : bTextTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Text file table class
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

#include "bTextTable.h"
#include "bTrace.h"
#include "bStdDirectory.h"
#include "valconv.h"
#include "db.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextTable	::bTextTable(	const char* path,
							const char* name,
							bool create,
							double reso,
							int srid,
							int* status)
			:bMemTable(	path,
						name,
						create,
						reso,
						srid,
						status){
int	bck_stat=*status;
	_update=false;
_bTrace_("bTextTable::bTextTable",false);
	if(*status<0){
		return;
	}
	
	_fp=NULL;

bStdUTF8Directory	dr(path);
	if(dr.status()){
//_te_("directory error "+dr.status()+" for "+path);
		*status=-1;
		return;
	}
	
	if(create){
		_fp=fopen(name,"w");
		if(!_fp){
//_te_("fopen (w) failed for "+name);
			*status=-1;
			return;
		}
		*status=bck_stat;
	}
	else{
		_fp=fopen(name,"r+");
		if(!_fp){
_tw_("fopen (r+) failed for "+name);
			_fp=fopen(name,"r");
			if(!_fp){
_te_("fopen (r+) failed for "+name);
				*status=-1;
				return;
			}
		}	
		*status=bck_stat;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextTable	::bTextTable(	const char* path,
							const char* name,
							bool create,
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
			:bMemTable(	path,
						name,
						create,
						reso,
						ox,
						oy,
						tsrid,
						asrid,
						status){
int	bck_stat=*status;
	_update=false;
_bTrace_("bTextTable::bTextTable",false);
	if(*status<0){
		return;
	}
	
	_fp=NULL;

bStdUTF8Directory	dr(path);
	if(dr.status()){
_te_("directory error "+dr.status()+" for "+path);
		*status=-1;
		return;
	}
	
	if(create){
		_fp=fopen(name,"w");
		if(!_fp){
_te_("fopen (w) failed for "+name);
			*status=-1;
			return;
		}
		*status=bck_stat;
	}
	else{
		_fp=fopen(name,"r+");
		if(!_fp){
_tw_("fopen (r+) failed for "+name);
			_fp=fopen(name,"r");
			if(!_fp){
_te_("fopen (r) failed for "+name);
				*status=-1;
				return;
			}
		}		
		*status=bck_stat;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextTable::~bTextTable(){
	if(_fp){
		fclose(_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::AddField(	const char *nam, 
							int sign, 
							int length, 
							int decs){
//_bTrace_("bTextTable::AddField",false);
//_tm_(nam+";"+(UInt32*)&sign+";"+length+";"+decs);
	switch(sign){
		case _int:
		case _double:
		case _char:
		
		case _bool:
		case _date:
		case _time:

			return(bMemTable::AddField(nam,sign,length,decs));
			break;
	}
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::ChgField(	int f, 
							const char* nam, 
							int sign, 
							int length, 
							int decs){
	switch(sign){
		case _int:
		case _double:
		case _char:
			return(bMemTable::ChgField(f,nam,sign,length,decs));
			break;
	}
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::WriteVal(	int o, 
							int f, 
							void* val){
//_bTrace_("bTextTable::WriteVal",false);
//_tm_(o+";"+f);
	_update=true;
	return(bMemTable::WriteVal(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::WriteVal(	int o, 
							int f, 
							void* val, 
							bool dummy){
	return(bMemTable::WriteVal(o,f,val));
}

// ---------------------------------------------------------------------------
// Trop dangereux de ne pas typer les champs !!!
// -----------
int	bTextTable::load(){
_bTrace_("bTextTable::load",false);
int		l,k,d;
char	nm[256];

	for(int	i=0;i<_loader->nb_columns();i++){
		k=_loader->column_kind(i);
		switch(k){
			case _char:
				l=_loader->column_len(i);
				if(l<=0){
					l=1;
				}
				d=0;
				break;
			case _int:
				l=_kIntLen;
				d=0;
				break;
			case _double:
				l=_kDoubleLen;
				d=6;
				break;
		}
		
		sprintf(nm,"col%d",i+1);
		if(AddField(nm,k,l,d)){
_te_("AddField "+nm+" failed");
			return(-1);
		}
	}
	_loader->iterate(this,dump_to_mem);	
	delete _loader;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::dump_to_mem(int o, int f, char* val, void* up){
//_bTrace_("bTextTable::dump_to_mem",false);
bTextTable*		t=(bTextTable*)up;
int				k,l,d,s;

	if(	(val[strlen(val)-1]==10)	||
		(val[strlen(val)-1]==13)	){
		val[strlen(val)-1]=0;
	}

	t->FieldSign(f+1,&k);
	t->FieldLength(f+1,&l);
	t->FieldDecimals(f+1,&d);
	t->FieldSize(f+1,&s);
	if((o==0)&&(t->_loader->has_header())){
		t->ChgField(f+1,val,k,l,d);
		return(0);
	}
	if(!t->_loader->has_header()){
		o++;
	}
//_tm_("malloc("+s+")");
void*	r=malloc(s);
//_tm_("charToX");
	charToX(k,d,val,r);
//_tm_("WriteVal");
	if(t->WriteVal(o,f+1,r,true)){
	}
//_tm_("free");
	free(r);
//_tm_("ok");
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextTable::dump_to_file(){
	return(0);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextDelimitedTable	::bTextDelimitedTable(	const char* path,
											const char* name,
											bool create,
											double reso,
											int srid,
											int* status,
											char del,
											bool encap)
					:bTextTable(path,
								name,
								create,
								reso,
								srid,
								status){
//_bTrace_("bTextDelimitedTable::bTextDelimitedTable",true);
	_del=del;
	_encap=encap;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextDelimitedTable	::bTextDelimitedTable(	const char* path,
											const char* name,
											bool create,
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status,
											char del,
											bool encap)
					:bTextTable(path,
								name,
								create,
								reso,
								ox,
								oy,
								tsrid,
								asrid,
								status){
//_bTrace_("bTextDelimitedTable::bTextDelimitedTable",true);
	_del=del;
	_encap=encap;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextDelimitedTable::~bTextDelimitedTable(){
	if(_update){
		dump_to_file();
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextDelimitedTable::dump_to_file(){
int		i,j,sz,k,l,d,msz=0,ml=0;
char	nm[256];

	fseek(_fp,0,SEEK_SET);

	for(i=1;i<=CountFields();i++){
		FieldSize(i,&sz);
		if(sz>msz){
			msz=sz;
		}
		FieldLength(i,&l);
		if(l>ml){
			ml=l;
		}
		FieldName(i,nm);
		if(_encap){
			if(i<CountFields()){
				fprintf(_fp,"\"%s\"%c",nm,_del);
			}
			else{
				fprintf(_fp,"\"%s\"\n",nm);
			}
		}
		else{
			if(i<CountFields()){
				fprintf(_fp,"%s%c",nm,_del);
			}
			else{
				fprintf(_fp,"%s\n",nm);
			}
		}
	}
void* buff=malloc(msz);
	if(msz>ml){
		ml=msz;
	}
char*	str=(char*)malloc(ml+2);
char	fmt[10];

	for(i=1;i<=CountRecords();i++){
		for(j=1;j<=CountFields();j++){
			FieldSign(j,&k);
			FieldDecimals(j,&d);
			ReadVal(i,j,buff);
			xToChar(k,d,buff,str);
			if(_encap){
				switch(k){
					case _char:
					case _date:
					case _time:
						if(j<CountFields()){
							strcpy(fmt,"\"%s\"%c");
						}
						else{
							strcpy(fmt,"\"%s\"\n");
						}
						break;
					default:
						if(j<CountFields()){
							strcpy(fmt,"%s%c");
						}
						else{
							strcpy(fmt,"%s\n");
						}
						break;
				}
			}
			else{
				if(j<CountFields()){
					strcpy(fmt,"%s%c");
				}
				else{
					strcpy(fmt,"%s\n");
				}
			}
			
			if(j<CountFields()){
				fprintf(_fp,fmt,str,_del);
			}
			else{
				fprintf(_fp,fmt,str);
			}
		}
	}
	
	free(str);
	free(buff);

	return(0);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextTabTable	::bTextTabTable(const char* path,
								const char* name,
								bool create,
								double reso,
								int srid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										srid,
										status,
										9,
										false){
//_bTrace_("bTextTabTable::bTextTabTable",true);
	if(create){
	}
	else if(*status==0){
        *status=load();
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextTabTable	::bTextTabTable(const char* path,
								const char* name,
								bool create,
								double* reso,
								double* ox,
								double* oy,
								int* tsrid,
								int* asrid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										ox,
										oy,
										tsrid,
										asrid,
										status,
										9,
										false){
//_bTrace_("bTextTabTable::bTextTabTable",true);
	if(create){
	}
	else if(*status==0){
		*status=load();
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextTabTable::~bTextTabTable(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextTabTable::signature(){
	return(kTableTabText);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextTabTable::load(){
//_bTrace_("bTextTabTable::load",true);
	_loader=new bTabTextLoader(_fp,true);
	return(bTextTable::load());
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextCSVTable	::bTextCSVTable(const char* path,
								const char* name,
								bool create,
								double reso,
								int srid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										srid,
										status,
										';',
										true){
//_bTrace_("bTextCSVTable::bTextCSVTable",true);
	if(create){
	}
	else if(*status==0){
		*status=load();
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextCSVTable	::bTextCSVTable(const char* path,
								const char* name,
								bool create,
								double* reso,
								double* ox,
								double* oy,
								int* tsrid,
								int* asrid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										ox,
										oy,
										tsrid,
										asrid,
										status,
										';',
										true){
//_bTrace_("bTextCSVTable::bTextCSVTable",true);
	if(create){
	}
	else if(*status==0){
		*status=load();
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextCSVTable::~bTextCSVTable(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextCSVTable::signature(){
	return(kTableCSVText);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextCSVTable::load(){
	_loader=new bCSVTextLoader(_fp,true);
	return(bTextTable::load());
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextDXFTable	::bTextDXFTable(	const char* path,
									const char* name,
									bool create,
									double reso,
									int srid,
									int* status)
					:bTextTable(path,
								name,
								create,
								reso,
								srid,
								status){
_bTrace_("bTextDXFTable::bTextDXFTable",false);
	if(create){
		if(CountFields()==0){
			AddField("tag",_char,256,0);
		}
	}
	else if(*status==0){
		*status=load();
_tm_("load return "+(*status));
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextDXFTable	::bTextDXFTable(	const char* path,
									const char* name,
									bool create,
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status)
					:bTextTable(path,
								name,
								create,
								reso,
								ox,
								oy,
								tsrid,
								asrid,
								status){
_bTrace_("bTextDXFTable::bTextDXFTable",false);
	if(create){
		if(CountFields()==0){
			AddField("tag",_char,256,0);
		}
	}
	else if(*status==0){
		*status=load();
_tm_("load return "+(*status));
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextDXFTable::~bTextDXFTable(){
	if(_update){
		dump_to_file();
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextDXFTable::dump_to_file(){
int		i,ml=0;

	fseek(_fp,0,SEEK_SET);
	FieldLength(1,&ml);
char*	str=(char*)malloc(ml+2);

	for(i=1;i<=CountRecords();i++){
		ReadVal(i,1,str);
		fprintf(_fp,"%s\n",str);
	}
	free(str);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextDXFTable::signature(){
	return(kTableDXF);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextDXFTable::load(){
	_loader=new bTabTextLoader(_fp,false);
	return(bTextTable::load());
}

// ---------------------------------------------------------------------------
// 
// -----------
dvertices* bTextDXFTable::get_ageom(ivertices* vxs){
dvertices*	dvxs=NULL;
	vxs_i2d(&dvxs,vxs,_reso);
	if(!dvxs){
		return(NULL);
	}
	dvs_move(dvxs,_ox,_oy);
	transform_a2t(dvxs);
	return(dvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bTextDXFTable::get_tgeom(dvertices* vxs){
ivertices*	ivxs;
dvertices*	dvxs;
	dvs2dvs(vxs,&dvxs);
	if(dvxs==NULL){
		return(false);
	}
	transform_t2a(dvxs);
	dvs_move(dvxs,-_ox,-_oy);
	vxs_d2i(&ivxs,dvxs,_reso);
	dvs_free(dvxs);
	return(ivxs);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextMMTable	::bTextMMTable(	const char* path,
								const char* name,
								bool create,
								double reso,
								int srid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										srid,
										status,
										9,
										false){
//_bTrace_("bTextMMTable::bTextMMTable",true);
	_tmp=NULL;
	_gk=kVXNoKind;
	if(create){
		_gk=(*status);
		if((_gk<kVXPoint)||(_gk>kVXPolygon)){
			_gk=kVXNoKind;
			*status=-1;
		}
	}
	else if(*status>=0){
		*status=load();
		if(*status==0){
			*status=_gk;
		}
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextMMTable	::bTextMMTable(	const char* path,
								const char* name,
								bool create,
								double* reso,
								double* ox,
								double* oy,
								int* tsrid,
								int* asrid,
								int* status)
				:bTextDelimitedTable(	path,
										name,
										create,
										reso,
										ox,
										oy,
										tsrid,
										asrid,
										status,
										9,
										false){
//_bTrace_("bTextMMTable::bTextMMTable",true);
	_tmp=NULL;
	_gk=kVXNoKind;
	if(create){
		_gk=(*status);
		if((_gk<kVXPoint)||(_gk>kVXPolygon)){
			_gk=kVXNoKind;
			*status=-1;
		}
	}
	else if(*status>=0){
		*status=load();
		if(*status==0){
			*status=_gk;
		}
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextMMTable::~bTextMMTable(){
	if(_update){
		dump_to_file();
		_update=false;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextMMTable::AddField(	const char *nam, 
							int sign, 
							int length, 
							int decs){
	return(bMemTable::AddField(nam,sign,length,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextMMTable::ChgField(	int f, 
							const char* nam, 
							int sign, 
							int length, 
							int decs){
	return(bMemTable::ChgField(f,nam,sign,length,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextMMTable::signature(){
	return(kTableMMText);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextMMTable::dump_to_file(){
int		i,j,sz,k,l,d,msz=0,ml=0;
char	nm[256];

	fseek(_fp,0,SEEK_SET);

// Nom
	for(i=1;i<=CountFields();i++){
		FieldSize(i,&sz);
		if(sz>msz){
			msz=sz;
		}
		FieldLength(i,&l);
		if(l>ml){
			ml=l;
		}
		FieldName(i,nm);
		if(i<CountFields()){
			fprintf(_fp,"%s%c",nm,_del);
		}
		else{
			fprintf(_fp,"%s\n",nm);
		}
	}
// Type
	for(i=1;i<=CountFields();i++){
		FieldSign(i,&k);
#ifdef __LITTLE_ENDIAN__
		swapword(sizeof(UInt32),&k);
#endif		
		if(i<CountFields()){
			fprintf(_fp,"%.4s%c",(char*)&k,_del);
		}
		else{
			fprintf(_fp,"%.4s\n",(char*)&k);
		}
	}
// Longueur
	for(i=1;i<=CountFields();i++){
		FieldLength(i,&k);
		if(i<CountFields()){
			fprintf(_fp,"%d%c",k,_del);
		}
		else{
			fprintf(_fp,"%d\n",k);
		}
	}
// Décimales
	for(i=1;i<=CountFields();i++){
		FieldDecimals(i,&k);
		if(i<CountFields()){
			fprintf(_fp,"%d%c",k,_del);
		}
		else{
			fprintf(_fp,"%d\n",k);
		}
	}
	
void* buff=malloc(msz);
	if(msz>ml){
		ml=msz;
	}
char*		str=(char*)malloc(ml+2);
ivertices*	vxs;

	for(i=1;i<=CountRecords();i++){
		for(j=1;j<=CountFields();j++){
			FieldSign(j,&k);
			switch(k){
				case _ivxs2:
					ReadVal(i,j,&vxs);
					dump_vxs(vxs);
					if(j<CountFields()){
						fprintf(_fp,"%c",_del);
					}
					else{
						fprintf(_fp,"\n");
					}
					break;
				
				default:
					FieldDecimals(j,&d);
					ReadVal(i,j,buff);
					xToChar(k,d,buff,str);
					if(j<CountFields()){
						fprintf(_fp,"%s%c",str,_del);
					}
					else{
						fprintf(_fp,"%s\n",str);
					}
					break;
			}
		}
	}
	
	free(str);
	free(buff);
	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bTextMMTable::dump_vxs(ivertices* vxs){
dvertices*	dvxs;
	vxs_i2d(&dvxs,vxs,_reso);
	if(!dvxs){
		return;
	}
	dvs_move(dvxs,_ox,_oy);
	transform_a2t(dvxs);
char*	txt=dvs2text(_gk,dvxs);	
	if(txt){
		fprintf(_fp,"%s",txt);
		free(txt);
	}
	dvs_free(dvxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices*	bTextMMTable::load_vxs(char* str){
//_bTrace_("bTextMMTable::load_vxs",true);
int			kind;
ivertices*	vxs=NULL;
dvertices*	dvxs=text2dvs(&kind,str);
	if(!dvxs){
		return(NULL);
	}
	transform_t2a(dvxs);
	dvs_move(dvxs,-_ox,-_oy);
	vxs_d2i(&vxs,dvxs,_reso);
	dvs_free(dvxs);
//_tm_(vxs->nv+":"+vxs->no);
	if(vxs==NULL){
		return(NULL);
	}
	if(_gk==kVXNoKind){
		_gk=kind;
	}
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextMMTable::load(){
_bTrace_("bTextMMTable::load",true);
int			l;
field_desc	dsc;

	_tmp=new bArray(sizeof(field_desc));
	_loader=new bTabTextLoader(_fp,true);
	for(int	i=0;i<_loader->nb_columns();i++){
		sprintf(dsc.name,"col%d",i+1);
		l=_loader->column_len(i);
		if(l<1){
			l=1;
		}
		_tmp->add(&dsc);
	}
	_loader->iterate(this,dump_to_mem);	
	delete _loader;
	delete _tmp;
	_tmp=NULL;
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bTextMMTable::dump_to_mem(int o, int f, char* val, void* up){
//_bTrace_("bTextMMTable::dump_to_mem",false);
bTextMMTable*	t=(bTextMMTable*)up;
int				k,l,d,s;
field_desc		dsc;

	if(	(val[strlen(val)-1]==10)	||
		(val[strlen(val)-1]==13)	){
		val[strlen(val)-1]=0;
	}

	if(o<4){
		t->_tmp->get(f+1,&dsc);
		if(o==0){
			strcpy(dsc.name,val);
		}
		else if(o==1){
			dsc.kind=*((int*)val);
#ifdef __LITTLE_ENDIAN__
			swapword(sizeof(UInt32),&dsc.kind);
#endif
		}
		else if(o==2){
			dsc.len=atoi(val);
		}
		else if(o==3){
			dsc.decs=atoi(val);
		}
		t->_tmp->put(f+1,&dsc);
		return(0);
	}
	
	if(t->_tmp->count()>0){
		for(int i=1;i<=t->_tmp->count();i++){
			t->_tmp->get(i,&dsc);
			if(t->AddField(dsc.name,dsc.kind,dsc.len,dsc.decs)){
//_te_("AddField "+dsc.name+" failed");
				t->_tmp->reset();
				return(-1);
			}	
		}
		t->_tmp->reset();
	}
	
	t->FieldSign(f+1,&k);
	t->FieldLength(f+1,&l);
	t->FieldDecimals(f+1,&d);
	t->FieldSize(f+1,&s);

	o-=3;	
	if(k==_ivxs2){
ivertices*	vxs=t->load_vxs(val);
		if(vxs==NULL){
//_te_("NULL vxs");
			return(0);
		}
		if(t->WriteVal(o,f+1,&vxs,true)){
//_te_("WriteVal failed");			
		}
	}
	else{
void*	r=malloc(s);
		charToX(k,d,val,r);
		if(t->WriteVal(o,f+1,r,true)){
		}
		free(r);
	}
	return(0);
}

