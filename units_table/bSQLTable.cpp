//----------------------------------------------------------------------------
// File : bSQLTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : SQL file table class
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
// 23/03/2007 creation.
//----------------------------------------------------------------------------

#include "bSQLTable.h"
#include "bTrace.h"
#include "bStdDirectory.h"
#include "valconv.h"
#include "C_Utils.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
static void norm_string(char* str){
	if(strlen(str)==0){
		return;
	}
char	c[2]="'";
int		n=strlen(str);
	for(int i=0;i<n;i++){
		if(str[i]==c[0]){
			strinsert(str,c,i);
			i++;
		}
		n=strlen(str);
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bSQLTable	::bSQLTable(	const char* path,
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
	_update=false;
_bTrace_("bSQLTable::bSQLTable",false);
	if(*status){
		return;
	}
	
	_fp=NULL;
	_name=strdup(name);
char*	c=strchr(_name,'.');
	if(c){
		c[0]=0;
	}
	// Virer ext
	
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
		*status=0;
	}
	else{
// Write Only
		*status=-2;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bSQLTable	::bSQLTable(	const char* path,
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
	_update=false;
_bTrace_("bSQLTable::bSQLTable",false);
	if(*status){
		return;
	}
	
	_fp=NULL;
	_name=strdup(name);
char*	c=strchr(_name,'.');
	if(c){
		c[0]=0;
	}
	// Virer ext
	
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
		*status=0;
	}
	else{
// Write Only
		*status=-2;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bSQLTable::~bSQLTable(){
	if(_fp&&_name){
		if(_update){
			dump_to_file();
		}
		fclose(_fp);
		free(_name);
	}
	else if(_fp){
		fclose(_fp);
	}
	else if(_name){
		free(_name);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSQLTable::AddField(	const char* nam, 
							int sign, 
							int length, 
							int decs){
//bTrace		trc("bSQLTable::AddField",false);
//trc.msg("add %s",nam);
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
int	bSQLTable::ChgField(	int f, 
							const char* nam, 
							int sign, 
							int length, 
							int decs){
	switch(sign){
		case _int:
		case _double:
		case _char:
		
		case _bool:
		case _date:
		case _time:

			return(bMemTable::ChgField(f,nam,sign,length,decs));
			break;
	}
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSQLTable::WriteVal(	int o, 
							int f, 
							void* val){
//bTrace		trc("bSQLTable::WriteVal",false);
//trc.msg("(%d;%d)",o,f);
	_update=true;
	return(bMemTable::WriteVal(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSQLTable::WriteVal(	int o, 
							int f, 
							void* val, 
							bool dummy){
	return(bMemTable::WriteVal(o,f,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bSQLTable::dump_to_file(){
int		i,j,sz,k,l,d,msz=32,ml=32;
char	nm[256],tnm[256];

	fseek(_fp,0,SEEK_SET);

	strcpy(tnm,_name);
	
	fprintf(_fp,"CREATE TABLE \"%s\" (\n",tnm);

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
		FieldSign(i,&k);
		norm_string(nm);
		switch(k){
			case _int:
				fprintf(_fp,"\"%s\" int4",nm);
				break;
			case _double:
				fprintf(_fp,"\"%s\" float8",nm);
				break;
			case _char:
				fprintf(_fp,"\"%s\" varchar(%d)",nm,l);
				break;
			case _bool:
				fprintf(_fp,"\"%s\" bool",nm);
				break;
			case _date:
				fprintf(_fp,"\"%s\" date",nm);
				break;
			case _time:
				fprintf(_fp,"\"%s\" time",nm);
				break;
		}
		if(i<CountFields()){
			fprintf(_fp,", \n");
		}
		else{
			fprintf(_fp,");\n");
		}
	}
	fprintf(_fp,"BEGIN;\n");
	
void* buff=malloc(msz);
	if(msz>ml){
		ml=msz;
	}
char*	str=(char*)malloc(ml+2);

	for(i=1;i<=CountRecords();i++){
		fprintf(_fp,"INSERT INTO \"%s\" (",tnm);
		for(j=1;j<=CountFields();j++){
			FieldName(j,nm);
			if(j<CountFields()){
				fprintf(_fp,"\"%s\", ",nm);
			}
			else{
				fprintf(_fp,"\"%s\") VALUES (",nm);
			}
		}
		for(j=1;j<=CountFields();j++){
			FieldSign(j,&k);
			FieldDecimals(j,&d);
			ReadVal(i,j,buff);
			str[0]=0;
			
			switch(k){
				case _char:
					xToChar(k,d,buff,str);
					norm_string(str);
					break;
				case _int:
				case _double:
				case _date:
				case _time:
					xToChar(k,d,buff,str);
					break;
				case _bool:
					if(*((int*)buff)==0){
						sprintf(str,"FALSE");
					}
					else{
						sprintf(str,"TRUE");
					}
					break;
			}
			
			if(j<CountFields()){
				fprintf(_fp,"'%s', ",str);
			}
			else{
				fprintf(_fp,"'%s');\n",str);
			}
		}
	}
	
	fprintf(_fp,"END;\n");

	free(str);
	free(buff);

	return(0);
}

