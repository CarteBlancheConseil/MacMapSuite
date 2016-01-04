
//----------------------------------------------------------------------------
// File : bMemTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Memory table class
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
// 13/01/2004 creation.
//----------------------------------------------------------------------------

#include "bMemTable.h"
#include "bTrace.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bMemTable	::bMemTable(	const char* path,
							const char* name,
							bool create, 
							double reso,
							int srid,
							int* status)
			:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bMemTable::bMemTable",false);
int	bck_stat=*status;
	_db=MEM_Create();
	if(!_db){
_te_("creation failed");
		*status=-1;
		return;
	}
	*status=bck_stat;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bMemTable	::bMemTable(	const char* path,
							const char* name,
							bool create, 
							double* reso,
							double* ox,
							double* oy,
							int* tsrid,
							int* asrid,
							int* status)
			:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status){
_bTrace_("bMemTable::bMemTable",false);
int	bck_stat=*status;
	_db=MEM_Create();
	if(!_db){
_te_("creation failed");
		*status=-1;
		return;
	}
	*status=bck_stat;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bMemTable::~bMemTable(){
	if(_db){
		MEM_Close(_db);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::KillRecord(int o){
	return(MEM_KillRecord(_db,o-1));	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::UnkillRecord(int	o){
	return(MEM_UnkillRecord(_db,o-1));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::RecordKilled(int	o){
	return(MEM_GetRecordState(_db,o-1));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::CountRecords(){
	return(MEM_CountRecords(_db));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::CountFields(){
	return(MEM_CountFields(_db));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::AddField(const char *nam, int sign, int length, int decs){
MEM_info*	db=_db;
int			k=MEM_AddField(&db,nam,sign,length,decs);
	if(db){
		_db=db;
	}
	if(k>-1){
		k=0;
	}
	return(k);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::RmvField(int f){
MEM_info*	db=_db;
int			k=MEM_RmvField(&db,f-1);
	if(db){
		_db=db;
	}
	if(k>-1){
		k=0;
	}
	return(k);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::ChgField(int	f, const char	*nam, int sign, int length, int decs){
	return(MEM_ChgField(_db,f-1,nam,sign,length,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::FieldSign(int	f,	int *sign){
	return(MEM_FieldSign(_db,f-1,sign));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::FieldName(int	f,	char *name){
	return(MEM_FieldName(_db,f-1,name));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::FieldDecimals(int f, int* decs){
	return(MEM_FieldDecimals(_db,f-1,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::FieldLength(int f, int* len){
	return(MEM_FieldSize(_db,f-1,len));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::FieldSize(int f, int* sz){
	return(MEM_FieldSize(_db,f-1,sz));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::ReadVal(int	o,	int	f, void*	val){
	return(MEM_ReadVal(_db,o-1,f-1,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::WriteVal(int o, int f, void* val){
	return(MEM_WriteVal(_db,o-1,f-1,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bMemTable::signature(){
	return(kTableMemDB);
}
