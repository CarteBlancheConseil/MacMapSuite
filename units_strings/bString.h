//----------------------------------------------------------------------------
// File : bString.h
// Project : MacMapSuite
// Purpose : Header file : String utily class
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
// 27/05/2006 creation.
//----------------------------------------------------------------------------

#ifndef __bString__
#define __bString__

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//----------------------------------------------------------------------------

class bString{
	public:		
		bString						(	const char* str);
		virtual ~bString			(	);

		virtual const char* string	(	){return((const char*)_str);};
		virtual bString& reset		(	);
		
		virtual bString& operator +	(	const char* str);
		virtual bString& operator +	(	char x);
		virtual bString& operator +	(	int x);
		virtual bString& operator +	(	int* x);
		virtual bString& operator +	(	unsigned int x);
		virtual bString& operator +	(	unsigned int* x);
		virtual bString& operator +	(	double x);
		virtual bString& operator +	(	float x);
		virtual bString& operator +	(	void* x);
		virtual bString& operator +	(	long x);
		virtual bString& operator +	(	bString& x);

		
	protected:
		virtual void cat			(	const char* str);
		
	private:
		char*	_str;
};

//----------------------------------------------------------------------------

#endif
