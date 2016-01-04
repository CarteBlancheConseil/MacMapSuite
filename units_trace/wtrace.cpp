//----------------------------------------------------------------------------
// File : wtrace.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Trace C wrapper
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
// 10/02/2006 creation.
//----------------------------------------------------------------------------

#include "wtrace.h"
#include "bTrace.h"

// ---------------------------------------------------------------------------
// 
// ------------
wtrace	wtrc_alloc(const char* msg, int echo){
	return(new bTrace(msg,echo));
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtrc_free(wtrace trc){
	delete (bTrace*)trc;
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtrc_err(wtrace trc, const char *fmt){
bString str(fmt);
	((bTrace*)trc)->err(str);
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtrc_msg(wtrace trc, const char *fmt){
bString str(fmt);
	((bTrace*)trc)->msg(str);
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtrc_set_on(wtrace trc, int b){
	((bTrace*)trc)->set_on(b);
}

// ---------------------------------------------------------------------------
// 
// ------------
int wtrc_get_on(wtrace trc){
	return(((bTrace*)trc)->get_on());
}

// ---------------------------------------------------------------------------
// 
// ------------
void wtrc_set_flush(wtrace trc, int b){
	((bTrace*)trc)->set_flush(b);
}

// ---------------------------------------------------------------------------
// 
// ------------
int wtrc_get_flush(wtrace trc){
	return(((bTrace*)trc)->get_flush());
}

