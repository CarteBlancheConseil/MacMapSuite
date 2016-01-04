//----------------------------------------------------------------------------
// File : bv300BaseAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Database base class version 3.0.0
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
// 13/10/2006 creation.
//----------------------------------------------------------------------------

#ifndef __bv300BaseAccessor__
#define __bv300BaseAccessor__

//----------------------------------------------------------------------------

#include <MacMapSuite/bArray.h>
#include <MacMapSuite/bStdBaseAccessor.h>
#include <MacMapSuite/bStdTable.h>
#include <MacMapSuite/base_def.h>
#include "bv310BaseAccessor.h"

//----------------------------------------------------------------------------

class bv300BaseAccessor : public bStdBaseAccessor{
	public:
		bv300BaseAccessor 					(	const char* hpath, 
												int* status);
		bv300BaseAccessor 					(	const char* hpath, 
												int* status,
												int kind,
												int srid,
												double reso,
												double u2m,
												const char* name,
												const char* data,
												int hkind);
												
		virtual ~bv300BaseAccessor			(	);

		virtual int	read					(	int o, 
												int f, 
												void *val);
		virtual int	write					(	int o, 
												int f, 
												void *val);
		
		virtual int count					(	);
		
		virtual int	add						(	int kind, 
												int len, 
												int decs, 
												const char* name);
		virtual int	modify					(	int f, 
												int kind, 
												int len, 
												int decs, 
												const char* name);
		virtual int	remove					(	int f);

		virtual bool get_kind				(	int f, 
												int* k);
		
		virtual bool get_len				(	int f, 
												int* l);
		
		virtual bool get_decs				(	int f, 
												int* d);
		
		virtual bool get_name				(	int f, 
												char* name);
		
		virtual bool get_size				(	int f, 
												int* sz);

		virtual int	get_id					(	int f);
		virtual int	get_id					(	const char *name);

		virtual int	get_index				(	int fid);
		virtual int	get_index				(	const char *fname);
		
		virtual bool is_delprotected		(	int f);
		virtual bool set_delprotected		(	int f, 
												bool b);
		
		virtual bool is_writeprotected		(	int f);
		virtual bool set_writeprotected		(	int f, 
												bool b);
		
		virtual bool is_hidden				(	int f);
		virtual bool set_hidden				(	int f, 
												bool b);

		virtual bool is_dyn					(	int f);
		
		virtual int	count_constraints		(	int f);
		virtual int	get_constraints_kind	(	int f);
		virtual bool get_constraint			(	int f, 
												int n, 
												void *val);
		virtual bool set_constraint			(	int f, 
												int n, 
												void *val);
		virtual bool add_constraint			(	int f, 
												int kind, 
												void *val);
		virtual bool rmv_constraint			(	int f, 
												int n);
		virtual int get_constraint_index	(	int f, 
												void *val);
		virtual bool set_constraint_index	(	int f, 
												int from, 
												int to);

		virtual bool has_default_value		(	int f);
		virtual bool get_default_value		(	int f, 
												void *val);
		virtual bool set_default_value		(	int f,
												void *val);

		virtual int count_records			(	);
		virtual bool kill_record			(	int r);
		virtual bool unkill_record			(	int r);
		virtual bool killed_record			(	int r);
		
		virtual int	h_read					(	int o, 
												int f, 
												void *val);
		virtual int	h_write					(	int o, 
												int f, 
												void *val);
		
		virtual int count_param				(	const char* sub);
		virtual bool get_param_name			(	const char* sub, 
												int idx, 
												char* name);
		virtual bool get_param				(	const char* sub, 
												const char* name, 
												void** p, 
												int* sz);
		virtual bool set_param				(	const char* sub, 
												const char* name, 
												void* p, 
												int sz);
		
		virtual int signature				(	);
		
	protected:
		virtual void f_put_date				(	int f);
		virtual void o_put_date				(	int o);
		virtual int std_add					(	);
		virtual int obj_add					(	);
		virtual int report_add				(	int kind, 
												int len, 
												int decs, 
												const char* name);
		virtual int load					(	);
		virtual int load_fields				(	);
		virtual int load_constraints		(	);
		virtual int load_infos				(	);
		virtual void fld_write				(	int k, 
												int id,
												int kind,
												int size,
												int len,
												int decs,
												int state,
												const char* name,
												int cntKind,
												const char* def,
												const char* fmt,
												int index);
		virtual int	h_new					(	const char* name);
		virtual int	h_init					(	);
		virtual void read_name				(	const char* path, 
												char* name);
		virtual void read_data				(	char* dbname,
												char* host,
												int* port,
												char* user,
												char* password);
		virtual void read_data				(	char* dbname,
												char* host,
												int* port,
												char* user,
												char* password,
												const char* path);

		virtual int append					(	);
		
		virtual bool set_constraint			(	int f, 
												int n, 
												void *val, 
												bool check);
		virtual bool add_constraint			(	int f, 
												int kind, 
												void *val, 
												bool check);
		virtual bool rmv_constraint			(	int f, 
												int n, 
												bool check);
		virtual bool check_for_val			(	int f, 
												void *val);
		virtual bool clear_default_value	(	int f);

		virtual bool is_lock				(	);
		virtual int nb_live					(	);
		virtual bool make_package			(	int sign, 
												const char* path, 
												const char* name, 
												const char* data);


/*		virtual int fld_open				();
		virtual int cnt_open				();
		virtual void fld_close				();
		virtual void cnt_close				();*/

		bArray				_elts;
		bArray				_celts;
		
		bStdTable*			_fld;
		bStdTable*			_objs;
		bStdTable*			_hdr;
		bStdTable*			_cnst;
		
		int					_fid;
		int					_oid;		
		int					_kind;
		int					_srid;
		double				_reso;
		double				_u2m;

		int					_fd;
		
	private:
		static int constraint_comp			(	const void* a, 
												const void* b);
		void init_user						(	);
		
		static char*		_user;
		
};

//----------------------------------------------------------------------------

#endif


