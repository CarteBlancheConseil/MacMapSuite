//----------------------------------------------------------------------------
// File : mem_db.h
// Project : MacMapSuite
// Purpose : Header file : DB_Lib memory storage utils
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
// 18/08/05 creation.
//----------------------------------------------------------------------------

#ifndef _mem_db_
#define _mem_db_

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------

// Description d'un champ
typedef struct MEM_fld{
	int			sign;		// signature
	char		nam[64];	// nom
	int			length;		// longueur utilisateur
	int			decs;		// nb décimales (champs numériques)
	size_t		sz;			// taille réelle
	void*		data;		// données
}MEM_fld;

// Descripteur de la table
typedef struct MEM_info{
	long		nFld;		// nombre de champs
	long		nRec;		// nombre d'enregistrements
	MEM_fld		flds[];		// les champs	
}MEM_info;

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

MEM_info*	MEM_Create			(void);
void		MEM_Close			(MEM_info*	db);

int			MEM_NamedFieldIdx	(MEM_info*	db,	const char*	nam,	int*	iFld);

int			MEM_FieldSign		(MEM_info*	db,	int	iFld,	int*	sign);
int			MEM_FieldID			(MEM_info*	db,	int	iFld,	int*	id);
int			MEM_FieldName		(MEM_info*	db,	int	iFld,	char*	name);
int			MEM_FieldDecimals	(MEM_info*	db,	int	iFld,	int*	decs);
int			MEM_FieldSize		(MEM_info*	db,	int	iFld,	int*	sz);		
int			MEM_FieldLength		(MEM_info*	db,	int	iFld,	int*	len);		

int			MEM_CountFields		(MEM_info*	db);
int			MEM_CountRecords	(MEM_info*	db);

int			MEM_AddField		(MEM_info**	db,	const char*	nam, int sign, int length, int decs);
int			MEM_RmvField		(MEM_info**	db,	int iFld);
int			MEM_ChgField		(MEM_info*	db,	int iFld, const char* nam, int sign, int length, int decs);

int			MEM_ReadVal			(MEM_info*	db,	int iRec, int iFld, void* val);
int			MEM_WriteVal		(MEM_info*	db,	int iRec, int iFld, void* val);

int			MEM_ReadStringVal	(MEM_info*	db,	int iRec, int iFld, char* val);
int			MEM_WriteStringVal	(MEM_info*	db,	int iRec, int iFld, char* val);

int			MEM_AllocVal		(MEM_info*	db,	int iRec, int iFld, int*	sz,	void**	val);

int			MEM_KillRecord		(MEM_info*	db,	int iRec);
int			MEM_UnkillRecord	(MEM_info*	db,	int iRec);
int			MEM_GetRecordState	(MEM_info*	db,	int iRec);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif


