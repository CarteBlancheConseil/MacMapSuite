//----------------------------------------------------------------------------
// File : bArray.h
// Project : MacMapSuite
// Purpose : Header file : Array management class
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
// array utilities
//----------------------------------------------------------------------------
// 09/01/2004 creation.
//----------------------------------------------------------------------------

#ifndef __bArray__
#define __bArray__

//----------------------------------------------------------------------------

class bArray{
	public:
		bArray 					(	long eltsz);
		bArray 					(	long eltsz, 
									long eltcount);
		bArray 					(	bArray& in);
		
		virtual ~bArray			(	);
		
		virtual bool get		(	long idx, 
									void *elt);
		virtual bool put		(	long idx, 
									void *elt);
		
		virtual bool add		(	void *elt);
		virtual bool rmv		(	long idx);
		virtual bool push		(	long idx, 
									long offset);
		virtual bool insert		(	long idx, 
									void* elt);
		virtual bool addcomp	(	void* elt, 
									int(*compare)(const void*,const void*));
		virtual long count		(	);
		
		virtual void reset		(	);
		
		virtual void sort		(	int(*compare)(const void*,const void*));
		virtual long  search	(	const void* elt, 
									int(*compare)(const void*,const void*));
		
		virtual void setsorted	(	bool sorted);
		
		virtual bool operator + (	bArray&);
		
	protected:
		
	private:
		long 	_n;
		long 	_sz;
		void* 	_elts;
		void* 	_buffer;
		bool	_sorted;
		long	_nblk;
};

//----------------------------------------------------------------------------

#endif
