//----------------------------------------------------------------------------
// File : bDBFTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : DBF table class (ShapeLib C++ wrapper)
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
// 21/11/2005 creation.
//----------------------------------------------------------------------------

#include "bDBFTable.h"
#include "bTrace.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bDBFTable	::bDBFTable(	const char* path,
							const char* name,
							bool create,
							double reso,
							int srid,
							int* status)
			:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bDBFTable::bDBFTable",false);

char	fpath[1024];
	
    *status=0;

    sprintf(fpath,"%s%s",path,name);
    
    if(create){
        _dbf=DBFCreate(fpath);
        if(!_dbf){
_te_("creation failed for : "+fpath);
            *status=-1;
            return;
        }
        return;
    }
    
	_dbf=DBFOpen(fpath,"rb+");
	if(!_dbf){
		_dbf=DBFOpen(fpath,"rb");	
	}
	if(_dbf){
		return;
	}

_te_("table not found : "+fpath);
    *status=-1;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bDBFTable	::bDBFTable(	const char* path,
							const char* name,
							bool create, 
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
				:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status){
_bTrace_("bDBFTable::bDBFTable",false);

char	fpath[1024];
	
    *status=0;

    sprintf(fpath,"%s%s",path,name);
        
    if(create){
        _dbf=DBFCreate(fpath);
        if(!_dbf){
_te_("creation failed for : "+fpath);
            *status=-1;
            return;
        }
        return;
    }
        
    _dbf=DBFOpen(fpath,"rb+");
    if(!_dbf){
        _dbf=DBFOpen(fpath,"rb");
    }
    if(_dbf){
        return;
    }

_te_("table not found : "+fpath);
    *status=-1;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bDBFTable::~bDBFTable(){
	if(_dbf){
		DBFClose(_dbf);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::KillRecord(int	o){
//	return(DB_KillRecord(_dbf,o-1));
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::UnkillRecord(int	o){
//	return(DB_UnkillRecord(_dbf,o-1));
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::RecordKilled(int	o){
//	return(DB_GetRecordState(_dbf,o-1));
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::CountRecords(){
	return(DBFGetRecordCount(_dbf));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::CountFields(){
	return(DBFGetFieldCount(_dbf));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::AddField(const char* nam, int sign, int length, int decs){
	if((sign!=_double)&&(decs!=0)){
		decs=0;
	}
	else if((sign==_double)&&(decs==0)){
		decs=2;
	}
	
	if(length>255){
		length=255;
	}

DBFFieldType	dbsgn=(DBFFieldType)db2dbf(sign);
	if(dbsgn==FTInvalid){
		return(-1);
	}
	if(DBFAddField(_dbf,nam,dbsgn,length,decs)==-1){
		return(-2);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::RmvField(int f){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::ChgField(int	f, const char	*nam, int sign, int length, int decs){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::FieldSign(int f, int* sign){
char	dbfnam[256];
int		dbflen;
int		dbfdecs;
int		dbfsgn=DBFGetFieldInfo(_dbf,f-1,dbfnam,&dbflen,&dbfdecs); 
	*sign=dbf2db(dbfsgn);
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::FieldName(int f, char* name){
int		dbflen;
int		dbfdecs;
int		dbfsgn=DBFGetFieldInfo(_dbf,f-1,name,&dbflen,&dbfdecs); 
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::FieldDecimals(int f, int* decs){
char	dbfnam[256];
int		dbflen;
int		dbfsgn=DBFGetFieldInfo(_dbf,f-1,dbfnam,&dbflen,decs); 
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::FieldLength(int f, int* len){
char	dbfnam[256];
int		dbfdecs;
int		dbfsgn=DBFGetFieldInfo(_dbf,f-1,dbfnam,len,&dbfdecs); 
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::FieldSize(int f, int* sz){
char	dbfnam[256];
int		dbflen;
int		dbfdecs;
int		dbfsgn=DBFGetFieldInfo(_dbf,f-1,dbfnam,&dbflen,&dbfdecs); 
	
	switch(dbfsgn){
		case FTString:
			*sz=dbflen+1;
			break;
		case FTInteger:
			*sz=sizeof(int);
			break;
		case FTDouble:
			*sz=sizeof(double);
			break;
		default:
			*sz=0;
			break;
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::ReadVal(int o, int f, void* val){
int	sgn;
	(void)FieldSign(f,&sgn);
	switch(sgn){
		case _int:
			(*(int*)val)=DBFReadIntegerAttribute(_dbf,o-1,f-1);
			break;
		case _double:
			(*(double*)val)=DBFReadDoubleAttribute(_dbf,o-1,f-1);
			break;
		case _char:
			strcpy((char*)val,DBFReadStringAttribute(_dbf,o-1,f-1));
			break;
		default:
			return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::WriteVal(int o, int f, void* val){
_bTrace_("bDBFTable::WriteVal",false);
	if(o==CountRecords()+1){
		o=CountRecords();
	}
	else{
		o--;
	}
int	sgn;
	(void)FieldSign(f,&sgn);
	f--;
	switch(sgn){
		case _int:
			if(!DBFWriteIntegerAttribute(_dbf,o,f,(*(int*)val))){
//_te_("DBFWriteIntegerAttribute "+f);
				return(-2);
			}
			break;
		case _double:
			if(!DBFWriteDoubleAttribute(_dbf,o,f,(*(double*)val))){
//_te_("DBFWriteDoubleAttribute "+f);
				return(-2);
			}
			break;
		case _char:
			if(!DBFWriteStringAttribute(_dbf,o,f,(char*)val)){
//_te_("DBFWriteStringAttribute "+f);
				return(-2);
			}
			break;
		default:
_te_("bad kind");
			return(-1);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::signature(){
	return(kTableDBF);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::db2dbf(int sign){
int r=0;
	switch(sign){
		case _bool:
		case _int:
			r=FTInteger;
			break;
		case _double:
		case _date:
		case _time:
			r=FTDouble;
			break;
		case _char:
			r=FTString;
			break;
		default:
			r=FTInvalid;
			break;
	}
	return(r);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bDBFTable::dbf2db(int sign){
int r=0;
	switch(sign){
		case FTString:
			r=_char;
			break;
		case FTInteger:
			r=_int;
			break;
		case FTDouble:
			r=_double;
			break;
		default:
			break;
	}
	return(r);
}


