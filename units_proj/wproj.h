//----------------------------------------------------------------------------
// File : wproj.h
// Project : MacMapSuite
// Purpose : Header file : bStdProj C wrappers
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
// C Wrapper for bProj
//----------------------------------------------------------------------------
// 10/02/2006 creation.
//----------------------------------------------------------------------------

#ifndef __wproj__
#define __wproj__

//----------------------------------------------------------------------------

#include <MacMapSuite/vx.h>
#include <MacMapSuite/wtable.h>

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

typedef void* wproj;

enum{
	kPRJWithParam	=-1,
	kPRJWGS84		='wg84',
	kPRJLambert93	='lb93'
};

//----------------------------------------------------------------------------

wproj	wprj_alloc		(	int sgn, 
							const char* prm);
void	wprj_free		(	wproj prj);
int		wprj_transform	(	wproj prj, 
							wproj src, 
							dvertices* vxs);
wtable	wprj_gettable	(	void);

//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
