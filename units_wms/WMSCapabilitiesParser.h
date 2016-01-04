//----------------------------------------------------------------------------
// File : WMSCapabilitiesParser.h
// Project : MacMapSuite
// Purpose : Header file : WMS capabilities parser
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
// 06/07/2009 creation.
//----------------------------------------------------------------------------

#ifndef __WMSCapabilitiesParser__
#define __WMSCapabilitiesParser__

//----------------------------------------------------------------------------

#include <Carbon/Carbon.h>
#include <MacMapSuite/bArray.h>

//----------------------------------------------------------------------------

typedef struct wmslayerdesc{
	char*	name;
	char*	title;
	bArray*	srs;
	int		level;
}wmslayerdesc;

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

int		WMSCapabilitiesParser		(	void* data, 
										size_t sz, 
										bArray &arr);
								
void	WMSCapabilitiesFreeDescs	(	bArray &arr);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif
