//----------------------------------------------------------------------------
// File : bLocalDBTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Local database table class
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

#include "bLocalDBTable.h"
#include "bTrace.h"

#pragma mark =>bLocalDBTable
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTable	::bLocalDBTable(	const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status,
									bool use_fu)
				:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bLocalDBTable::bLocalDBTable",false);

char	fpath[1024];
	*status=0;
	sprintf(fpath,"%s%s",path,name);
	_db=DB_OpenRW(fpath,use_fu);
	if(!_db){
		_db=DB_OpenRO(fpath,use_fu);
	}
	if(_db){
//_tm_("DB_Open : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
		return;
	}
	if(!create){
		*status=-1;
_te_(fpath+" : table not found");
		return;
	}
	_db=DB_Create(fpath,use_fu);
	if(!_db){
_te_(fpath+" : creation failed");
		*status=-1;
		return;
	}
//_tm_("DB_Create : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
	*status=0;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTable	::bLocalDBTable(	const char* path,
									const char* name,
									bool create, 
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status,
									bool use_fu)
				:bStdTable(*reso,*ox,*oy,*tsrid,*asrid,status){
_bTrace_("bLocalDBTable::bLocalDBTable",false);
char	fpath[1024];
	*status=0;
	sprintf(fpath,"%s%s",path,name);
	_db=DB_OpenRW(fpath,use_fu);
	if(!_db){
		_db=DB_OpenRO(fpath,use_fu);
	}
	if(_db){
//_tm_("DB_Open : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
		return;
	}
	if(!create){
		*status=-1;
_te_(fpath+" : table not found");
		return;
	}
	_db=DB_Create(fpath,use_fu);
	if(!_db){
_te_(fpath+" : creation failed");
		*status=-1;
		return;
	}
//_tm_("DB_Create : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
	*status=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalDBTable::~bLocalDBTable(){
	if(_db){
		DB_Close(_db);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::KillRecord(int	o){
	return(DB_KillRecord(_db,o-1));	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::UnkillRecord(int	o){
	return(DB_UnkillRecord(_db,o-1));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::RecordKilled(int	o){
	return(DB_GetRecordState(_db,o-1));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::CountRecords(){
	return(DB_CountRecords(_db));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::CountFields(){
	return(DB_CountFields(_db));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::AddField(const char *nam, int sign, int length, int decs){
DB_info*	db=_db;
int			k=DB_AddField(&db,nam,sign,length,decs);
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
int	bLocalDBTable::RmvField(int f){
DB_info*	db=_db;
int			k=DB_RmvField(&db,f-1);
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
int	bLocalDBTable::ChgField(int	f, const char	*nam, int sign, int length, int decs){
	return(DB_ChgField(_db,f-1,nam,sign,length,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::FieldSign(int	f,	int *sign){
	return(DB_FieldSign(_db,f-1,sign));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::FieldName(int	f,	char *name){
	return(DB_FieldName(_db,f-1,name));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::FieldDecimals(int f, int* decs){
	return(DB_FieldDecimals(_db,f-1,decs));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::FieldLength(int f, int* len){
	return(DB_FieldLength(_db,f-1,len));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::FieldSize(int f, int* sz){
	return(DB_FieldSize(_db,f-1,sz));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::ReadVal(int	o,	int	f, void*	val){
	return(DB_ReadVal(_db,o-1,f-1,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::WriteVal(int	o,	int	f, void*	val){
	return(DB_WriteVal(_db,o-1,f-1,val));
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTable::Pack(){
	return(DB_Pack(_db));
}

#pragma mark -
#pragma mark =>bLocalDBTableFU
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTableFU	::bLocalDBTableFU(	const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status)
				:bLocalDBTable(path,name,create,reso,srid,status,true){
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTableFU	::bLocalDBTableFU(	const char* path,
									const char* name,
									bool create, 
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status)
				:bLocalDBTable(path,name,create,reso,ox,oy,tsrid,asrid,status,true){
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalDBTableFU::~bLocalDBTableFU(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTableFU::signature(){
	return(kTableLocalDBFU);
}

#pragma mark -
#pragma mark =>bLocalDBTableNoFU
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTableNoFU::bLocalDBTableNoFU(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status)
					:bLocalDBTable(path,name,create,reso,srid,status,false){
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalDBTableNoFU::bLocalDBTableNoFU(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status)
				:bLocalDBTable(path,name,create,reso,ox,oy,tsrid,asrid,status,false){
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalDBTableNoFU::~bLocalDBTableNoFU(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalDBTableNoFU::signature(){
	return(kTableLocalDBNoFU);
}
