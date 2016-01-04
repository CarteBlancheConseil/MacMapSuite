//----------------------------------------------------------------------------
// File : bLocalOCDBTable.cpp
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
// 17/04/2009 creation.
//----------------------------------------------------------------------------

#include "bLocalOCDBTable.h"
#include "bTrace.h"
#include "bStdDirectory.h"
#include <unistd.h>

#pragma mark =>bLocalOCDBTable
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTable	::bLocalOCDBTable(	const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status,
									bool use_fu)
				:bStdTable(reso,0.0,0.0,srid,srid,status){
_bTrace_("bLocalOCDBTable::bLocalOCDBTable",false);

	_fd=(*status);
	*status=0;
	sprintf(_fpath,"%s%s",path,name);
	_use_fu=use_fu;
	_db=DB_OpenRW(_fpath,_use_fu);
	if(!_db){
		_db=DB_OpenRO(_fpath,_use_fu);
	}
	if(_db){
//_tm_("DB_Open : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
		db_close();
		return;
	}
	if(!create){
		*status=-1;
_te_(_fpath+" : table not found");
		return;
	}
	_db=DB_Create(_fpath,_use_fu);
	if(!_db){
_te_(_fpath+" : creation failed");
		*status=-1;
		return;
	}
//_tm_("DB_Create : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
	db_close();
	*status=0;
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTable	::bLocalOCDBTable(	const char* path,
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
_bTrace_("bLocalOCDBTable::bLocalOCDBTable",false);

	_fd=(*status);
	*status=0;
	sprintf(_fpath,"%s%s",path,name);
	_use_fu=use_fu;
	_db=DB_OpenRW(_fpath,_use_fu);
	if(!_db){
		_db=DB_OpenRO(_fpath,_use_fu);
	}
	if(_db){
//_tm_("DB_Open : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
		db_close();
		return;
	}
	if(!create){
		*status=-1;
_te_(_fpath+" : table not found");
		return;
	}
	_db=DB_Create(_fpath,_use_fu);
	if(!_db){
_te_(_fpath+" : creation failed");
		*status=-1;
		return;
	}
//_tm_("DB_Create : "+_db->ff->_file+"/"+(_db->fu?_db->fu->_file:-1));
	db_close();
	*status=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalOCDBTable::~bLocalOCDBTable(){
	db_close();
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::KillRecord(int	o){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_KillRecord(_db,o-1);
	db_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::UnkillRecord(int	o){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_UnkillRecord(_db,o-1);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::RecordKilled(int	o){
int status=db_open();
	if(status){
		return(0);
	}
	status=DB_GetRecordState(_db,o-1);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::CountRecords(){
int status=db_open();
	if(status){
		return(0);
	}
	status=DB_CountRecords(_db);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::CountFields(){
int status=db_open();
	if(status){
		return(0);
	}
	status=DB_CountFields(_db);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::AddField(const char *nam, int sign, int length, int decs){
int status=db_open();
	if(status){
		return(status);
	}
DB_info*	db=_db;
	status=DB_AddField(&db,nam,sign,length,decs);
	if(db){
		_db=db;
	}
	if(status>-1){
		status=0;
	}
	db_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::RmvField(int f){
int status=db_open();
	if(status){
		return(status);
	}
DB_info*	db=_db;
	status=DB_RmvField(&db,f-1);
	if(db){
		_db=db;
	}
	if(status>-1){
		status=0;
	}
	db_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::ChgField(int	f, const char	*nam, int sign, int length, int decs){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_ChgField(_db,f-1,nam,sign,length,decs);
	db_close();
	return(status);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::FieldSign(int	f,	int *sign){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_FieldSign(_db,f-1,sign);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::FieldName(int	f,	char *name){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_FieldName(_db,f-1,name);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::FieldDecimals(int f, int* decs){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_FieldDecimals(_db,f-1,decs);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::FieldLength(int f, int* len){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_FieldLength(_db,f-1,len);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::FieldSize(int f, int* sz){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_FieldSize(_db,f-1,sz);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::ReadVal(int	o,	int	f, void*	val){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_ReadVal(_db,o-1,f-1,val);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::WriteVal(int	o,	int	f, void*	val){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_WriteVal(_db,o-1,f-1,val);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::Pack(){
int status=db_open();
	if(status){
		return(status);
	}
	status=DB_Pack(_db);
	db_close();
	return(status);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::db_open(){
_bTrace_("bLocalOCDBTable::db_open",false);
bStdDirectory	root(_fd);
	if(root.status()){
_te_("root.status()="+root.status());
		return(-1);
	}

char cwd[PATH_MAX];
getcwd(cwd,PATH_MAX);
_tm_(cwd+"->"+_fpath);

	_db=DB_OpenRW(_fpath,_use_fu);
	if(!_db){
		_db=DB_OpenRO(_fpath,_use_fu);
	}
	if(!_db){
_te_("DB_OpenRx failed");
		return(-2);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTable::db_close(){
	if(_db){
		DB_Close(_db);
		_db=NULL;
	}
	return(0);
}

#pragma mark -
#pragma mark =>bLocalOCDBTableFU
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTableFU	::bLocalOCDBTableFU(const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status)
					:bLocalOCDBTable(path,name,create,reso,srid,status,true){
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTableFU	::bLocalOCDBTableFU(const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status)
					:bLocalOCDBTable(path,name,create,reso,ox,oy,tsrid,asrid,status,true){
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalOCDBTableFU::~bLocalOCDBTableFU(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTableFU::signature(){
//	return(kTableLocalOCDBFU);
return(0);
}

#pragma mark -
#pragma mark =>bLocalOCDBTableNoFU
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTableNoFU::bLocalOCDBTableNoFU(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status)
					:bLocalOCDBTable(path,name,create,reso,srid,status,false){
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bLocalOCDBTableNoFU::bLocalOCDBTableNoFU(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status)
				:bLocalOCDBTable(path,name,create,reso,ox,oy,tsrid,asrid,status,false){
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bLocalOCDBTableNoFU::~bLocalOCDBTableNoFU(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bLocalOCDBTableNoFU::signature(){
//	return(kTableLocalOCDBNoFU);
return(0);
}
