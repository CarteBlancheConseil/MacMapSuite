//----------------------------------------------------------------------------
// File : bLocalDBTable.h
// Project : MacMapSuite
// Purpose : Header file : Local database table class
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
// 13/01/2004 creation.
//----------------------------------------------------------------------------

#ifndef __bLocalDBTable__
#define __bLocalDBTable__

//----------------------------------------------------------------------------

#include "bStdTable.h"
#include "db.h"

//----------------------------------------------------------------------------

class bLocalDBTable : public bStdTable{
	public:
		virtual ~bLocalDBTable  		(	);

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
				
	protected:
		bLocalDBTable 					(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status,
											bool use_fu);
		bLocalDBTable 					(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status,
											bool use_fu);
		
	private:
		DB_info*	_db;
};

//----------------------------------------------------------------------------

class bLocalDBTableFU : public bLocalDBTable{
	public:
		bLocalDBTableFU 				(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status);
		bLocalDBTableFU 				(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bLocalDBTableFU  		(	);
		virtual int signature			(	);
};

//----------------------------------------------------------------------------

class bLocalDBTableNoFU : public bLocalDBTable{
	public:
		bLocalDBTableNoFU 				(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status);
		bLocalDBTableNoFU 				(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bLocalDBTableNoFU  	(	);
		virtual int signature			(	);
};

//----------------------------------------------------------------------------

#endif


