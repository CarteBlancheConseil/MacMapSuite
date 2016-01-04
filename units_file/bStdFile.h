//----------------------------------------------------------------------------
// File : bStdFile.h
// Project : MacMapSuite
// Purpose : Header file : File class utils
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
// 07/02/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bStdFile__
#define __bStdFile__

//----------------------------------------------------------------------------

#include <stdio.h>

//----------------------------------------------------------------------------

class bFile{
	public:		
		bFile					(	);
		virtual ~bFile			(	);
		
		virtual int status		(	);
		
		virtual void mount		(	char** buffer, 
									int* sz);
		virtual bool next_line	(	char* buffer, 
									int len_max);
		virtual void write		(	const char* fmt);
		virtual void write		(	void* buffer,
									int sz);
		
	protected:
		int		_status;

	private:
		
};

//----------------------------------------------------------------------------

class bStdFile : public bFile{
	public:		
		bStdFile				(	const char* path,
									const char* perm);
		virtual ~bStdFile		(	);
				
		virtual void mount		(	char** buffer, 
									int* sz);
		virtual bool next_line	(	char* buffer, 
									int len_max);
		virtual void write		(	const char* fmt);
		virtual void write		(	void* buffer,
									int sz);
		
		virtual void change_name(	const char* name);
		virtual void copy		(	bStdFile& fin);
		
		virtual void erase		(	);

		virtual FILE* entity	(	);
		virtual void set_close	(	bool b);
		
	protected:
		virtual void check_path	(	const char* path);
		
	private:
		FILE*	_fp;
		char	_path[FILENAME_MAX];
		char	_perm[10];
		bool	_close_it;
		
};

//----------------------------------------------------------------------------

class bMemFile : public bFile{
	public:		
		bMemFile				(	);
		virtual ~bMemFile		(	);
				
		virtual void mount		(	char** buffer, 
									int* sz);
		virtual void write		(	const char* fmt);
		virtual void write		(	void* buffer,
									int sz);
		
	protected:

	private:
		char*	_buffer;
		int		_sz;
		char	_ln[1024];
};

//----------------------------------------------------------------------------

#endif
