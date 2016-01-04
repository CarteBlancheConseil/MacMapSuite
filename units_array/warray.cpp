//----------------------------------------------------------------------------
// File : warray.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Array management C wrappers
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
// 11/01/2006 creation.
//----------------------------------------------------------------------------

#include "warray.h"
#include "bArray.h"

// ---------------------------------------------------------------------------
// 
// ------------
warray warr_alloc(int eltsz, int eltcount){
	return(new bArray(eltsz,eltcount));
}

// ---------------------------------------------------------------------------
// 
// ------------
warray warr_dup(warray arr){
	return(new bArray(*((bArray*)arr)));
}

// ---------------------------------------------------------------------------
// 
// ------------
void warr_free(warray arr){
	return(delete (bArray*)arr);
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_get(warray arr, int idx, void* elt){
bArray* barr=(bArray*)arr;
	return(barr->get(idx,elt));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_put(warray arr, int idx, void* elt){
bArray* barr=(bArray*)arr;
	return(barr->put(idx,elt));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_add(warray arr, void* elt){
bArray* barr=(bArray*)arr;
	return(barr->add(elt));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_rmv(warray arr, int idx){
bArray* barr=(bArray*)arr;
	return(barr->rmv(idx));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_push(warray arr, int idx, int offset){
bArray* barr=(bArray*)arr;
	return(barr->push(idx,offset));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_insert(warray arr, int idx, void* elt){
bArray* barr=(bArray*)arr;
	return(barr->insert(idx,elt));
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_count(warray arr){
bArray* barr=(bArray*)arr;
	return(barr->count());
}

// ---------------------------------------------------------------------------
// 
// ------------
void	warr_reset(warray arr){
bArray* barr=(bArray*)arr;
	barr->reset();
}

// ---------------------------------------------------------------------------
// 
// ------------
void	warr_sort(warray arr, int(*compare)(const void*,const void*)){
bArray* barr=(bArray*)arr;
	barr->sort(compare);
}

// ---------------------------------------------------------------------------
// 
// ------------
int	warr_search(warray arr, const void* elt, 
							int(*compare)(const void*,const void*)){
bArray* barr=(bArray*)arr;
	return(barr->search(elt,compare));
}

// ---------------------------------------------------------------------------
// 
// ------------
void warr_setsorted(warray arr, int sorted){
bArray* barr=(bArray*)arr;
	return(barr->setsorted(sorted));
}

// ---------------------------------------------------------------------------
// 
// ------------
int warr_cat(warray a, warray b){
bArray* ba=(bArray*)a;
bArray* bb=(bArray*)b;
	return((*ba)+(*bb));
}
