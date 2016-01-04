//----------------------------------------------------------------------------
// File : C_Utils.h
// Project : MacMapSuite
// Purpose : Header file : C utils
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
// 01/01/2002 creation.
//----------------------------------------------------------------------------

#ifndef __C_UTILS__
#define __C_UTILS__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

int		mtoupper		(	int c);
int		mtolower		(	int c);
void	strupperfcw		(	char* str);
void	strupperfcs		(	char* str);
void	strupper		(	char* str);
void	strlower		(	char* str);
void	strinsert		(	char* str, 
							const char* ins, 
							int k);
void	strrep			(	char* str, 
							const char* pat, 
							const char* rp);
int		strfloat		(	char* str, 
							int sign);
int		strint			(	char* str, 
							int sign);
char*	strcatr			(	char* a, 
							char* b);
void	relpath			(	char* base, 
							char* abs, 
							char* rel);
void    splitPath       (   const char* fullPath,
                            char* path,
                            char* name);
double	matof			(	char* str);

//----------------------------------------------------------------------------
								
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
