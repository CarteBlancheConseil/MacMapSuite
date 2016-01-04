//----------------------------------------------------------------------------
// File : db_types.h
// Project : MacMapSuite
// Purpose : Header file : DB_Lib defines
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
// 25/05/01 creation.
//----------------------------------------------------------------------------

#ifndef __db_types__
#define __db_types__

//----------------------------------------------------------------------------

// Types simples
#define	_char		'CHAR'
#define	_bool		'BOOL'
#define	_int		'INT_'
#define	_double		'DBLE'
#define	_date		'DATE'
#define	_time		'TIME'

// Binaire
#define	_binary		'BIN_'

// Types composites entiers
#define	_ivx2		'iVX2'
#define	_ivx3		'iVX3'
#define	_ivxs2		'iVS2'
#define	_ivxs3		'iVS3'
#define	_ivxrect	'iVR2'
#define	_ivxcube	'iVR3'

// Types composites flottants
#define	_dvx2		'dVX2'
#define	_dvx3		'dVX3'
#define	_dvxs2		'dVS2'
#define	_dvxs3		'dVS3'
#define	_dvxrect	'dVR2'
#define	_dvxcube	'dVR3'

// Etat des enregistrements
#define	kDead_		1
#define	kLive_		0

// Container pour passage des binaries
typedef struct DB_cont{
	int			sz;			// taille
	void*		content;	// contenu
}DB_cont;

#define _kIntLen		16
#define _kDoubleLen		32
#define _kDateTimeLen	24

//----------------------------------------------------------------------------

#endif
