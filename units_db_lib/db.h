//----------------------------------------------------------------------------
// File : db.h
// Project : MacMapSuite
// Purpose : Header file : DB_Lib (MacMap local database file format) allocation, deallocation, read, write
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
// 15/06/05 byte order support.
//----------------------------------------------------------------------------

#ifndef _db_
#define _db_

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------

#define _kDBCurrentVersion_	10000

//----------------------------------------------------------------------------

typedef void (*swapper)	(size_t,size_t,void*);

//----------------------------------------------------------------------------

// Description d'un champ
typedef struct DB_fld{
	int			sign;		// signature
	int			fid;		// ID (inutilisé actuellement)
	char		nam[64];	// nom
	int			attrs;		// attributs (inutilisé actuellement)
	int			length;		// longueur utilisateur
	int			decs;		// nb décimales (champs numériques)
	int			sz;			// taille réelle
	int			off;		// offset du champ
	int			reserved0;	// réservé
	int			reserved1;	// réservé
	int			reserved2;	// réservé
	int			reserved3;	// réservé
}DB_fld;

// Stockage des champs à largeur variable
typedef struct DB_usz{
	int			off;		// offset
	int			sz;			// taille
}DB_usz;

// Descripteur de la table
typedef struct DB_info{
	int			spy;		// témoin de crash (inutilisé actuellement)
	int			vers;		// version (inutilisé actuellement)
	int			endian;		// environnement dans lequel a été créé la base
	int			nFld;		// nombre de champs
	int			nRec;		// nombre d'enregistrements
	
	size_t		recSz;		// taille d'un enregistrement
	size_t		dtOff;		// début des enregistrements
	
	size_t		fPos;		// position fichier du buffer de lecture ff
	void*		fBuff;		// buffer de lecture ff
	size_t		uPos;		// position fichier du buffer de lecture fv
	void*		uBuff;		// buffer de lecture fv
	
	void*		emptyBuff;	// buffer d'init
	
	int			readOnly;	// base en lecture seule ou non
	
	size_t		rBuffSz;	// taille du buffer de lecture
	
	swapper		swproc;		// swapper big-endian <-> little-endian
	
	int			reserved2;	// réservé
	int			reserved3;	// réservé
	int			reserved4;	// réservé
	int			reserved5;	// réservé
	int			reserved6;	// réservé
	int			reserved7;	// réservé
	int			reserved8;	// réservé
	int			reserved9;	// réservé
	int			reserved10;	// réservé
	int			reserved11;	// réservé
	int			reserved12;	// réservé
	int			reserved13;	// réservé
	int			reserved14;	// réservé
	int			reserved15;	// réservé
	int			reserved16;	// réservé
	
	FILE*		ff;			// le fichier des champs à largeur fixe
	FILE*		fu;			// le fichier des champs à largeur variable
		
	DB_fld		flds[];		// les champs	
}DB_info;

// Descripteur de la table pour lecture/ecriture fichier 32bits
typedef struct DB_info32{
    int			spy;		// témoin de crash (inutilisé actuellement)
    int			vers;		// version (inutilisé actuellement)
    int			endian;		// environnement dans lequel a été créé la base
    int			nFld;		// nombre de champs
    int			nRec;		// nombre d'enregistrements
    
    int			recSz;		// taille d'un enregistrement
    int			dtOff;		// début des enregistrements
    
    int			fPos;		// position fichier du buffer de lecture ff
    int         fBuff;		// buffer de lecture ff
    int			uPos;		// position fichier du buffer de lecture fv
    int         uBuff;		// buffer de lecture fv
    
    int         emptyBuff;	// buffer d'init
    
    int			readOnly;	// base en lecture seule ou non
    
    int			rBuffSz;	// taille du buffer de lecture
    
    int         swproc;		// swapper big-endian <-> little-endian
    
    int			reserved2;	// réservé
    int			reserved3;	// réservé
    int			reserved4;	// réservé
    int			reserved5;	// réservé
    int			reserved6;	// réservé
    int			reserved7;	// réservé
    int			reserved8;	// réservé
    int			reserved9;	// réservé
    int			reserved10;	// réservé
    int			reserved11;	// réservé
    int			reserved12;	// réservé
    int			reserved13;	// réservé
    int			reserved14;	// réservé
    int			reserved15;	// réservé
    int			reserved16;	// réservé
    
    int         ff;			// le fichier des champs à largeur fixe
    int         fu;			// le fichier des champs à largeur variable
    
    DB_fld		flds[];		// les champs	
}DB_info32;


typedef DB_info* DB_infop;

enum{
	fuDontUse	=0,
	fuNeeded	=1,
	fuFit		=2
};

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

void		swapword			(	size_t sz, 
									void* word);

DB_info*	DB_Create			(	const char* fName,
									int use_fu);
DB_info*	DB_OpenRW			(	const char* fName,
									int use_fu);
DB_info*	DB_OpenRO			(	const char* fName,
									int use_fu);
void		DB_Close			(	DB_info* db);

int			DB_NamedFieldIdx	(	DB_info* db,
									const char* nam,
									int* iFld);

int			DB_FieldSign		(	DB_info* db,
									int iFld,
									int* sign);
int			DB_FieldName		(	DB_info* db,
									int	iFld,
									char* name);
int			DB_FieldDecimals	(	DB_info* db,
									int	iFld,
									int* decs);
int			DB_FieldSize		(	DB_info* db,
									int	iFld,
									int* sz);		
int			DB_FieldLength		(	DB_info* db,
									int	iFld,
									int* len);		

int			DB_CountFields		(	DB_info* db);
int			DB_CountRecords		(	DB_info* db);

int			DB_AddField			(	DB_info** db,
									const char* nam, 
									int sign, 
									int length,
									int decs);
int			DB_RmvField			(	DB_info** db,
									int iFld);
int			DB_ChgField			(	DB_info* db,
									int iFld, 
									const char* nam, 
									int sign, 
									int length, 
									int decs);

int			DB_ReadVal			(	DB_info* db,
									int iRec,
									int iFld,
									void* val);
int			DB_WriteVal			(	DB_info* db,
									int iRec,
									int iFld,
									void* val);

int			DB_ReadStringVal	(	DB_info* db,	
									int iRec,
									int iFld,
									char* val);
int			DB_WriteStringVal	(	DB_info* db,
									int iRec,
									int iFld,
									char* val);

int			DB_AllocVal			(	DB_info* db,
									int iRec, 
									int iFld, 
									int* sz, 
									void** val);

int			DB_KillRecord		(	DB_info* db,
									int iRec);
int			DB_UnkillRecord		(	DB_info* db,
									int iRec);
int			DB_GetRecordState	(	DB_info* db,
									int iRec);

int			DB_SetBufferSize	(	DB_info* db,
									int sz);

int			DB_Pack				(	DB_info* db);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#endif


