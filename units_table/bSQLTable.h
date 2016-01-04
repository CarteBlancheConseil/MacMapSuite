//----------------------------------------------------------------------------
// File : bSQLTable.h
// Project : MacMapSuite
// Purpose : Header file : SQL file table class
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
// 23/03/2007 creation.
//----------------------------------------------------------------------------

#ifndef __bSQLTable__
#define __bSQLTable__

//----------------------------------------------------------------------------

#include "bMemTable.h"

//----------------------------------------------------------------------------

class bSQLTable : public bMemTable{
	public:
		bSQLTable 				(	const char* path,
									const char* name,
									bool create, 
									double reso,
									int srid,
									int* status);
		bSQLTable				(	const char* path,
									const char* name,
									bool create, 
									double* reso,
									double* ox,
									double* oy,
									int* tsrid,
									int* asrid,
									int* status);
		virtual ~bSQLTable  	(	);

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
		virtual int WriteVal	(	int o, 
									int f, 
									void* val,
									bool dummy);
		virtual int dump_to_file(	);
				
	private:
		FILE*			_fp;
		char*			_name;
		bool			_update;
	
};

//----------------------------------------------------------------------------

#endif


