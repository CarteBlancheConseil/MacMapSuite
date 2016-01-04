//----------------------------------------------------------------------------
// File : bDBFTable.h
// Project : MacMapSuite
// Purpose : Header file : DBF table class (ShapeLib C++ wrapper)
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
// 21/11/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bDBFTable__
#define __bDBFTable__

//----------------------------------------------------------------------------

#include "bStdTable.h"
#include "shapefil.h"

//----------------------------------------------------------------------------

class bDBFTable : public bStdTable{
	public:
		bDBFTable						(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status);
		bDBFTable						(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bDBFTable				(	);

		virtual int KillRecord			(	int o);
		virtual int UnkillRecord		(	int o);
		virtual int RecordKilled		(	int o);

		virtual int CountRecords		(	);
		virtual int CountFields			(	);
		virtual int AddField			(	const char* nam, 
											int sign, 
											int length, 
											int decs);
		virtual int RmvField			(	int f);
		virtual int ChgField			(	int f, 
											const char* nam, 
											int sign, 
											int length, 
											int decs);
		virtual int FieldSign			(	int f, 
											int* sign);
		virtual int FieldName			(	int f, 
											char* name);
		virtual int FieldDecimals		(	int f, 
											int* decs);
		virtual int FieldLength			(	int f, 
											int* len);
		virtual int FieldSize			(	int f, 
											int* sz);

		virtual int ReadVal				(	int o, 
											int f, 
											void* val);
		virtual int WriteVal			(	int o, 
											int f, 
											void* val);
		
		virtual int Pack				(	);
		
		virtual int signature			(	);

		
	protected:
		virtual int db2dbf			(	int sign);
		virtual int dbf2db			(	int sign);
		
	private:
		DBFHandle	_dbf;
};

//----------------------------------------------------------------------------

#endif


