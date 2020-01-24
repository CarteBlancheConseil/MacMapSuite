//----------------------------------------------------------------------------
// File : wtable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Table C wrappers
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
// 11/01/2006 creation.
//----------------------------------------------------------------------------


#include "wtable.h"
#include "mms_config.h"
#include "C_Utils.h"
//#include "bTrace.h"

#include "bLocalDBTable.h"
#include "bLocalOCDBTable.h"
#include "bMemTable.h"
#include "bSHPTable.h"
#include "bDBFTable.h"
#include "bMITABTable.h"
#include "bPGisTable.h"
#include "bTextTable.h"
#include "bSQLTable.h"

// ---------------------------------------------------------------------------
// 
// ------------
int ext2sign(char* ext){
char	lext[64];
	strcpy(lext,ext);
	strupper(lext);
	if(!strcmp(lext,"DBF")){
		return(kTableDBF);
	}
	else if(!strcmp(lext,"TXT")){
		return(kTableTabText);
	}
	else if(!strcmp(lext,"CSV")){
		return(kTableCSVText);
	}
	else if(!strcmp(lext,"MMT")){
		return(kTableMMText);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// ------------
wtable wtbl_alloc(	int signature,
					const char* path,
					const char* name,
					bool create, 
					double reso,
					int srid,
					int* status){
//_bTrace_("wtbl_alloc",true);
bStdTable*	tbl=NULL;
	switch(signature){
		case kTableLocalDBFU:
            //_tm_("kTableLocalDBFU");
			tbl=new bLocalDBTableFU(path,name,create,reso,srid,status);
			break;
		case kTableLocalDBNoFU:
            //_tm_("kTableLocalDBNoFU");
			tbl=new bLocalDBTableNoFU(path,name,create,reso,srid,status);
			break;
		case kTableMemDB:
            //_tm_("kTableMemDB");
			tbl=new bMemTable(path,name,create,reso,srid,status);
			break;
		case kTableShape:
            //_tm_("kTableShape");
			tbl=new bSHPTable(path,name,create,reso,srid,status);
			break;
		case kTableDBF:
            //_tm_("kTableDBF");
			tbl=new bDBFTable(path,name,create,reso,srid,status);
			break;
		case kTableMITAB:
            //_tm_("kTableMITAB");
			tbl=new bMITABTable(path,name,create,reso,srid,status);
			break;
		case kTablePostGIS:
            //_tm_("kTablePostGIS");
			tbl=new bPGisTable(path,name,create,reso,srid,status);
			break;
		case kTableTabText:
            //_tm_("kTableTabText");
			tbl=new bTextTabTable(path,name,create,reso,srid,status);
			break;		
		case kTableCSVText:
            //_tm_("kTableCSVText");
			tbl=new bTextCSVTable(path,name,create,reso,srid,status);
			break;		
		case kTableSQL:
            //_tm_("kTableSQL");
			tbl=new bSQLTable(path,name,create,reso,srid,status);
			break;		
		case kTableDXF:
            //_tm_("kTableDXF");
			tbl=new bTextDXFTable(path,name,create,reso,srid,status);
			break;		
		case kTableMMText:
            //_tm_("kTableMMText");
			tbl=new bTextMMTable(path,name,create,reso,srid,status);
			break;		
		default:
//_te_("unknown signature "+signature);
			*status=-1;
			break;
	}
	return(tbl);
}

// ---------------------------------------------------------------------------
// 
// ------------
wtable wtbl_falloc(	int signature,
					const char* path,
					const char* name,
					bool create, 
					double *reso,
					double* ox,
					double* oy,
					int *tsrid,
					int *asrid,
					int* status){
//_bTrace_("wtbl_falloc",true);
bStdTable*	tbl=NULL;
	switch(signature){
		case kTableLocalDBFU:
			tbl=new bLocalDBTableFU(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableLocalDBNoFU:
			tbl=new bLocalDBTableNoFU(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableMemDB:
			tbl=new bMemTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableShape:
			tbl=new bSHPTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableDBF:
			tbl=new bDBFTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableMITAB:
			tbl=new bMITABTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTablePostGIS:
			tbl=new bPGisTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;
		case kTableTabText:
			tbl=new bTextTabTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;		
		case kTableCSVText:
			tbl=new bTextCSVTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;		
		case kTableSQL:
			tbl=new bSQLTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;		
		case kTableDXF:
			tbl=new bTextDXFTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;		
		case kTableMMText:
			tbl=new bTextMMTable(path,name,create,reso,ox,oy,tsrid,asrid,status);
			break;		
		default:
			*status=-1;
			break;
	}
	return(tbl);
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtbl_free(wtable tbl){
	delete (bStdTable*)tbl;
}

// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_KillRecord(	wtable tbl, 
						int o){
	return(((bStdTable*)tbl)->KillRecord(o));
}
	
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_UnkillRecord(	wtable tbl, 
						int o){
	return(((bStdTable*)tbl)->UnkillRecord(o));
}									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_RecordKilled(	wtable tbl, 
						int o){
	return(((bStdTable*)tbl)->RecordKilled(o));
}	
																
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_CountRecords(	wtable tbl){
	return(((bStdTable*)tbl)->CountRecords());
}	

// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_CountFields(	wtable tbl){
	return(((bStdTable*)tbl)->CountFields());
}	

// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_AddField(	wtable tbl,
					const char* nam, 
					int sign, 
					int length, 
					int decs){
	return(((bStdTable*)tbl)->AddField(nam,sign,length,decs));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_RmvField(	wtable tbl, 
					int f){
	return(((bStdTable*)tbl)->RmvField(f));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_ChgField(	wtable tbl, 
					int f, 
					const char* nam, 
					int sign,
					int length, 
					int decs){
	return(((bStdTable*)tbl)->ChgField(f,nam,sign,length,decs));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_FieldSign(	wtable tbl, 
					int f, 
					int* sign){
	return(((bStdTable*)tbl)->FieldSign(f,sign));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_FieldName(	wtable tbl, 
					int f, 
					char* name){
	return(((bStdTable*)tbl)->FieldName(f,name));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_FieldDecimals(	wtable tbl, 
						int f, 
						int* decs){
	return(((bStdTable*)tbl)->FieldDecimals(f,decs));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_FieldLength(	wtable tbl, 
						int f, 
						int* len){
	return(((bStdTable*)tbl)->FieldLength(f,len));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_FieldSize(	wtable tbl, 
					int f, 
					int* sz){
	return(((bStdTable*)tbl)->FieldSize(f,sz));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_ReadVal(	wtable tbl, 
					int o, 
					int f,
					void* val){
	return(((bStdTable*)tbl)->ReadVal(o,f,val));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_WriteVal(	wtable tbl, 
					int o, 
					int f, 
					void* val){
	return(((bStdTable*)tbl)->WriteVal(o,f,val));
}	
									
// ---------------------------------------------------------------------------
// 
// ------------
int wtbl_Pack(	wtable tbl){
	return(((bStdTable*)tbl)->Pack());
}	
