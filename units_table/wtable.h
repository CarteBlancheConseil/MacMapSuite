//----------------------------------------------------------------------------
// File : wtable.h
// Project : MacMapSuite
// Purpose : Header file : Table C wrappers
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
// 16/01/2006 creation.
//----------------------------------------------------------------------------


#ifndef __wtable__
#define __wtable__

//----------------------------------------------------------------------------

#include <MacMapSuite/db_types.h>
#include <MacMapSuite/table_def.h>

//----------------------------------------------------------------------------

#ifdef __cplusplus
#include <MacMapSuite/bStdTable.h>
typedef bStdTable*	wtable;
extern "C" {
#else
typedef void*		wtable;
#endif
	
//----------------------------------------------------------------------------

int		ext2sign				(	char* ext);

wtable	wtbl_alloc				(	int signature,
									const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status);

wtable	wtbl_falloc				(	int signature,
									const char* path,
									const char* name,
									bool create, 
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status);
									
void	wtbl_free				(	wtable tbl);

int		wtbl_KillRecord			(	wtable tbl, 
									int o);
									
int		wtbl_UnkillRecord		(	wtable tbl, 
									int o);
									
int		wtbl_RecordKilled		(	wtable tbl, 
									int o);
									
int		wtbl_CountRecords		(	wtable tbl);

int		wtbl_CountFields		(	wtable tbl);

int		wtbl_AddField			(	wtable tbl,
									const char* nam, 
									int sign, 
									int length, 
									int decs);
									
int		wtbl_RmvField			(	wtable tbl, 
									int f);
									
int		wtbl_ChgField			(	wtable tbl, 
									int f, 
									const char* nam, 
									int sign,
									int length, 
									int decs);
									
int		wtbl_FieldSign			(	wtable tbl, 
									int f, 
									int* sign);
									
int		wtbl_FieldName			(	wtable tbl, 
									int f, 
									char* name);
									
int		wtbl_FieldDecimals		(	wtable tbl, 
									int f, 
									int* decs);
									
int		wtbl_FieldLength		(	wtable tbl, 
									int f, 
									int* len);
									
int		wtbl_FieldSize			(	wtable tbl, 
									int f, 
									int* sz);
									
int		wtbl_ReadVal			(	wtable tbl, 
									int o, 
									int f,
									void* val);
									
int		wtbl_WriteVal			(	wtable tbl, 
									int o, 
									int f, 
									void* val);
									
int		wtbl_Pack				(	wtable tbl);

//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif


