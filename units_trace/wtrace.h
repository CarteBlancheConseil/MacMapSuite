//----------------------------------------------------------------------------
// File : wtrace.h
// Project : MacMapSuite
// Purpose : Header file : Trace C wrapper
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
// C Wrapper for bTrace
//----------------------------------------------------------------------------
// 10/02/2006 creation.
//----------------------------------------------------------------------------

#ifndef __wtrace__
#define __wtrace__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif	
	
//----------------------------------------------------------------------------

typedef void* wtrace;

//----------------------------------------------------------------------------

wtrace	wtrc_alloc		(const char* msg, int echo);
void	wtrc_free		(wtrace trc);
void	wtrc_err		(wtrace trc, const char *fmt);
void	wtrc_msg		(wtrace trc, const char *fmt);
void	wtrc_set_on		(wtrace trc, int b);
int		wtrc_get_on		(wtrace trc);
void	wtrc_set_flush	(wtrace trc, int b);
int		wtrc_get_flush	(wtrace trc);

//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
