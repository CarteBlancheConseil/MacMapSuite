//----------------------------------------------------------------------------
// File : warray.h
// Project : MacMapSuite
// Purpose : Header file : Array management C wrappers
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
// C Wrapper for bArray
//----------------------------------------------------------------------------
// 11/01/2006 creation.
//----------------------------------------------------------------------------

#ifndef __warray__
#define __warray__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

typedef void* warray;

#define warr_allocempty(sz) warr_alloc(sz,0)

//----------------------------------------------------------------------------

warray	warr_alloc		(int eltsz, int eltcount);
warray	warr_dup		(warray arr);
void	warr_free		(warray arr);
int		warr_get		(warray arr, int idx, void* elt);
int		warr_put		(warray arr, int idx, void* elt);
int		warr_add		(warray arr, void* elt);
int		warr_rmv		(warray arr, int idx);
int		warr_push		(warray arr, int idx, int offset);
int		warr_insert		(warray arr, int idx, void* elt);
int		warr_count		(warray arr);
void	warr_reset		(warray arr);
void	warr_sort		(warray arr, int(*compare)(const void*,const void*));
int		warr_search		(warray arr, const void* elt, 
									 int(*compare)(const void*,const void*));
void	warr_setsorted	(warray arr, int sorted);
int		warr_cat		(warray a, warray b);

//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
