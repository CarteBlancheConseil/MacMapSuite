//----------------------------------------------------------------------------
// File : wbase.h
// Project : MacMapSuite
// Purpose : Header file : Database C wrappers
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


#ifndef __wbase__
#define __wbase__

//----------------------------------------------------------------------------

#include <MacMapSuite/base_def.h>

//----------------------------------------------------------------------------

#ifdef __cplusplus
#include <MacMapSuite/bStdBaseAccessor.h>
typedef bStdBaseAccessor*	wbase;
typedef bool				wbool;
extern "C" {
#else
typedef void*				wbase;
typedef unsigned char		wbool;
#endif

//----------------------------------------------------------------------------

wbase	wbse_create					(	int signature,
										const char* hpath,
										const char* name,
										const char* data,
										int kind,
										double reso,
										double u2m,
										int srid,
										int* status);
									
wbase	wbse_open					(	const char* hpath, 
										int* status);
wbase	wbse_openws					(	const char* hpath,
										int wanted_srid,
										double wanted_reso,
										int* status);
									
void	wbse_free					(	wbase bse);

int		wbse_read					(	wbase bse, 
										int o,  
										int f,  
										void *val);

int		wbse_write					(	wbase bse, 
										int o,  
										int f,  
										void *val);
		
int		wbse_count					(	wbase bse);
		
int		wbse_add					(	wbase bse, 
										int kind,  
										int len,  
										int decs,  
										const char* name);

int		wbse_modify					(	wbase bse, 
										int f,  
										int kind,  
										int len,  
										int decs,  
										const char* name);

int		wbse_remove					(	wbase bse, 
										int f);

wbool	wbse_get_kind				(	wbase bse, 
										int f,  
										int* k);
										
wbool	wbse_get_len				(	wbase bse, 
										int f,  
										int* l);
										
wbool	wbse_get_decs				(	wbase bse, 
										int f,  
										int* d);
												
wbool	wbse_get_name				(	wbase bse, 
										int f,  
										char* name);
												
wbool	wbse_get_size				(	wbase bse, 
										int f, 
										int* sz);

int		wbse_get_id					(	wbase bse, 
										int f);

int		wbse_get_id_with_name		(	wbase bse, 
										const char* name);

int		wbse_get_index				(	wbase bse, 
										int fid);

int		wbse_get_index_with_name	(	wbase bse, 
										const char *fname);
		
wbool	wbse_is_delprotected		(	wbase bse, 
										int f);

wbool	wbse_set_delprotected		(	wbase bse, 
										int f, 
										wbool b);
		
wbool	wbse_is_writeprotected		(	wbase bse, 
										int f);

wbool	wbse_set_writeprotected		(	wbase bse, 
										int f, 
										wbool b);
		
wbool	wbse_is_hidden				(	wbase bse, 
										int f);

wbool	wbse_set_hidden				(	wbase bse, 
										int f,
										wbool b);

wbool	wbse_is_dyn					(	wbase bse, 
										int f);

wbool	wbse_is_dyn_ref				(	wbase bse, 
										int f);
		
int		wbse_count_constraints		(	wbase bse, 
										int f);

int		wbse_get_constraints_kind	(	wbase bse, 
										int f);

wbool	wbse_get_constraint			(	wbase bse, 
										int f,
										int n, 
										void *val);
										
wbool	wbse_set_constraint			(	wbase bse, 
										int f, 
										int n, 
										void *val);
										
wbool	wbse_add_constraint			(	wbase bse, 
										int f, 
										int kind, 
										void *val);
										
wbool	wbse_rmv_constraint			(	wbase bse, 
										int f, 
										int n);
										
int		wbse_get_constraint_index	(	wbase bse, 
										int f, 
										void *val);
										
wbool	wbse_set_constraint_index	(	wbase bse, 
										int f,
										int from, 
										int to);

wbool	wbse_has_default_value		(	wbase bse, 
										int f);

wbool	wbse_get_default_value		(	wbase bse, 
										int f,
										void *val);
										
wbool	wbse_set_default_value		(	wbase bse, 
										int f, 
										void *val);

int		wbse_count_records			(	wbase bse);

wbool	wbse_kill_record			(	wbase bse, 
										int r);

wbool	wbse_unkill_record			(	wbase bse, 
										int r);

wbool	wbse_killed_record			(	wbase bse, 
										int r);
		
int		wbse_h_read					(	wbase bse, 
										int o, 
										int f, 
										void *val);
										
int		wbse_h_write				(	wbase bse, 
										int o, 
										int f, 
										void *val);

int		wbse_count_param			(	wbase bse, 
										const char* sub);

wbool	wbse_get_param_name			(	wbase bse, 
										const char* sub,
										int idx,
										char* name);
										
wbool	wbse_get_param				(	wbase bse, 
										const char* sub, 
										const char* name, 
										void** p,
										int* sz);

wbool	wbse_set_param				(	wbase bse, 
										const char* sub, 
										const char* name, 
										void* p,
										int sz);

int		wbse_add_dyn				(	wbase bse, 
										const char* path,
										const char* name,
										int tbl_f,
										int bse_f);
									
int		wbse_pack					(	wbase bse);
	
//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif


