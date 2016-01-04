//----------------------------------------------------------------------------
// File : bMITABTable.h
// Project : MacMapSuite
// Purpose : Header file : MITAB table class (MITAB C++ wrapper)
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
// 24/05/2006 creation.
//----------------------------------------------------------------------------

#ifndef __bMITABTable__
#define __bMITABTable__

//----------------------------------------------------------------------------

#include "bStdTable.h"
#include "mitab_capi.h"
#include "vx.h"
#include "bArray.h"

//----------------------------------------------------------------------------

class bMITABTable : public bStdTable{
	public:
		bMITABTable 				(	const char* path,
										const char* name,
										bool create, 
										double reso,
										int srid,
										int* status);
		bMITABTable					(	const char* path,
										const char* name,
										bool create, 
										double* reso,
										double* ox,
										double* oy,
										int* tsrid,
										int* asrid,
										int* status);
		virtual ~bMITABTable  		(	);

		virtual int KillRecord		(	int o);
		virtual int UnkillRecord	(	int o);
		virtual int RecordKilled	(	int o);

		virtual int CountRecords	(	);
		virtual int CountFields		(	);
		virtual int AddField		(	const char* nam, 
										int sign, 
										int length, 
										int decs);
		virtual int RmvField		(	int f);
		virtual int ChgField		(	int f, 
										const char* nam, 
										int sign, 
										int length, 
										int decs);
		virtual int FieldSign		(	int f, 
										int* sign);
		virtual int FieldName		(	int f, 
										char* name);
		virtual int FieldDecimals	(	int f, 
										int* decs);
		virtual int FieldLength		(	int f, 
										int* len);
		virtual int FieldSize		(	int f, 
										int* sz);

		virtual int ReadVal			(	int o, 
										int f, 
										void* val);
		virtual int WriteVal		(	int o, 
										int f, 
										void* val);
		
		virtual int Pack			(	);
		
		virtual int signature		(	);
		
	protected:
		ivertices* feat2vxs			(	mitab_feature o, 
										int* kind);
		mitab_feature vxs2feat		(	ivertices* vxs);

		virtual int	db2mitab		(	int sign);

	private:
		
		mitab_handle			_mt;
		mitab_feature			_feat;
		int						_cur;
		bArray					_arr;
		int						_ptype;
		bool					_new;
};

//----------------------------------------------------------------------------

#endif


