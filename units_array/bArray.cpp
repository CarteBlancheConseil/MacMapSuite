//----------------------------------------------------------------------------
// File : bArray.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Array management class
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
// 09/01/2004 creation.
//----------------------------------------------------------------------------

#include "bArray.h"
#include "bTrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
// 
// ------------
#define kBlockCount			4L
#define kBlockRemoveCoef	0.75

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bArray::bArray(long eltsz){
	_n=0;
	_nblk=0;
	_sz=eltsz;
	_elts=NULL;
	_sorted=false;
	_buffer=malloc(_sz);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bArray::bArray(long eltsz, long eltcount){
	_nblk=eltcount/kBlockCount;
	if(eltcount%kBlockCount){
		_nblk++;
	}
	_n=eltcount;
	_sz=eltsz;
	_sorted=false;
	_elts=malloc(_nblk*kBlockCount*_sz);
	if(!_elts){
		_n=0;
		_nblk=0;
		return;
	}
	memset(_elts,0,_nblk*kBlockCount*_sz);
	_buffer=malloc(_sz);
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bArray::bArray(bArray& in){
	_nblk=in._n/kBlockCount;
	if(in._n%kBlockCount){
		_nblk++;
	}
	_n=in._n;
	_sz=in._sz;
	_elts=malloc(_nblk*kBlockCount*_sz);
	_sorted=in._sorted;
	if(!_elts){
		_sorted=false;
		_n=0;
		return;
	}
	memset(_elts,0,_nblk*kBlockCount*_sz);
	memmove(_elts,in._elts,_n*_sz);
	_buffer=malloc(_sz);
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bArray::~bArray(){
	reset();
	free(_buffer);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::put(long idx, void *elt){
	if((idx<1)||(idx>_n)){
		return(false);
	}
	memmove((void*)(((long)_elts)+((idx-1)*_sz)),elt,_sz);
	if(_sorted){
		_sorted=false;
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::get(long idx, void *elt){
	if((idx<1)||(idx>_n)){
		return(false);
	}
	memmove(elt,(void*)(((long)_elts)+((idx-1)*_sz)),_sz);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::add(void *elt){
//bTrace	trc("bArray::add",false);
	if(_elts){
		if((_n%kBlockCount)==0){
void*		e=realloc(_elts,(_nblk+1)*kBlockCount*_sz);
			if(!e){
				return(false);
			}
			_elts=e;
			_nblk++;
		}
	}
	else{
		_nblk=1;
		_elts=malloc(_nblk*kBlockCount*_sz);
		if(!_elts){
			_nblk=0;
			return(false);
		}
	}
	_n++;
//trc.msg("%d->%d",_n,_nblk);
	if(_sorted){
		_sorted=false;
	}
	return(put(_n,elt));
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::rmv(long idx){
//bTrace	trc("bArray::rmv",false);
	if((idx<1)||(idx>_n)){
		return(false);
	}
	if(_n==1){
		free(_elts);
		_elts=NULL;
		_n=0;
		_nblk=0;
//trc.msg("%d->%d",_n,_nblk);
		return(true);
	}
	if(idx<_n){
void*	from=(void*)(((long)_elts)+(idx*_sz));
void*	to=(void*)(((long)_elts)+((idx-1)*_sz));
		memmove(to,from,(_n-idx)*_sz);
	}
	if/*(*/	(((_n-1)%kBlockCount)==0)							/*&&
		(((double)(_n-1)/(double)(_nblk*kBlockCount))<0.75)	)*/{
		_elts=realloc(_elts,(_nblk-1)*kBlockCount*_sz);
		_nblk--;
	}
	_n--;
//trc.msg("%d->%d",_n,_nblk);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::push(long idx, long offset){
	if((idx<1)||(idx>_n)){
		return(false);
	}
long newidx=idx+offset;
	if((newidx<1)||(newidx>_n)){
		return(false);
	}
	if(idx==newidx){
		return(true);
	}
	if(!_buffer){
		return(false);
	}
	
void*	from=	(void*)(((long)_elts)+(idx*_sz));
void*	to=		(void*)(((long)_elts)+((idx-1)*_sz));
	
	memmove(_buffer,to,_sz);
	memmove(to,from,(_n-idx)*_sz);
	
	from=(void*)(((long)_elts)+((newidx-1)*_sz));
	to=(void*)(((long)_elts)+(newidx*_sz));
	
	memmove(to,from,(_n-newidx)*_sz);
	memmove(from,_buffer,_sz);
		
	if(_sorted){
		_sorted=false;
	}
	
	return(true);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::insert(long idx, void* elt){
	if((idx<1)||(idx>_n)){
		return(false);
	}
	if(!add(elt)){
		return(false);
	}
	
void*	from=	(void*)(((long)_elts)+((idx-1)*_sz));
void*	to=		(void*)(((long)_elts)+((idx)*_sz));

	memmove(to,from,(_n-idx)*_sz);

	(void)put(idx,elt);
	return(true);
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::addcomp(void* elt, int(*compare)(const void*,const void*)){
_bTrace_("bArray::addcomp",true);
	if(_n==0){
		return(add(elt));
	}
void*	ref=_elts;
	if(compare(elt,ref)<=0){
_tm_("inf");
		return(insert(1,elt));
	}
	ref=(void*)(((long)_elts)+((_n-1)*_sz));
	if(compare(elt,ref)>=0){
_tm_("sup");
		return(add(elt));
	}
	return(false);
}

// ---------------------------------------------------------------------------
// 
// ------------
long bArray::count(){
	return(_n);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bArray::reset(){
	if(_elts){
		free(_elts);
	}
	_elts=NULL;
	_n=0;
	_nblk=0;
	_sorted=false;
}

// ---------------------------------------------------------------------------
// 
// ------------
void bArray::sort(int(*compare)(const void*,const void*)){
	qsort(_elts,_n,_sz,compare);
	_sorted=true;
}

// ---------------------------------------------------------------------------
// 
// ------------
long bArray::search(const void* elt,int(*compare)(const void*,const void*)){
	if(_sorted){
void *addr=bsearch(elt,_elts,_n,_sz,compare);
		if(addr==NULL){
			return(0);
		}
long	base=(long)_elts;
		base=((long)addr)-base;
		base/=_sz;
		return(base+1);
	}
	else{
long	i,idx=0;
		
		for(i=1;i<=_n;i++){
			if((get(i,_buffer))&&(!compare(elt,_buffer))){
				idx=i;
				break;
			}
		}
		return(idx);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bArray::setsorted(bool sorted){
	_sorted=sorted;
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bArray::operator + (bArray& b){
	if(_sz!=b._sz){
		return(false);
	}
	if(!b._elts){
		return(true);
	}
	if(_elts){
long	ne=_n+b._n;
long	nb=ne/kBlockCount;
		if(ne%kBlockCount){
			nb++;
		}
		if(nb!=_nblk){
void*		e=realloc(_elts,nb*kBlockCount*_sz);
			if(!e){
				return(false);
			}
			_nblk=nb;
			_elts=e;
		}
void*	to=(void*)(((long)_elts)+(_n*_sz));
		memmove(to,b._elts,b._n*_sz);
		_n+=b._n;
	}
	else{
		_nblk=b._n/kBlockCount;
		if(b._n%kBlockCount){
			_nblk++;
		}
		_elts=malloc(_nblk*kBlockCount*_sz);
		if(!_elts){
			_nblk=0;
			return(false);
		}
		memset(_elts,0,_nblk*kBlockCount*_sz);
		memmove(_elts,b._elts,b._n*_sz);
		_n=b._n;
	}
	return(true);
}
