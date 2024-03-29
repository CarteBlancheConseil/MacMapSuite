//----------------------------------------------------------------------------
// File : bStdBaseAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Database base class
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
// 04/07/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bStdBaseAccessor__
#define __bStdBaseAccessor__

//----------------------------------------------------------------------------

#include <MacMapSuite/bArray.h>
#include <MacMapSuite/bStdTable.h>
#include <MacMapSuite/base_def.h>

//----------------------------------------------------------------------------

class bStdBaseAccessor{
public:
    bStdBaseAccessor 					(	);												
    virtual ~bStdBaseAccessor			(	);

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
    
    virtual bool get_dummy				(	int f, 
                                            char* fmt);
    virtual bool set_dummy				(	int f, 
                                            char* fmt);

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
    virtual bool is_dyn_ref				(	int f);
    
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
    
    virtual int add_dyn					(	const char* path,
                                            const char* name,
                                            int tbl_f,
                                            int bse_f);
    
    virtual int signature				(	);
    virtual int version					(	);
    virtual int pack					(	){return 0;};
    
protected:
    int	_vers;

private:
    
};

//----------------------------------------------------------------------------

#endif


