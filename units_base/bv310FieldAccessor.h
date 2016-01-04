//----------------------------------------------------------------------------
// File : bv310FieldAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Database base field accessor class version 3.1.0
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
// 22/10/2007 creation.
//----------------------------------------------------------------------------

#ifndef __bv310FieldAccessor__
#define __bv310FieldAccessor__

//----------------------------------------------------------------------------

#include <stdio.h>
#include <MacMapSuite/bArray.h>
#include <MacMapSuite/bStdTable.h>
#include <MacMapSuite/base_def.h>

// ---------------------------------------------------------------------------
// 
// ------------
typedef struct tbl_index{
	int		o;
	void*	val;
}tbl_index;

//----------------------------------------------------------------------------

class bv310FieldAccessor{
	public:
		bv310FieldAccessor 					(	bStdTable* tbl,
												const char* name,
												int tid,
												int rfield,
												int off);
												
		virtual ~bv310FieldAccessor			(	);
		
		virtual const char* get_name		(	);
		virtual int get_id					(	);
		virtual int get_ref					(	);
		virtual int get_offset				(	);
		virtual void remove					(	);
		
		virtual int	read					(	int o, 
												int f, 
												void *val,
												void* prm,
												int(*rd_proc)(void*,int,int,void*));
				
	protected:
		bStdTable*	_tbl;
		char		_name[FILENAME_MAX];
		int			_tid;
		int			_rfield;
		int			_off;
		
	private:

		
};

//----------------------------------------------------------------------------

class bv310StaFieldAccessor : public bv310FieldAccessor{
	public:
		bv310StaFieldAccessor 				(	bStdTable* tbl,
												int off);
												
		virtual ~bv310StaFieldAccessor		(	);

		virtual int	read					(	int o, 
												int f, 
												void *val,
												void* prm,
												int(*rd_proc)(void*,int,int,void*));
				
	protected:
		
	private:

		
};

//----------------------------------------------------------------------------

class bv310DynFieldAccessor : public bv310FieldAccessor{
	public:
		bv310DynFieldAccessor 				(	bStdTable* tbl,
												const char* name,
												int tid,
												int rfield,
												int off,
												int tfield,
												int rsign,
												int rsize);
												
		virtual ~bv310DynFieldAccessor		(	);

		virtual int	read					(	int o, 
												int f, 
												void *val,
												void* prm,
												int(*rd_proc)(void*,int,int,void*));
				
	protected:
		bArray		_field_index;
		tbl_index	_buff;

		int			_tfield;
		int			_tsign;
		int			_tprm;
	
		int			_rsign;
		int			_rsize;
			
		int			(*_comp)(const void*,const void*);
		
	private:

		
};

//----------------------------------------------------------------------------

#endif


