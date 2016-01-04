//----------------------------------------------------------------------------
// File : test
// Project : MacMapSuite
// Purpose : Header file : root header for C wrappers
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
// 16/01/2006 creation.
//----------------------------------------------------------------------------

#ifndef __MapSuiteC__
#define __MapSuiteC__

//----------------------------------------------------------------------------

#ifndef _vx_lib_
#include <MacMapSuite/vx_lib.h>
#endif

#ifndef _db_lib_
#include <MacMapSuite/db_lib.h>
#endif

#ifndef __C_UTILS__
#include <MacMapSuite/C_Utils.h>
#endif

#ifndef __warray__
#include <MacMapSuite/warray.h>
#endif

#ifndef __wtable__
#include <MacMapSuite/wtable.h>
#endif

#ifndef __wbase__
#include <MacMapSuite/wbase.h>
#endif

#ifndef __wproj__
#include <MacMapSuite/wproj.h>
#endif

#ifndef __wtrace__
#include <MacMapSuite/wtrace.h>
#endif

//----------------------------------------------------------------------------

extern int		_VERBOSE_;
extern int		_APP_WORLD_;
extern int		_USE_STDERR_;

enum{
	kAppWorldTool,
	kAppWorldApplication
};

//----------------------------------------------------------------------------

#endif
