//----------------------------------------------------------------------------
// File : bTextTable.h
// Project : MacMapSuite
// Purpose : Header file : Text file table class
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
// 29/06/2006 creation.
//----------------------------------------------------------------------------

#ifndef __bTextTable__
#define __bTextTable__

//----------------------------------------------------------------------------

#include "bMemTable.h"
#include "bTextLoader.h"
#include "vx_lib.h"
#include "bArray.h"

//----------------------------------------------------------------------------

class bTextTable : public bMemTable{
	public:
		virtual ~bTextTable  	(	);

		virtual int AddField	(	const char* nam, 
									int sign, 
									int length, 
									int decs);
		virtual int ChgField	(	int f, 
									const char* nam, 
									int sign, 
									int length, 
									int decs);
		virtual int WriteVal	(	int o, 
									int f, 
									void* val);		
	protected:
		bTextTable 				(	const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status);
		bTextTable				(	const char* path,
									const char* name,
									bool create, 
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status);
		virtual int WriteVal	(	int o, 
									int f, 
									void* val,
									bool dummy);
		virtual int dump_to_file(	);
		virtual int load		(	);
		static int dump_to_mem	(	int l,
									int c,
									char* val,
									void* up);
		
									
		FILE*			_fp;
		bTextLoader*	_loader;
		bool			_update;
		
	private:
	
};

//----------------------------------------------------------------------------

class bTextDelimitedTable : public bTextTable{
	public:
		bTextDelimitedTable 			(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status,
											char del,
											bool encap);
		bTextDelimitedTable				(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status,
											char del,
											bool encap);
		virtual ~bTextDelimitedTable  	(	);
				
	protected:
		virtual int dump_to_file		(	);

		char	_del;
		bool	_encap;

	private:
};

//----------------------------------------------------------------------------

class bTextTabTable : public bTextDelimitedTable{
	public:
		bTextTabTable 				(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status);
		bTextTabTable				(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status);
		virtual ~bTextTabTable  	(	);
		
		virtual int signature		(	);
		
	protected:
		virtual int load			(	);
		
	private:

};

//----------------------------------------------------------------------------

class bTextCSVTable : public bTextDelimitedTable{
	public:
		bTextCSVTable 				(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status);
		bTextCSVTable				(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status);
		virtual ~bTextCSVTable  	(	);
		
		virtual int signature		(	);
		
	protected:
		virtual int load			(	);
		
	private:

};

//----------------------------------------------------------------------------

class bTextDXFTable : public bTextTable{
	public:
		bTextDXFTable				(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status);
		bTextDXFTable				(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status);
		virtual ~bTextDXFTable		(	);

		virtual int signature		(	);
		
		dvertices* get_ageom		(	ivertices* vxs);
		ivertices* get_tgeom		(	dvertices* vxs);

	protected:
		virtual int dump_to_file	(	);
		virtual int load			(	);
		
	private:

};

//----------------------------------------------------------------------------

class bTextMMTable : public bTextDelimitedTable{
	public:
		bTextMMTable				(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status);
		bTextMMTable				(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status);
		virtual ~bTextMMTable		(	);

		virtual int AddField		(	const char* nam, 
										int sign, 
										int length, 
										int decs);
		virtual int ChgField		(	int f, 
										const char* nam, 
										int sign, 
										int length, 
										int decs);

		virtual int signature		(	);
				
	protected:
		virtual int dump_to_file	(	);
		virtual void dump_vxs		(	ivertices* vxs);
		virtual ivertices* load_vxs	(	char* str);
		virtual int load			(	);
		static int dump_to_mem		(	int l,
										int c,
										char* val,
										void* up);
		
	private:
		int		_gk;
		bArray*	_tmp;
};

//----------------------------------------------------------------------------

#endif


