//----------------------------------------------------------------------------
// File : table_def.h
// Project : MacMapSuite
// Purpose : Header file : Table defines
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
// 06/02/2006 creation.
//----------------------------------------------------------------------------

#ifndef __table_def__
#define __table_def__

//----------------------------------------------------------------------------

enum{
	kTableLocalDBFU		='tLDB',
	kTableLocalDBNoFU	='tLDn',

	kTableLocalDB		=kTableLocalDBFU,
	
	kTableMemDB			='tMEM',
	kTableShape			='tSHP',
	kTableDBF			='tDBF',
	kTableMITAB			='tMIT',
	kTablePostGIS		='tPGi',
	kTableTabText		='tTTx',
	kTableCSVText		='tTCs',
	kTableSQL			='tSQL',
	kTableDXF			='tDXF',
	kTableMMText		='tTMM'
};

typedef struct field_desc{
	int		kind;
	int		len;
	int		decs; 
	char	name[256];
	int		index;
}field_desc;

//----------------------------------------------------------------------------

#endif


