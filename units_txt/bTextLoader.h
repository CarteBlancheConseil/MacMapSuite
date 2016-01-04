//----------------------------------------------------------------------------
// File : bTextLoader.h
// Project : MacMapSuite
// Purpose : Header file : Text file parser class
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

#ifndef __bTextLoader__
#define __bTextLoader__

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_types.h"

//----------------------------------------------------------------------------

typedef struct acf{
	int start;
	int len;
	int lmax;
	int	kind;
	int	kfirst;
}acf;

enum{
	kEOLUndef,
	kEOLUnix,
	kEOLWin,
	kEOLMac
};

enum{
	kStringUndef,
	kStringText=_char,
	kStringInt=_int,
	kStringDouble=_double
};

#define	bmax	1024

//----------------------------------------------------------------------------

class bTextLoader{
	public:		
		virtual ~bTextLoader			(	);
		
		virtual int kind_eol			(	);
		virtual int nb_lines			(	);
		virtual int nb_columns			(	);
		virtual bool has_header			(	);
		virtual int column_kind			(	int col);
		virtual int column_len			(	int col);

		virtual void iterate			(	void* up,
											int(*proc)
											(int,int,char*,void*));

	protected:
	
		bTextLoader						(	FILE* fp,
											bool lfh);

		virtual void hash				(	);
		virtual void analyse			(	);
		
		virtual int qualify				(	char* str);
		virtual char* get_text			(	int n);
		virtual char* get_line			(	);


		FILE*	_fp;
		int		_eol;
		int		_cols;
		int		_lines;
		bool	_hdr;
		bool	_lfh;

		acf*	_hsh;
		char*	_lb;
		int		_lbsz;
		char*	_sb;
		int		_sbsz;

	private:
		
};

//----------------------------------------------------------------------------

class bDelimitedTextLoader : public bTextLoader{
	public:		

	protected:
	
		bDelimitedTextLoader			(	FILE* f,
											bool lfh,
											char del);
		virtual ~bDelimitedTextLoader	(	);

		virtual void hash				(	);

	private:
		char	_del;
};

//----------------------------------------------------------------------------

class bTabTextLoader : public bDelimitedTextLoader{
	public:		
		bTabTextLoader			(	FILE* f,
									bool lfh);
		virtual ~bTabTextLoader	(	);

	protected:
	
	private:

};


//----------------------------------------------------------------------------

class bCSVTextLoader : public bDelimitedTextLoader{
	public:		
		bCSVTextLoader			(	FILE* f,
									bool lfh);
		virtual ~bCSVTextLoader	(	);

	protected:
	
	private:

};

//----------------------------------------------------------------------------

#endif
