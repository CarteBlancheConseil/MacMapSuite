//----------------------------------------------------------------------------
// File : bPGisTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Postgis table class
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
// 28/06/2005 creation.
//----------------------------------------------------------------------------

#include "bPGisTable.h"
#include "PGis_utils.h"
#include "C_Utils.h"
#include "bTrace.h"
#include "vx_utils.h"
#include "vx_allocation.h"
#include "db_utils.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 
// -----------
static void MacRoman2UTF8(char* str, int sz){
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,str,sz,kCFStringEncodingUTF8);
	CFRelease(cfs);
}

// ---------------------------------------------------------------------------
// 
// -----------
static void UTF82MacRoman(char* str, int sz){
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingUTF8);
	CFStringGetCString(cfs,str,sz,kCFStringEncodingMacRoman);
	CFRelease(cfs);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bPGisTable	::bPGisTable(	const char* path,
							const char* name, 
							bool create, 
							double reso,
							int srid,
							int* status)
			:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bPGisTable::bPGisTable",false);
	
	_cnx=NULL;
	_row_buf=NULL;
	_cur=-1;
	_geomid=0;
	_box2did=0;
	_box3did=0;
	strcpy(_name,name);
	_dyn_rq=NULL;
	_rq_sz=0;
	
	for(;;){
		_cnx=PQconnectdb(path);
		if(PQstatus(_cnx)!=CONNECTION_OK){
_te_("Connection failed with "+PQerrorMessage(_cnx));			
			*status=-1;
			break;
		}
		
		if(create){
			snprintf(_stat_rq,
					512,
					"CREATE TABLE \"%s\" (live bool, gid serial);\nALTER TABLE ONLY \"%s\" ADD CONSTRAINT \"%s_pkey\" PRIMARY KEY (gid);",
					name,
					name,
					name);
			if(exec(_stat_rq,PGRES_COMMAND_OK)){
_te_("creation failed with "+PQerrorMessage(_cnx));			
				*status=-2;
				break;
			}

		}
		else{
			snprintf(_stat_rq,512,
					"SELECT count(*) FROM \"%s\";",
					name);
			if(exec(_stat_rq,PGRES_TUPLES_OK)){
_te_("table "+_name+"does not exist");			
				*status=-2;
				break;
			}
		}
		if(f_geom_tid()){
_te_("no geometry id");			
			*status=-3;
			break;
		}
		*status=0;
		return;
	}
	
	PQfinish(_cnx);
	_cnx=NULL;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bPGisTable	::bPGisTable(	const char* path,
							const char* name,
							bool create, 
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
			:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status){
_bTrace_("bPGisTable::bPGisTable",false);
	
	_cnx=NULL;
	_row_buf=NULL;
	_cur=-1;
	_geomid=0;
	_box2did=0;
	_box3did=0;
	strcpy(_name,name);
	_dyn_rq=NULL;
	_rq_sz=0;
	
	for(;;){
		_cnx=PQconnectdb(path);
		if(PQstatus(_cnx)!=CONNECTION_OK){
_te_("Connection failed with "+PQerrorMessage(_cnx));			
			*status=-1;
			break;
		}
		
		if(create){
			snprintf(_stat_rq,
					512,
					"CREATE TABLE \"%s\" (live bool, gid serial);\nALTER TABLE ONLY \"%s\" ADD CONSTRAINT \"%s_pkey\" PRIMARY KEY (gid);",
					name,
					name,
					name);
			if(exec(_stat_rq,PGRES_COMMAND_OK)){
_te_("creation failed with "+PQerrorMessage(_cnx));			
				*status=-2;
				break;
			}

		}
		else{
			snprintf(_stat_rq,512,
					"SELECT count(*) FROM \"%s\";",
					name);
			if(exec(_stat_rq,PGRES_TUPLES_OK)){
_te_("table "+_name+"does not exist");			
				*status=-2;
				break;
			}
		}
		if(f_geom_tid()){
_te_("no geometry id");			
			*status=-3;
			break;
		}
		*status=0;
		return;
	}
	
	PQfinish(_cnx);
	_cnx=NULL;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bPGisTable::~bPGisTable(){
	if(_cnx){
		PQfinish(_cnx);
	}
	if(_row_buf){
		PQclear(_row_buf);
	}
	if(_dyn_rq){
		free(_dyn_rq);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::KillRecord(int o){
_bTrace_("bPGisTable::KillRecord",false);
	if((o<1)||(o>CountRecords())){
_te_("o ("+o+") out of range [1.."+CountRecords()+"]");
		return(-1);
	}
	snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"live\"='FALSE' WHERE \"gid\"=%d;",_name,o);
int status=exec(_stat_rq,PGRES_COMMAND_OK);
	if((_cur==o)&&(load_current())){
_te_("reload failed");
		return(-2);
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::UnkillRecord(int o){
_bTrace_("bPGisTable::UnkillRecord",false);
	if((o<1)||(o>CountRecords())){
_te_("o ("+o+") out of range [1.."+CountRecords()+"]");
		return(-1);
	}
	if(_cur!=o){
		_cur=o;
		if(load_current()){
			return(-2);
		}
	}
	snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"live\"='TRUE' WHERE \"gid\"=%d;",_name,o);
int status=exec(_stat_rq,PGRES_COMMAND_OK);
	if(load_current()){
_te_("reload failed");
		return(-2);
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::RecordKilled(int o){
_bTrace_("bPGisTable::CountRecords",false);
	if((o<1)||(o>CountRecords())){
_te_("o ("+o+") out of range [1.."+CountRecords()+"]");
		return(-1);
	}
	if(_cur!=o){
		_cur=o;
		if(load_current()){
_te_("load failed");
			return(-2);
		}
	}
	if(!strcmp("t",PQgetvalue(_row_buf,0,0))){
		return(0);
	}
	else{
		return(1);
	}	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::CountRecords(){
_bTrace_("bPGisTable::CountRecords",false);
	snprintf(_stat_rq,512,"SELECT count(*) FROM \"%s\";",_name);
PGresult*	res=PQexec(_cnx,_stat_rq);
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_te_("PQexec failed with "+PQerrorMessage(_cnx));
		PQclear(res);
		return(-1);
	}
int n=atoi(PQgetvalue(res,0,0));
	PQclear(res);
	return(n);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::CountFields(){
_bTrace_("bPGisTable::CountFields",false);
PGresult*	res;
	if(fields_property("attnum",&res)){
_te_("cant access property \"attnum\" "+PQerrorMessage(_cnx));
		return(-2);
	}
int n=PQntuples(res)-2;
	if(n<0){
		n=0;
	}
	PQclear(res);	
	return(n);
}

// ---------------------------------------------------------------------------
// 
// -----------
// Convention pour les géométries :
// decs = Geometry type
// les segmentations et rects sont stockées en ivx
// les coordonnées sont stockées en dvx
int	bPGisTable::AddField(const char* nam, int sign, int length, int decs){
_bTrace_("bPGisTable::AddField",false);
int		expect=PGRES_COMMAND_OK;

char	namutf8[1024];
	sprintf(namutf8,nam);
	MacRoman2UTF8(namutf8,sizeof(namutf8)-1);
	
	switch(sign){
		case _char:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" varchar(%d);",_name,namutf8,length);
			break;
		case _bool:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" bool;",_name,namutf8);
			break;
		case _int:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" int4;",_name,namutf8);
			break;
		case _double:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" float8;",_name,namutf8);
			break;
		case _date:
			//snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" date;",_name,namutf8);
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" timestamp;",_name,namutf8);
			break;
		case _time:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" time;",_name,namutf8);
			break;
		case _binary:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" bytea;",_name,namutf8);
			break;
		
		case _dvxs3:
		case _ivxs3:
			switch(decs){
				case kVXPoint:
				case 3://kText_:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOINT',3);",_name,namutf8,_tsrid);
					break;
				case kVXPolyline:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTILINESTRING',3);",_name,namutf8,_tsrid);
					break;
				case kVXPolygon:
				case 5://kRaster_:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOLYGON',3);",_name,namutf8,_tsrid);
					break;
				default:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOINT',3);",_name,namutf8,_tsrid);
					break;
			}
			expect=PGRES_TUPLES_OK;
			break;
		
		case _dvxs2:
		case _ivxs2:
			switch(decs){
				case kVXPoint:
				case 3://kText_:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOINT',2);",_name,namutf8,_tsrid);
					break;
				case kVXPolyline:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTILINESTRING',2);",_name,namutf8,_tsrid);
					break;
				case kVXPolygon:
				case 5://kRaster_:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOLYGON',2);",_name,namutf8,_tsrid);
					break;
				default:
					snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','MULTIPOINT',2);",_name,namutf8,_tsrid);
					break;
			}
			expect=PGRES_TUPLES_OK;
			break;
	
		case _ivx2:
		case _dvx2:
			snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','POINT',2);",_name,namutf8,_tsrid);
			expect=PGRES_TUPLES_OK;
			break;
			
		case _ivx3:
		case _dvx3:
			snprintf(_stat_rq,512,"SELECT AddGeometryColumn('','%s','%s','%d','POINT',3);",_name,namutf8,_tsrid);
			expect=PGRES_TUPLES_OK;
			break;

		case _dvxrect:
		case _ivxrect:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" box2d;",_name,namutf8);
			break;

		case _dvxcube:
		case _ivxcube:
			snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ADD COLUMN \"%s\" box3d;",_name,namutf8);
			break;
			
		default:
_err_(_strr_+"bad field kind ("+(UInt32*)&sign+")");
			return(-1);
			break;
	}
int status=exec(_stat_rq,expect);
	if((!status)&&(CountRecords()>0)){
		status=load_current();
	}
	if(status){
_te_("exec failed for : "+_stat_rq);		
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::RmvField(int f){
_bTrace_("bPGisTable::RmvField",false);

int		expect=PGRES_COMMAND_OK;
char	field[256];
	FieldName(f,field);
	MacRoman2UTF8(field,sizeof(field)-1);
	
int tid=natural_sign(f+1);
	if(tid==_geomid){
		snprintf(_stat_rq,512,"SELECT DropGeometryColumn('%s','%s');",_name,field);
		expect=PGRES_TUPLES_OK;
	}
	else{
		snprintf(_stat_rq,512,"ALTER TABLE \"%s\" DROP COLUMN \"%s\";",_name,field);
		expect=PGRES_COMMAND_OK;
	}
int status=exec(_stat_rq,expect);
	if(status){
_err_(_strr_+"exec failed for "+_stat_rq);
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::ChgField(int f, const char* nam, int sign, int length, int decs){
_bTrace_("bPGisTable::ChgField",false);
// On peut changer le nom
// On renvoie 0 pour decs ?

int		tid=natural_sign(f+1);
int		nat_sgn=0;
char	field[256];
	FieldName(f,field);
	MacRoman2UTF8(field,sizeof(field)-1);

	switch(tid){
		case 20:
		case 21:
		case 23:
			nat_sgn=_int;
			break;
		case 700:
		case 701:
		case 1700:
			nat_sgn=_double;
			break;
		case 1082://
		case 1114:
			nat_sgn=_date;
			break;
		case 1083:
			nat_sgn=_time;
			break;
		case 1043:
			nat_sgn=_char;
			break;
		case 17:
			nat_sgn=_binary;
			break;
		case 16:
			nat_sgn=_bool;
			break;
	}
	
	if(nat_sgn!=sign){
		switch(sign){
			case _char:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE varchar(%d);",_name,field,length);
				break;
			case _bool:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE bool;",_name,field);
				break;
			case _int:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE int4;",_name,field);
				break;
			case _double:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE float8;",_name,field);
				break;
			case _date:
//				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE date;",_name,field);
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE timestamp;",_name,field);
				break;
			case _time:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE time;",_name,field);
				break;
			case _binary:
				snprintf(_stat_rq,512,"ALTER TABLE \"%s\" ALTER COLUMN \"%s\" TYPE bytea;",_name,field);
				break;
			default:
_err_(_strr_+"bad field sign for conversion ("+(UInt32*)&sign+")");
				return(-1);
		}
		rq_add(_stat_rq);
	}	

	if(!strcmp(nam,field)){
		snprintf(_stat_rq,512,"ALTER TABLE \"%s\" RENAME COLUMN \"%s\" TO \"%s\";\n",_name,field,nam);
		rq_add(_stat_rq);
	}

//	Longueur ?
	
int status=0;
	if(_dyn_rq){
		status=exec(_dyn_rq,PGRES_COMMAND_OK);
		free(_dyn_rq);
		_dyn_rq=NULL;
		_rq_sz=0;
	}
	else{
		status=-1;
	}
	if(!status){	
		status=load_current();
	}
	return(status
	);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::FieldSign(int f, int* sign){
_bTrace_("bPGisTable::FieldSign",false);
int tid=natural_sign(f+1);
	if(tid==_geomid){
		switch(natural_geom_sign(f+1)){
			case MULTIPOLYGONTYPE:
			case POLYGONTYPE:
			case MULTILINETYPE:
			case LINETYPE:
			case MULTIPOINTTYPE:
				*sign=_ivxs2;
				break;
			case POINTTYPE:
				*sign=_ivx2;
				break;
			default:
				*sign='NULL';
_err_(_strr_+"bad geom field kind");
				return(-1);
				break;
		}
	}
	else if(tid==_box2did){
		*sign=_ivxrect;
	}
	else if(tid==_box3did){
		*sign=_ivxcube;
	}
	else switch(tid){
		case 20:
		case 21:
		case 23:
			*sign=_int;
			break;
		case 700:
		case 701:
		case 1700:
			*sign=_double;
			break;
//		case 1082:// Date
		case 1114:// Date = Timestamp
			*sign=_date;
			break;
		case 1083:// Time
			*sign=_time;
			break;
		case 1043://varchar
			*sign=_char;
			break;
		case 17://bytea
			*sign=_binary;
			break;
		case 16://bool
			*sign=_bool;
			break;
			
		default:
			*sign='NULL';
_err_(_strr_+"bad field kind "+tid);
			return(-2);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::FieldName(int f, char* name){
	if(_row_buf){
		strcpy(name,PQfname(_row_buf,f+1));
		UTF82MacRoman(name,255);
		return(0);
	}
int	r=field_property(f+1,"attname",name);
	UTF82MacRoman(name,255);
	return(r);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::FieldDecimals(int f, int* decs){
int sign=natural_sign(f+1);
	switch(sign){
		case 700:
		case 701:
		case 1700:
			*decs=6;
			break;
		default:
			*decs=0;
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::FieldLength(int f, int* len){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::FieldSize(int f, int* sz){
int sign=natural_sign(f+1);
	if(sign==_geomid){
		*sz=sizeof(ivertices*);
	}
	else if(sign==_box2did){
		*sz=sizeof(ivx_rect);
	}
	else if(sign==_box3did){
		*sz=sizeof(ivx_cube);
	}
	else switch(sign){
		case 1043:
			*sz=sizeof(char*);
			break;
		case 17:
			*sz=sizeof(DB_cont);
			break;
		case 16:	
		case 20:
		case 21:
		case 23:
			*sz=sizeof(int);
			break;
		case 700:
		case 701:
		case 1700:
		case 1082://
		case 1114:
		case 1083:
			*sz=sizeof(double);
			break;
		default:
			*sz=0;
			return(-1);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::ReadVal(int o, int f, void* val){
_bTrace_("bPGisTable::ReadVal",false);
	if((f<1)||(f>CountFields())){
_err_(_strr_+"f ("+f+") out of range [1.."+CountFields()+"]");
		return(-1);
	}
	if(o!=_cur){
		_cur=o;
		if(load_current()){
_err_(_strr_+"load_current failed");
			return(-2);
		}
	}

//trc.msg("o=%d;f=%d",o,f);
	
int sign=natural_sign(f+1);
//size_t junk;
//const char *cval;

	if(sign==_geomid){
byte* wkb;
		switch(natural_geom_sign(f+1)){
			case MULTIPOLYGONTYPE:
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				(*((ivertices**)val))=read_multipolygon2D(wkb);
				free(wkb);
				break;
			case POLYGONTYPE:
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				(*((ivertices**)val))=read_polygon2D(wkb);
				free(wkb);
				break;
			case MULTILINETYPE:
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				(*((ivertices**)val))=read_multiline2D(wkb);
				free(wkb);
				break;
			case LINETYPE:
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				(*((ivertices**)val))=read_line2D(wkb);
				free(wkb);
				break;
			case MULTIPOINTTYPE:
//trc.msg("MULTIPOINTTYPE>");
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				if(wkb==NULL){
_err_(_strr_+"NULL WKB");					
				}
				(*((ivertices**)val))=read_multipoint2D(wkb);
				free(wkb);
//trc.msg("MULTIPOINTTYPE<");
				break;
			case POINTTYPE:
				wkb=HexDecode((byte*)PQgetvalue(_row_buf,0,f+1));
				(*((i2dvertex*)val))=read_point2D(wkb);
				free(wkb);
				break;
			default:
_err_(_strr_+"bad geom field kind");
				return(-1);
				break;
		}
	}
	else if(sign==_box2did){
		(*((ivx_rect*)val))=read_box2d(PQgetvalue(_row_buf,0,f+1));
	}
	else if(sign==_box3did){
		(*((ivx_cube*)val))=read_box3d(PQgetvalue(_row_buf,0,f+1));
	}
	else switch(sign){
		case 1043:
		// varchar
			strcpy((char*)val,PQgetvalue(_row_buf,0,f+1));
			break;
		case 16:
		// bool
			if(!strcmp("t",PQgetvalue(_row_buf,0,f+1))){
				(*((int*)val))=1;
			}
			else{
				(*((int*)val))=0;
			}	
			break;
		case 17:
		// bytea
			break;
			
		case 20:
		case 21:
		case 23:
		// int
			(*((int*)val))=atoi(PQgetvalue(_row_buf,0,f+1));
			break;
			
		case 700:
		case 701:
		case 1700:
		// double
			(*((double*)val))=atof(PQgetvalue(_row_buf,0,f+1));
			break;
			
		//case 1082:
		case 1114:
		// date=timestamp
			(*((double*)val))=make_datetime(PQgetvalue(_row_buf,0,f+1));
			break;
		case 1083:
		// time
			(*((double*)val))=make_time(PQgetvalue(_row_buf,0,f+1));
			break;
		
		default:
_err_(_strr_+"bad field kind");
			return(-1);
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::WriteVal(int o, int f, void* val){
_bTrace_("bPGisTable::WriteVal",true);
	if((f<1)||(f>CountFields())){
_err_(_strr_+"f ("+f+") out of range [1.."+CountFields()+"]");
		return(-1);
	}
	if(o==CountRecords()+1){
		o=append();
		if(o<0){
_err_(_strr_+"append failed with "+o);
			return(-1);
		}
	}

	if(o!=_cur){
		_cur=o;
		if(load_current()){
_err_(_strr_+"load_current failed");
			return(-2);
		}
	}
	
char	field[1024];
	FieldName(f,field);
	MacRoman2UTF8(field,sizeof(field)-1);

int		sign=natural_sign(f+1);

	if(sign==_geomid){
		switch(natural_geom_sign(f+1)){
			case MULTIPOLYGONTYPE:
			case POLYGONTYPE:
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
				rq_add(_stat_rq);
				write_polygon((*((ivertices**)val)));
				snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
				rq_add(_stat_rq);
				break;
			case MULTILINETYPE:
			case LINETYPE:
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
				rq_add(_stat_rq);
				write_linestring((*((ivertices**)val)));
				snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
				rq_add(_stat_rq);
				break;
			case MULTIPOINTTYPE:
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
				rq_add(_stat_rq);
				write_multipoint((*((ivertices**)val)));
				snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
				rq_add(_stat_rq);
				break;
			case POINTTYPE:
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
				rq_add(_stat_rq);
				write_point((i2dvertex*)val);
				snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
				rq_add(_stat_rq);
				break;
			default:
_err_(_strr_+"bad geom field kind");
				return(-1);
				break;
		}
	}
	else if(sign==_box2did){
		snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
		rq_add(_stat_rq);
		write_box2d((ivx_rect*)val);
		snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
		rq_add(_stat_rq);
	}
	else if(sign==_box3did){
		snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
		rq_add(_stat_rq);
		write_box3d((ivx_cube*)val);
		snprintf(_stat_rq,512,"' WHERE gid='%d'\n",_cur);
		rq_add(_stat_rq);
	}
	else switch(sign){
		case 1043:{
// varchar
char	buf[1024];
			snprintf(buf,sizeof(buf)-1,"%s",(char*)val);
			MacRoman2UTF8(buf,sizeof(buf)-1);
			snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='%s' WHERE \"gid\"='%d';",_name,field,buf,_cur);
			rq_add(_stat_rq);
		}break;
		case 16:
// bool
			if((*((int*)val))){
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='TRUE' WHERE \"gid\"='%d';",_name,field,_cur);
				rq_add(_stat_rq);
			}
			else{
				snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='FALSE' WHERE \"gid\"='%d';",_name,field,_cur);
				rq_add(_stat_rq);
			}
			break;
		case 17:
// bytea
			return(0);
			break;
			
		case 20:
		case 21:
		case 23:
// int
			snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"=%d WHERE \"gid\"='%d';",_name,field,(*((int*)val)),_cur);
			rq_add(_stat_rq);
			break;
		case 700:
		case 701:
		case 1700:
// double
			snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='%.20f' WHERE \"gid\"='%d';",_name,field,(*((double*)val)),_cur);
			rq_add(_stat_rq);
			break;
//		case 1082:{
		case 1114:{
// date
			snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
			rq_add(_stat_rq);
			
time_t		t=(*((double*)val));
struct tm*	tt=localtime(&t);
			(void)strftime(_stat_rq,512,"%Y-%m-%d %H:%M:%S",tt);
			rq_add(_stat_rq);
		
			snprintf(_stat_rq,512,"' WHERE \"gid\"='%d';",_cur);
			rq_add(_stat_rq);
			}
			break;
		case 1083:{
// time 
			snprintf(_stat_rq,512,"UPDATE \"%s\" SET \"%s\"='",_name,field);
			rq_add(_stat_rq);
			
time_t		t=(*((double*)val));
struct tm*	tt=localtime(&t);
			(void)strftime(_stat_rq,512,"%H:%M:%S",tt);
			rq_add(_stat_rq);
		
			snprintf(_stat_rq,512,"' WHERE \"gid\"='%d';",_cur);
			rq_add(_stat_rq);
			}
			break;
		
		default:
_err_(_strr_+"bad field kind");
			return(-1);
			break;
	}	
int status=0;
	if(_dyn_rq){
		status=exec(_dyn_rq,PGRES_COMMAND_OK);
		free(_dyn_rq);
		_dyn_rq=NULL;
		_rq_sz=0;
	}
	else{
		status=-5;
	}
	if(!status){	
		status=load_current();
	}
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bPGisTable::signature(){
	return('tPGi');
}

// ---------------------------------------------------------------------------
// 
// -----------
#pragma mark -
#pragma mark -> PGIS STUFF
// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::rq_add(const char* part){
//_bTrace_("bPGisTable::rq_add",false);
	if(_dyn_rq==NULL){
		_rq_sz=strlen(part)+1;
		_dyn_rq=(char*)malloc(_rq_sz);
		memmove((void*)_dyn_rq,(void*)part,_rq_sz);
	}
	else{
		int sz=strlen(part);
		_dyn_rq=(char*)realloc(_dyn_rq,_rq_sz+sz);		
		void* buf=(void*)(_dyn_rq+_rq_sz-1);
		memmove(buf,(void*)part,sz+1);
		_rq_sz+=sz;
	}
//trc.msg(_dyn_rq);
}

// ---------------------------------------------------------------------------
// simple exec
// -----------
int	bPGisTable::exec(char* req, int expect){
_bTrace_("bPGisTable::exec",false);
PGresult*	res=PQexec(_cnx,req);
	if(int status=PQresultStatus(res)!=expect){
_err_(_strr_+"exec failed with "+PQerrorMessage(_cnx)+" ("+status+")");
_err_(_strr_+"request was : "+req);
		PQclear(res);
		return(-2);
	}
	PQclear(res);
	return(0);
}

// ---------------------------------------------------------------------------
// load buffer
// -----------
int	bPGisTable::load_current(){
_bTrace_("bPGisTable::load_current",false);
	if((_cur<1)||(_cur>CountRecords())){
_err_(_strr_+"_cur ("+_cur+") out of range [1.."+CountRecords()+"]");
		return(-1);	
	}
PGresult* res=PQexec(_cnx,"BEGIN");
	if(PQresultStatus(res)!=PGRES_COMMAND_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx)+" for BEGIN");
		PQclear(res);
		return(-1);
	}
	PQclear(res);

	snprintf(_stat_rq,512,"DECLARE \"%s_port\" CURSOR FOR SELECT * from \"%s\" where \"gid\"=%d",_name,_name,_cur);
	res=PQexec(_cnx,_stat_rq);
	if(PQresultStatus(res)!=PGRES_COMMAND_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx)+" for DECLARE CURSOR");
		PQclear(res);
		return(-2);
	}
	PQclear(res);
	
	snprintf(_stat_rq,512,"FETCH ALL in \"%s_port\"",_name);
	res=PQexec(_cnx,_stat_rq);
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx)+" for FETCH ALL");
		PQclear(res);
		return(-3);
	}

	_row_buf=res;
	
	snprintf(_stat_rq,512,"CLOSE \"%s_port\"",_name);
	res=PQexec(_cnx,_stat_rq);
	PQclear(res);

	res=PQexec(_cnx,"END");
	PQclear(res);
	
	return(0);
}

// ---------------------------------------------------------------------------
// Append new row, returns new row gid, < 0 if error
// -----------
int	bPGisTable::append(){
_bTrace_("bPGisTable::append",false);
// Empty Insert
	snprintf(_stat_rq,
			512,
			"INSERT INTO \"%s\" (\"live\",\"gid\") "
			"VALUES ('TRUE',nextval('public.\"%s_gid_seq\"'::text));",
			_name,
			_name);
	if(exec(_stat_rq,PGRES_COMMAND_OK)){
_err_(_strr_+"exec failed with "+PQerrorMessage(_cnx)+" for INSERT");
		return(-2);	
	}
// Get last value of gid sequence
	snprintf(	_stat_rq,
				512,
				"SELECT last_value FROM \"%s_gid_seq\";",
				_name);
PGresult*	res=PQexec(_cnx,_stat_rq);
if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx));
		PQclear(res);
		return(-1);
	}
// Returns last gid
int o=atoi(PQgetvalue(res,0,0));
	return(o);
}

// ---------------------------------------------------------------------------
// Access to specified property of fields
// -----------
int	bPGisTable::fields_property(const char* property_name, PGresult** res){
_bTrace_("bPGisTable::fields_property",false);
	*res=NULL;
	snprintf(_stat_rq,512,	
			"SELECT a.%s FROM "
			"pg_attribute a, pg_class c WHERE "
			"a.attrelid = c.oid and a.attnum > 0 AND "
			"a.atttypid != 0 AND "
			"c.relname = '%s'",
			property_name,
			_name);
	*res=PQexec(_cnx,_stat_rq);
	if(PQresultStatus(*res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx));
		return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// Access to specified property of specified field
// -----------
int	bPGisTable::field_property(int f, const char* property_name, char* property_value){
_bTrace_("bPGisTable::field_property",false);
	if(f<2){
_err_(_strr_+"bad field index ("+f+")");
		return(-1);
	}
PGresult*	res;
	if(fields_property(property_name,&res)){
		return(-2);
	}
	if(f>PQntuples(res)){
_err_(_strr_+"bad field index ("+f+") for "+PQntuples(res)+" tuples");
		PQclear(res);	
		return(-3);
	}
	strcpy(property_value,PQgetvalue(res,f,0));
	PQclear(res);	
	return(0);
}

// ---------------------------------------------------------------------------
// returns type kind of f field
// -----------
int	bPGisTable::natural_sign(int f){
_bTrace_("bPGisTable::natural_sign",false);
char name[256];
int status=field_property(f,"atttypid",name);
	if(status){
		return(0);
	}
	return(atoi(name));
}

// ---------------------------------------------------------------------------
// returns type kind of f geometry field
// -----------
// ATTENTION : CA NE MARCHE QUE SI _ROW_BUF est CHARGE
int	bPGisTable::natural_geom_sign(int f){
_bTrace_("bPGisTable::natural_geom_sign",false);
	snprintf(	_stat_rq,
				512,
				"SELECT \"type\" FROM \"geometry_columns\" "
				"WHERE \"f_table_name\"='%s'"
				" AND \"f_geometry_column\"='%s';",
				_name,
				PQfname(_row_buf,f));
PGresult* res=PQexec(_cnx,_stat_rq);
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx)+" for "+_stat_rq);
		PQclear(res);
		return(0);
	}

	if(PQntuples(res)<=0){
_err_(_strr_+"Geometry field not found for "+_stat_rq);
		PQclear(res);
		return(0);
	}		
int sgn=0;
	if(!strcmp(PQgetvalue(res,0,0),"MULTIPOLYGON")){
		sgn=MULTIPOLYGONTYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"MULTILINESTRING")){
		sgn=MULTILINETYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"MULTIPOINT")){
		sgn=MULTIPOINTTYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"POLYGON")){
		sgn=POLYGONTYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"LINESTRING")){
		sgn=LINETYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"POINT")){
		sgn=POINTTYPE;
	}
	else if(!strcmp(PQgetvalue(res,0,0),"GEOMETRYCOLLECTION")){
		sgn=COLLECTIONTYPE;
	}
	PQclear(res);
	return(sgn);
}

// ---------------------------------------------------------------------------
// init geometry type
// -----------
int bPGisTable::f_geom_tid(){
_bTrace_("bPGisTable::f_geom_tid",false);
PGresult* res=PQexec(_cnx,"SELECT OID FROM pg_type WHERE typname='geometry'");	
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx));
		PQclear(res);
		return(-1);
	}
	if(PQntuples(res)<=0){
_err_(_strr_+"geometry type not found");
		PQclear(res);
		return(-1);
	}
	_geomid=atoi(PQgetvalue(res,0,0));
	PQclear(res);

	res=PQexec(_cnx,"SELECT OID FROM pg_type WHERE typname='box2d'");	
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx));
		PQclear(res);
		return(-1);
	}
	if(PQntuples(res)<=0){
_err_(_strr_+"geometry type not found");
		PQclear(res);
		return(-1);
	}
	_box2did=atoi(PQgetvalue(res,0,0));
	PQclear(res);

	res=PQexec(_cnx,"SELECT OID FROM pg_type WHERE typname='box3d'");	
	if(PQresultStatus(res)!=PGRES_TUPLES_OK){
_err_(_strr_+"PQexec failed with "+PQerrorMessage(_cnx));
		PQclear(res);
		return(-1);
	}
	if(PQntuples(res)<=0){
_err_(_strr_+"geometry type not found");
		PQclear(res);
		return(-1);
	}
	_box3did=atoi(PQgetvalue(res,0,0));
	PQclear(res);

	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
#pragma mark -
#pragma mark -> WRITING GEOMETRIES
// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::print_wkb_bytes(unsigned char *ptr, unsigned int cnt, size_t size){
_bTrace_("bPGisTable::print_wkb_bytes",false);
unsigned int	bc;
static char		buf[256];
char*			bufp;

	if(size*cnt*2>256){
		return;
	}

	bufp=buf;
	while(cnt--){
		for(bc=0;bc<size;bc++){
			*bufp++=outchr[ptr[bc]>>4];
			*bufp++=outchr[ptr[bc]&0x0F];
		}
	}
	*bufp='\0';
	rq_add(buf);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::print_wkb_double(double val){
	print_wkb_bytes((unsigned char*)&val,1,8);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::print_wkb_byte(unsigned char val){
	print_wkb_bytes((unsigned char*)&val,1,1);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::print_wkb_int(int val){
	print_wkb_bytes((unsigned char*)&val,1,4);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_polygon(ivertices* vxs){
//_bTrace_("bPGisTable::write_polygon",false);
Ring**			Outer;				// Pointers to Outer rings
int				out_index=0;		// Count of Outer rings
Ring**			Inner;				// Pointers to Inner rings
int				in_index=0;			// Count of Inner rings
int				pi;					// part index
unsigned int	wkbtype=MULTIPOLYGONTYPE;
unsigned int	subtype=POLYGONTYPE|(wkbtype&WKBZOFFSET);
int				nParts=(vxs->no)?vxs->no:1;
d2dvertex		vxa,vxb;

// Allocate initial memory
	Outer=(Ring**)malloc(sizeof(Ring*)*nParts);
	Inner=(Ring**)malloc(sizeof(Ring*)*nParts);

// Iterate over rings dividing in Outers and Inners
	for(pi=0;pi<nParts;pi++){
int		vi; // vertex index
int		vs; // start index
int		ve; // end index
int		nv; // number of vertex
double	area = 0.0;
Ring*	ring;

// Set start and end vertexes
		ve=(pi==nParts-1)?vxs->nv:vxs->offs[pi+1];
		//vs=(vxs->no)?vxs->offs[pi+1]:0;
		vs=(vxs->no)?vxs->offs[pi]:0;

// Compute number of vertexes
		nv=ve-vs;

// Allocate memory for a ring
		ring=(Ring*)malloc(sizeof(Ring));
		ring->list=(gPoint*)malloc(sizeof(gPoint)*nv);
		ring->n=nv;
		ring->next=NULL;
		ring->linked=0;

// Iterate over ring vertexes
		for(vi=vs;vi<ve;vi++){
			int vn=vi+1; // next vertex for area
			if(vn==ve){
				vn=vs;
			}
			
			vx_i2d(&vxa,&vxs->vx.vx2[vi],_reso);
			vxa.x+=_ox;
			vxa.y+=_oy;
			vx_i2d(&vxb,&vxs->vx.vx2[vn],_reso);
			vxb.x+=_ox;
			vxb.y+=_oy;
			
			ring->list[vi-vs].x=vxa.x;
			ring->list[vi-vs].y=vxa.y;
			
			area+=(vxa.x*vxb.y)-(vxa.y*vxb.x);
		}

// Clockwise (or single-part). It's an Outer Ring !
		if(area<0.0||nParts==1) {
			Outer[out_index]=ring;
			out_index++;
		}
// Counterclockwise. It's an Inner Ring !
		else{
			Inner[in_index]=ring;
			in_index++;
		}
	}

// Put the inner rings into the list of the outer rings
// of which they are within
	for(pi=0; pi<in_index; pi++){
gPoint	pt,pt2;
int		i;
Ring*	inner=Inner[pi],*outer=NULL;

		pt.x=inner->list[0].x;
		pt.y=inner->list[0].y;

		pt2.x=inner->list[1].x;
		pt2.y=inner->list[1].y;

		for(i=0;i<out_index;i++){
int in=PIP(pt,Outer[i]->list,Outer[i]->n);
			if(in||PIP(pt2,Outer[i]->list,Outer[i]->n)){
				outer=Outer[i];
				break;
			}
		}

		if(outer){
			outer->linked++;
			while(outer->next){
				outer=outer->next;
			}
			outer->next=inner;
		}
		else{
// The ring wasn't within any outer rings,
// assume it is a new outer ring.
			Outer[out_index]=inner;
			out_index++;
		}
	}
	
	if(_tsrid!=-1){
		snprintf(_stat_rq,512,"SRID=%d;",_tsrid);
		rq_add(_stat_rq);
	}

	print_wkb_byte(getEndianByte());
	print_wkb_int(wkbtype);
	print_wkb_int(out_index);

// Write the coordinates
	for(pi=0;pi<out_index;pi++){
Ring*	poly;

		poly=Outer[pi];

		print_wkb_byte(getEndianByte());
		print_wkb_int(subtype);
		print_wkb_int(poly->linked+1);

		while(poly){
int vi;
			print_wkb_int(poly->n);
			for(vi=0;vi<poly->n;vi++){
				print_wkb_double(poly->list[vi].x);
				print_wkb_double(poly->list[vi].y);
				/*if(wkbtype&WKBZOFFSET){
					print_wkb_double(poly->list[vi].z);
				}
				if ( wkbtype & WKBMOFFSET )
				print_wkb_double(obj->padfM[vi]);*/
			}
			poly=poly->next;
		}

	}
	
// Release all memory
	for(pi=0;pi<out_index;pi++){
Ring	*Poly,*temp;
		Poly=Outer[pi];
		while(Poly!=NULL){
			temp=Poly;
			Poly=Poly->next;
			free(temp->list);
			free(temp);
		}
	}
	free(Outer);
	free(Inner);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_linestring(ivertices* vxs){
_bTrace_("bPGisTable::write_linestring",false);
int				pi; // part index
unsigned int	wkbtype=MULTILINETYPE;
unsigned int	subtype=	LINETYPE |
							(wkbtype&WKBZOFFSET) | 
							(wkbtype&WKBMOFFSET);
int				nParts=(vxs->no)?vxs->no:1;
d2dvertex		vx;

// Invalid (MULTI)Linestring
	if(vxs->nv<2){
_err_(_strr_+"MULTILINESTRING vertices invalid, set to NULL\n");
		rq_add("NULL");
		return;
	}

	if(_tsrid!=-1){
		snprintf(_stat_rq,512,"SRID=%d;",_tsrid);
		rq_add(_stat_rq);
	}
	
	print_wkb_byte(getEndianByte());
	print_wkb_int(wkbtype);
	print_wkb_int(nParts);

	for(pi=0;pi<nParts;pi++){
int	vs; // start vertex
int	ve; // end vertex
		
		print_wkb_byte(getEndianByte());
		print_wkb_int(subtype);
		
// Set start and end vertexes
		ve=(pi==nParts-1)?vxs->nv:vxs->offs[pi+1];
		vs=(vxs->no)?vxs->offs[pi]:0;

		print_wkb_int(ve-vs);
		for(int vi=vs;vi<ve;vi++){
			vx_i2d(&vx,&vxs->vx.vx2[vi],_reso);
			vx.x+=_ox;
			vx.y+=_oy;
			print_wkb_double(vx.x);
			print_wkb_double(vx.y);
		}
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_multipoint(ivertices* vxs){
unsigned int	u;
unsigned int	wkbtype=MULTIPOINTTYPE;
unsigned int	subtype=POINTTYPE |
						(wkbtype&WKBZOFFSET) | 
						(wkbtype&WKBMOFFSET);
d2dvertex		vx;

	if(_tsrid!=-1){
		snprintf(_stat_rq,512,"SRID=%d;",_tsrid);
		rq_add(_stat_rq);
	}

	print_wkb_byte(getEndianByte());
	print_wkb_int(wkbtype);
	print_wkb_int(vxs->nv);
	
	for(u=0;u<vxs->nv;u++){
		print_wkb_byte(getEndianByte());
		print_wkb_int(subtype);
		vx_i2d(&vx,&vxs->vx.vx2[u],_reso);
		vx.x+=_ox;
		vx.y+=_oy;
		print_wkb_double(vx.x);
		print_wkb_double(vx.y);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_point(i2dvertex* vx){
d2dvertex		dvx;
	if(_tsrid!=-1){
		snprintf(_stat_rq,512,"SRID=%d;",_tsrid);
		rq_add(_stat_rq);
	}
	vx_i2d(&dvx,vx,_reso);
	dvx.x+=_ox;
	dvx.y+=_oy;
	print_wkb_byte(getEndianByte());
	print_wkb_int(POINTTYPE);
	print_wkb_double(dvx.x);
	print_wkb_double(dvx.y);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_box2d(ivx_rect* vr){
i2dvertex	vx;
d2dvertex	vxa,vxb;
	
	vx.h=vr->left;
	vx.v=vr->top;
	vx_i2d(&vxa,&vx,_reso);
	vxa.x+=_ox;
	vxa.y+=_oy;

	vx.h=vr->right;
	vx.v=vr->bottom;
	vx_i2d(&vxb,&vx,_reso);
	vxb.x+=_ox;
	vxb.y+=_oy;

	snprintf(_stat_rq,256,"BOX(%.6f %.6f,%.6f %.6f)",vxb.x,vxa.y,vxa.x,vxb.y);
	
	rq_add(_stat_rq);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bPGisTable::write_box3d(ivx_cube* vr){
i2dvertex	vx;
d2dvertex	vxa,vxb;
	
	vx.h=vr->left;
	vx.v=vr->top;
	vx_i2d(&vxa,&vx,_reso);
	vxa.x+=_ox;
	vxa.y+=_oy;
	vx.h=vr->right;
	vx.v=vr->bottom;
	vx_i2d(&vxb,&vx,_reso);
	vxb.x+=_ox;
	vxb.y+=_oy;
	
	snprintf(_stat_rq,256,"BOX(%.6f %.6f 0,%.6f %.6f 0)",vxb.x,vxa.y,vxa.x,vxb.y);
	rq_add(_stat_rq);
}

// ---------------------------------------------------------------------------
// 
// -----------
#pragma mark -
#pragma mark -> READING GEOMETRIES
// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::read_multipolygon2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_multipolygon2D",false);
UInt32	nrings,nparts=0;
UInt32	npolys;
UInt32	totpoints=0;
int*	part_index=NULL;
UInt32	pi;
double	*x=NULL,*y=NULL;
int		zmflag,gk;
byte	ed;

// byteOrder 
	ed=popbyte(&wkb); 
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
// Scan all polygons in multipolygon
	npolys=popint(&wkb,ed!=getEndianByte());// num_wkbPolygons
// Now wkb points to a WKBPolygon structure
	for(pi=0;pi<npolys;pi++){
UInt32	ri;
// skip byteOrder and wkbType
		ed=popbyte(&wkb); 
		skipint(&wkb);
// Find total number of points and
// fill part index
		nrings=popint(&wkb,ed!=getEndianByte());
		part_index=(int *)realloc(part_index,sizeof(int)*(nparts+nrings));
// wkb now points at first ring
		for(ri=0;ri<nrings;ri++){
UInt32 pn; // point number
UInt32 npoints;
			npoints=popint(&wkb,ed!=getEndianByte());
			x=(double*)realloc(x,sizeof(double)*(totpoints+npoints));
			y=(double*)realloc(y,sizeof(double)*(totpoints+npoints));
// wkb now points at first point 
			for(pn=0;pn<npoints;pn++){
				x[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
				y[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
			}
// First ring should be clockwise,
// other rings should be counter-clockwise
			if(!ri){
				if(!is_clockwise(	npoints,
									x+totpoints,
									y+totpoints)){
					reverse_points(	npoints,
									x+totpoints,
									y+totpoints,
									NULL,
									NULL);
				}
			}
			else{
				if(is_clockwise(	npoints,
									x+totpoints,
									y+totpoints)){
					reverse_points(	npoints,
									x+totpoints,
									y+totpoints,
									NULL,
									NULL);
				}
			}
			part_index[nparts+ri]=totpoints;
			totpoints+=npoints;
		}
		nparts+=nrings;
	}

ivertices*	vxs=make_vertices(totpoints,x,y,nparts,part_index);
	
	free(part_index);
	free(x);
	free(y);
	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::read_polygon2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_polygon2D",false);
int		ri,nrings,totpoints=0,*part_index=NULL;
double	*x=NULL,*y=NULL;
int		zmflag,gk;
byte	ed;
	
// byteOrder 
	ed=popbyte(&wkb); 
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
// Scan all rings
	nrings=popint(&wkb,ed!=getEndianByte());
	part_index=(int*)malloc(sizeof(int)*nrings);
	for(ri=0; ri<nrings; ri++){
int pn;
int npoints=popint(&wkb,ed!=getEndianByte());
		x=(double*)realloc(x,sizeof(double)*(totpoints+npoints));
		y=(double*)realloc(y,sizeof(double)*(totpoints+npoints));
		for(pn=0;pn<npoints;pn++){
			x[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
			y[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
		}
// First ring should be clockwise,
// other rings should be counter-clockwise
		if(!ri){
			if(is_clockwise(npoints,
							x+totpoints,
							y+totpoints)){
				reverse_points(	npoints,		
								x+totpoints,
								y+totpoints,
								NULL,
								NULL);
			}
		}
		else{
			if(is_clockwise(npoints,
							x+totpoints,
							y+totpoints)){
				reverse_points(	npoints,		
								x+totpoints,
								y+totpoints,
								NULL,
								NULL);
			}
		}
		part_index[ri]=totpoints;
		totpoints+=npoints;
	}

ivertices*	vxs=make_vertices(totpoints,x,y,nrings,part_index);
	
	free(part_index);
	free(x);
	free(y);
	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::read_multiline2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_multiline2D",false);
double	*x=NULL,*y=NULL;
int		*part_index=NULL,totpoints=0,nlines=0;
int		li;
int		zmflag,gk;
byte	ed;

// byteOrder 
	ed=popbyte(&wkb);
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
// Scan all lines in multiline
	nlines=popint(&wkb,ed!=getEndianByte()); // num_wkbLineStrings
//trc.msg("nlines=%d",nlines);
	part_index=(int *)malloc(sizeof(int)*(nlines));
	for(li=0;li<nlines;li++){
int	npoints, pn;
// skip byteOrder and wkbType
		ed=popbyte(&wkb);
		skipint(&wkb);
		npoints=popint(&wkb,ed!=getEndianByte());
		x=(double*)realloc(x,sizeof(double)*(totpoints+npoints));
		y=(double*)realloc(y,sizeof(double)*(totpoints+npoints));
// wkb now points at first point
		for(pn=0;pn<npoints;pn++){
			x[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
			y[totpoints+pn]=popdouble(&wkb,ed!=getEndianByte());
		}
		part_index[li]=totpoints;
		totpoints+=npoints;
	}

ivertices*	vxs=make_vertices(totpoints,x,y,nlines,part_index);
	
	free(part_index);
	free(x);
	free(y);
	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::read_line2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_line2D",false);
double	*x=NULL,*y=NULL;
UInt32	npoints=0,pn;
int		zmflag,gk;
byte	ed;
	
// byteOrder 
	ed=popbyte(&wkb);
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
	npoints=popint(&wkb,ed!=getEndianByte());
	x=(double*)malloc(sizeof(double)*(npoints));
	y=(double*)malloc(sizeof(double)*(npoints));
// wkb now points at first point
	for(pn=0;pn<npoints; pn++){
		x[pn]=popdouble(&wkb,ed!=getEndianByte());
		y[pn]=popdouble(&wkb,ed!=getEndianByte());
	}

ivertices*	vxs=make_vertices(npoints,x,y,0,NULL);
	free(x);
	free(y);	
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::read_multipoint2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_multipoint2D",true);
double	*x=NULL,*y=NULL;
int*	parts;
UInt32	npoints;
UInt32	pn;
int		zmflag,gk;
byte	ed;

// byteOrder 
	ed=popbyte(&wkb);
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
	npoints=popint(&wkb,ed!=getEndianByte());
	x=(double*)malloc(sizeof(double)*npoints);
	y=(double*)malloc(sizeof(double)*npoints);
	parts=(int*)malloc(sizeof(int)*npoints);
	for(pn=0;pn<npoints;pn++){
		ed=popbyte(&wkb);
		skipint(&wkb);
		x[pn]=popdouble(&wkb,ed!=getEndianByte());
		y[pn]=popdouble(&wkb,ed!=getEndianByte());
		parts[pn]=pn;
	}
ivertices*	vxs=make_vertices(npoints,x,y,npoints,parts);
	free(parts);
	free(x);
	free(y);
	return(vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
i2dvertex bPGisTable::read_point2D(unsigned char* wkb){
//_bTrace_("bPGisTable::read_point2D",false);
d2dvertex	vx;
i2dvertex	ivx;
int			zmflag,gk;
byte		ed;

// byteOrder 
	ed=popbyte(&wkb); 
// extract zmflag from type
	gk=popint(&wkb,ed!=getEndianByte());
	if(gk&WKBSRIDFLAG){
		skipint(&wkb);
	}
	zmflag=ZMFLAG(gk);
	vx.x=popdouble(&wkb,ed!=getEndianByte())-_ox;
	vx.y=popdouble(&wkb,ed!=getEndianByte())-_oy;
	vx_d2i(&ivx,&vx,_reso);
	return(ivx);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivx_rect bPGisTable::read_box2d(char* str){
//_bTrace_("bPGisTable::read_box2d",false);
ivx_rect	bx;
d2dvertex	dvx;
i2dvertex	ivx;

//xmin
char*		b1=strstr(str,"(");
	b1++;
char*		b2=strstr(b1," ");
char		b3[256];
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.x=atof(b3)-_ox;

//ymin
	b1=b2;
	b1++;
	b2=strstr(b1,",");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.y=atof(b3)-_oy;
	
	vx_d2i(&ivx,&dvx,_reso);
	bx.left=ivx.h;
	bx.bottom=ivx.v;

//xmax
	b1=b2;
	b1++;
	b2=strstr(b1," ");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.x=atof(b3)-_ox;
	
//ymax	
	b1=b2;
	b1++;
	b2=strstr(b1,")");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.y=atof(b3)-_oy;
	
	vx_d2i(&ivx,&dvx,_reso);	
	bx.right=ivx.h;
	bx.top=ivx.v;

	return(bx);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivx_cube bPGisTable::read_box3d(char* str){
//_bTrace_("bPGisTable::read_box3d",false);
ivx_cube	bx;
d2dvertex	dvx;
i2dvertex	ivx;

//xmin
char*		b1=strstr(str,"(");
	b1++;
char*		b2=strstr(b1," ");
char		b3[256];
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.x=atof(b3)-_ox;

//ymin
	b1=b2;
	b1++;
	b2=strstr(b1," ");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.y=atof(b3)-_oy;
	
	vx_d2i(&ivx,&dvx,_reso);
	bx.left=ivx.h;
	bx.bottom=ivx.v;

//zmin
	b1=b2;
	b1++;
	b2=strstr(b1,",");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	bx.min=round(atof(b3));

//xmax
	b1=b2;
	b1++;
	b2=strstr(b1," ");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.x=atof(b3)-_ox;


//ymax	
	b1=b2;
	b1++;
	b2=strstr(b1," ");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	dvx.y=atof(b3)-_oy;
	
	vx_d2i(&ivx,&dvx,_reso);
	bx.right=ivx.h;
	bx.top=ivx.v;

//zmax
	b1=b2;
	b1++;
	b2=strstr(b1,")");
	memmove(b3,b1,b2-b1);
	b3[b2-b1]=0;
	bx.max=round(atof(b3));

	return(bx);
}

// ---------------------------------------------------------------------------
// 
// -----------
ivertices* bPGisTable::make_vertices(	int nv, 
										double* x,
										double* y,
										int no,
										int* offs){
//_bTrace_("bPGisTable::make_vertices",false);
	if(no==1){
		no=0;
	}
ivertices*	vxs=ivs_new(_2D_VX,nv,no);
	if(!vxs){
//_err_(_strr_+"ivs_new failed for nv=%d and no=%d",nv,no);
		return(NULL);
	}
d2dvertex	vx;
	for(int i=0;i<no;i++){
		vxs->offs[i]=offs[i];
	}
	for(int i=0;i<nv;i++){
		vx.x=x[i]-_ox;
		vx.y=y[i]-_oy;
		vx_d2i(&vxs->vx.vx2[i],&vx,_reso);
	}
	return(vxs);
}


