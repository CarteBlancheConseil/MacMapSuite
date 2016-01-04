//----------------------------------------------------------------------------
// File : valconv.h
// Project : MacMapSuite
// Purpose : Header file : Value conversion utils
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
// 28/01/04 creation.
//----------------------------------------------------------------------------

#ifndef __valconv__
#define __valconv__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

int xToChar		(int sign, int prm, void *val, char *conv);
int xToInt		(int sign, int prm, void *val, int *conv);
int xToDouble	(int sign, int prm, void *val, double *conv);
int xToDate		(int sign, int prm, void *val, double *conv);
int xToTime		(int sign, int prm, void *val, double *conv);

int charToX		(int sign, int prm, char *val, void *conv);
int intToX		(int sign, int prm, int val, void *conv);
int doubleToX	(int sign, int prm, double val, void *conv);
int dateToX		(int sign, int prm, double val, void *conv);
int timeToX		(int sign, int prm, double val, void *conv);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif


