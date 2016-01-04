//----------------------------------------------------------------------------
// File : bLocalOCDBTable.h
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
// 17/04/2009 creation.
//----------------------------------------------------------------------------

#ifndef __bLocalOCDBTable__
#define __bLocalOCDBTable__

//----------------------------------------------------------------------------

#include "bStdTable.h"
#include "db.h"

//----------------------------------------------------------------------------

class bLocalOCDBTable : public bStdTable{
	public:
		virtual ~bLocalOCDBTable  		(	);

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
		bLocalOCDBTable 				(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status,
											bool use_fu);
		bLocalOCDBTable 				(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status,
											bool use_fu);
		
		int db_open						(	);
		int db_close					(	);

	private:
		DB_info*	_db;
		char		_fpath[PATH_MAX];
		bool		_use_fu;
		int			_fd;
};

//----------------------------------------------------------------------------

class bLocalOCDBTableFU : public bLocalOCDBTable{
	public:
		bLocalOCDBTableFU 				(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status);
		bLocalOCDBTableFU 				(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bLocalOCDBTableFU  	(	);
		virtual int signature			(	);

};

//----------------------------------------------------------------------------

class bLocalOCDBTableNoFU : public bLocalOCDBTable{
	public:
		bLocalOCDBTableNoFU 			(	const char* path,
											const char* name,
											bool create, 
											double reso,
											int srid,
											int* status);
		bLocalOCDBTableNoFU 			(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bLocalOCDBTableNoFU  	(	);
		virtual int signature			(	);

};

//----------------------------------------------------------------------------

#endif


