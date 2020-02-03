//----------------------------------------------------------------------------
// File : db.c
// Project : MacMapSuite
// Purpose : C source file : DB_Lib (MacMap local database file format) allocation, deallocation, read, write
// Author : Benoit Ogier, benoit.ogier@macmap.com
//
// Copyright (C) 1997-2017 Carte Blanche Conseil.
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
// 25/05/2001 creation.
// 15/06/2005 byte order support.
//----------------------------------------------------------------------------

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "vx.h"
#include "vx_allocation.h"
#include "vx_utils.h"

#include "db.h"
#include "db_types.h"
#include "db_utils.h"

#include "valconv.h"

//----------------------------------------------------------------------------

#define	fcpybufsz_	1048576

//----------------------------------------------------------------------------
#pragma mark =>Debug
//----------------------------------------------------------------------------
//
//------------
static void printFLD(DB_fld fld){
    fprintf(stderr,"FLD sign : %d\n",fld.sign);
    fprintf(stderr,"FLD fid : %d\n",fld.fid);
    fprintf(stderr,"FLD nam : %s\n",fld.nam);
    fprintf(stderr,"FLD attrs : %d\n",fld.attrs);
    fprintf(stderr,"FLD length : %d\n",fld.length);
    fprintf(stderr,"FLD decs : %d\n",fld.decs);
    fprintf(stderr,"FLD sz : %d\n",fld.sz);
    fprintf(stderr,"FLD off : %d\n",fld.off);
}

//----------------------------------------------------------------------------
//
//------------
static void printDB(DB_info db){
    fprintf(stderr,"DB spy : %d\n",db.spy);
    fprintf(stderr,"DB vers : %d\n",db.vers);
    fprintf(stderr,"DB endian : %d\n",db.endian);
    fprintf(stderr,"DB nFld : %d\n",db.nFld);
    fprintf(stderr,"DB nRec : %d\n",db.nRec);
    fprintf(stderr,"DB recSz : %lu\n",db.recSz);
    fprintf(stderr,"DB dtOff : %lu\n",db.dtOff);
    fprintf(stderr,"DB fPos : %lu\n",db.fPos);
    fprintf(stderr,"DB uPos : %lu\n",db.uPos);
    fprintf(stderr,"DB readOnly : %d\n",db.readOnly);
    fprintf(stderr,"DB rBuffSz : %lu\n",db.rBuffSz);
}

//----------------------------------------------------------------------------
//
//------------
static void printDB32(DB_info32 db){
    fprintf(stderr,"DB32 spy : %d\n",db.spy);
    fprintf(stderr,"DB32 vers : %d\n",db.vers);
    fprintf(stderr,"DB32 endian : %d\n",db.endian);
    fprintf(stderr,"DB32 nFld : %d\n",db.nFld);
    fprintf(stderr,"DB32 nRec : %d\n",db.nRec);
    fprintf(stderr,"DB32 recSz : %d\n",db.recSz);
    fprintf(stderr,"DB32 dtOff : %d\n",db.dtOff);
    fprintf(stderr,"DB32 fPos : %d\n",db.fPos);
    fprintf(stderr,"DB32 uPos : %d\n",db.uPos);
    fprintf(stderr,"DB32 readOnly : %d\n",db.readOnly);
    fprintf(stderr,"DB32 rBuffSz : %d\n",db.rBuffSz);
}


//----------------------------------------------------------------------------
#pragma mark =>Utils
//----------------------------------------------------------------------------
// Utilities
//----------------------------------------------------------------------------
// File duplication
//------------
static int filecpy(FILE *dst, FILE *src){
long    i;
size_t  szf,szb,pos;
void	*buff;

	buff=malloc(fcpybufsz_);
	if(!buff){
fprintf(stderr,"filecpy::ERROR : malloc failed for %d\n",fcpybufsz_);
		return(-1);
	}

	fflush(src);

// FSEEK A MODIFIER
	fseek(src,0,SEEK_END);
// FTELL A MODIFIER
	szf=ftell(src);
	
	szb=fcpybufsz_;
		
	if(szb>szf){
		szb=szf;
	}
	
	i=0;
	do{
// FSEEK A MODIFIER
		fseek(src,i,SEEK_SET);
		fread(buff,szb,1,src);
		
// FSEEK A MODIFIER
		fseek(dst,i,SEEK_SET);
		fwrite(buff,szb,1,dst);
		
		i+=szb;
		
// FTELL A MODIFIER
		pos=ftell(src);
		
		if(szb>(szf-pos)){
			szb=szf-pos;
		}
fprintf(stderr,"filecpy::%ld processed (%.0f\%%)\n",i,(double)i/szf*100.0);
	}
	while(pos<szf);
	
// On repasse le fichier destination à szf, 
// pour le cas ou l'on a de l'espace disque à récupérer
	ftruncate(dst->_file,szf);
	fflush(dst);
	
	return(0);
}

//----------------------------------------------------------------------------
// Fields offset shift
//------------
static void pushOffsets(DB_info* db, int from, int sz){
	for(long i=from;i<db->nFld;i++){
		db->flds[i].off+=sz;
	}
}

//----------------------------------------------------------------------------
// Conversion to text
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur
// variable ne peut pas être converti
//------------
static void convertToString(void* v, DB_fld* f, char* s){
	switch(f->sign){
		case _bool:
		case _int:
			sprintf(s,"%d",(*((int*)v)));
			break;
		case _double:
			sprintf(s,"%.*f",f->decs,(*((double*)v)));
			break;
		case _char:
			sprintf(s,"%s",(char*)v);
			break;
			
		default:
			strcpy(s,"NOT CONVERTED");
			break;
	}
}

//----------------------------------------------------------------------------
// Conversion from text
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur
// variable ne peut pas être converti
//------------
static void convertFromString(char* s, DB_fld* f, void* v){
	switch(f->sign){
		case _bool:
		case _int:
			(*((int*)v))=strtol(s,NULL,10);
			break;
		case _double:
			(*((double*)v))=strtod(s,NULL);
			break;
		case _char:
			sprintf((char*)v,"%s",s);
			break;
			
		default:
			s[0]=0;
			break;
	}
}

//----------------------------------------------------------------------------
#pragma mark -
#pragma mark =>Endian Swap Procs
//----------------------------------------------------------------------------
// Swap procs
//----------------------------------------------------------------------------
// endian swap proc
//------------
void	swapword(size_t sz, void* word){
unsigned char	buff;

	for(size_t i=0;i<sz/2;i++){
		buff=((unsigned char*)word)[i];
		((unsigned char*)word)[i]=((unsigned char*)word)[sz-i-1];
		((unsigned char*)word)[sz-i-1]=buff;
	}
}

//----------------------------------------------------------------------------
// endian swap proc
//------------
static void	swap(size_t sz, size_t n, void* arr){
    for(size_t i=0;i<n;i++){
		swapword(sz,arr+i*sz);
    }
}

//----------------------------------------------------------------------------
// no swap proc 
//------------
static void	dontswap(size_t sz, size_t n, void* arr){
}

//----------------------------------------------------------------------------
// Header swap utilities
//------------
//----------------------------------------------------------------------------
//
//------------
/*static void	DB_CheckEndian(DB_info* db){
	if(db->endian!=1){
//fprintf(stderr,"ENDIANNESS : Table need swap proc\n");
		db->swproc=swap;
		swapword(sizeof(int),&db->spy);
		swapword(sizeof(int),&db->vers);
		swapword(sizeof(int),&db->nFld);
		swapword(sizeof(int),&db->nRec);
		swapword(sizeof(int),&db->recSz);
		swapword(sizeof(int),&db->dtOff);
		swapword(sizeof(int),&db->fPos);
		swapword(sizeof(int),&db->uPos);
		swapword(sizeof(int),&db->readOnly);
		swapword(sizeof(int),&db->rBuffSz);
	}
	else{
//fprintf(stderr,"ENDIANNESS : Table don't need swap proc\n");
		db->swproc=dontswap;
	}
}*/

//----------------------------------------------------------------------------
//
//------------
static void DB32_CheckEndian(DB_info32* db32, DB_info* db){
    if(db32->endian!=1){
fprintf(stderr,"ENDIANNESS : Table need swap proc\n");
        swapword(sizeof(int),&db32->spy);
        swapword(sizeof(int),&db32->vers);
        swapword(sizeof(int),&db32->nFld);
        swapword(sizeof(int),&db32->nRec);
        swapword(sizeof(int),&db32->recSz);
        swapword(sizeof(int),&db32->dtOff);
        swapword(sizeof(int),&db32->fPos);
        swapword(sizeof(int),&db32->uPos);
        swapword(sizeof(int),&db32->readOnly);
        swapword(sizeof(int),&db32->rBuffSz);
        
        db->swproc=swap;
    }
    else{
fprintf(stderr,"ENDIANNESS : Table don't need swap proc\n");
        db->swproc=dontswap;
    }
    
    db->spy=db32->spy;
    db->vers=db32->vers;
    db->nFld=db32->nFld;
    db->nRec=db32->nRec;
    db->recSz=db32->recSz;
    db->dtOff=db32->dtOff;
    db->fPos=db32->fPos;
    db->uPos=db32->uPos;
    db->readOnly=db32->readOnly;
    db->rBuffSz=db32->rBuffSz;

    db->endian=db32->endian;
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_CheckEndianFields(DB_info* db){
long	i;
	for(i=0;i<db->nFld;i++){
//fprintf(stderr,"DB_CheckEndianFields %d\n",i);
//fflush(stderr);
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].sign));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].fid));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].attrs));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].length));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].decs));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].sz));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].off));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].reserved0));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].reserved1));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].reserved2));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].reserved3));
	}
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_Swap32(DB_info32* db, int nFld, swapper swproc){
// Pas le endian, pour garder la trace intacte
	(*(swproc))(sizeof(int),1,&db->spy);
	(*(swproc))(sizeof(int),1,&db->vers);
	(*(swproc))(sizeof(int),1,&db->nFld);
	(*(swproc))(sizeof(int),1,&db->nRec);
	(*(swproc))(sizeof(int),1,&db->recSz);
	(*(swproc))(sizeof(int),1,&db->dtOff);
	(*(swproc))(sizeof(int),1,&db->fPos);
	(*(swproc))(sizeof(int),1,&db->uPos);
	(*(swproc))(sizeof(int),1,&db->readOnly);
	(*(swproc))(sizeof(int),1,&db->rBuffSz);
}

//----------------------------------------------------------------------------
//
//------------
static void    DB_SwapFields(DB_fld* flds, int nFld, swapper swproc){
long    i;
    
    for(i=0;i<nFld;i++){
        (*(swproc))(sizeof(int),1,&flds[i].sign);
        (*(swproc))(sizeof(int),1,&flds[i].fid);
        (*(swproc))(sizeof(int),1,&flds[i].attrs);
        (*(swproc))(sizeof(int),1,&flds[i].length);
        (*(swproc))(sizeof(int),1,&flds[i].decs);
        (*(swproc))(sizeof(int),1,&flds[i].sz);
        (*(swproc))(sizeof(int),1,&flds[i].off);
        (*(swproc))(sizeof(int),1,&flds[i].reserved0);
        (*(swproc))(sizeof(int),1,&flds[i].reserved1);
        (*(swproc))(sizeof(int),1,&flds[i].reserved2);
        (*(swproc))(sizeof(int),1,&flds[i].reserved3);
    }

}

//----------------------------------------------------------------------------
// Data swap utilities
//----------------------------------------------------------------------------
//
//------------
static void	DB_Swap_ivx(DB_info* db, ivertices* vxs){
	(*(db->swproc))(sizeof(int),3,vxs);
	switch(vxs->sign){
		case _2D_VX:
			(*(db->swproc))(sizeof(int),vxs->nv*2L,vxs->vx.vx2);
			if(vxs->no>0){
				(*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx2[vxs->nv]);
			}
			break;
		case _3D_VX:
			(*(db->swproc))(sizeof(int),vxs->nv*3L,vxs->vx.vx3);
			if(vxs->no>0){
				(*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx3[vxs->nv]);
			}
			break;
	}
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_Swap_ivx32(DB_info* db, ivertices32* vxs){
    (*(db->swproc))(sizeof(int),3,vxs);
    switch(vxs->sign){
        case _2D_VX:
            (*(db->swproc))(sizeof(int),vxs->nv*2L,vxs->vx.vx2);
            if(vxs->no>0){
                (*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx2[vxs->nv]);
            }
            break;
        case _3D_VX:
            (*(db->swproc))(sizeof(int),vxs->nv*3L,vxs->vx.vx3);
            if(vxs->no>0){
                (*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx3[vxs->nv]);
            }
            break;
    }
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_SwapAndBack_ivx(DB_info* db, ivertices* vxs, int sign, int nv, int no){
	(*(db->swproc))(sizeof(int),3,vxs);
	switch(sign){
		case _2D_VX:
			(*(db->swproc))(sizeof(int),nv*2L,vxs->vx.vx2);
			if(no>0){
				(*(db->swproc))(sizeof(int),no,vxs->offs);
			}
			break;
		case _3D_VX:
			(*(db->swproc))(sizeof(int),nv*3L,vxs->vx.vx3);
			if(no>0){
				(*(db->swproc))(sizeof(int),no,vxs->offs);
			}
			break;
	}
}

//----------------------------------------------------------------------------
// 
//------------
static void	DB_Swap_dvx(DB_info* db, dvertices* vxs){
	(*(db->swproc))(sizeof(int),3,vxs);
	switch(vxs->sign){
		case _2D_VX:
			(*(db->swproc))(sizeof(double),vxs->nv*2L,vxs->vx.vx2);
			if(vxs->no>0){
				(*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx2[vxs->nv]);
			}
			break;
		case _3D_VX:
			(*(db->swproc))(sizeof(double),vxs->nv*3L,vxs->vx.vx3);
			if(vxs->no>0){
				(*(db->swproc))(sizeof(int),vxs->no,&vxs->vx.vx3[vxs->nv]);
			}
			break;
	}
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_SwapAndBack_dvx(DB_info* db, dvertices* vxs, int sign, int nv, int no){
	(*(db->swproc))(sizeof(int),3,vxs);
	switch(sign){
		case _2D_VX:
			(*(db->swproc))(sizeof(double),nv*2L,vxs->vx.vx2);
			if(no>0){
				(*(db->swproc))(sizeof(int),no,vxs->offs);
			}
			break;
		case _3D_VX:
			(*(db->swproc))(sizeof(double),nv*3L,vxs->vx.vx3);
			if(no>0){
				(*(db->swproc))(sizeof(int),no,vxs->offs);
			}
			break;
	}
}

//----------------------------------------------------------------------------
// IO utilities
//----------------------------------------------------------------------------
#pragma mark -
#pragma mark =>IO utilities
//----------------------------------------------------------------------------
// no endian needed
//------------
static int DB_writeHeader(DB_info* db){
DB_info32   db32;
FILE*       ff=db->ff;
size_t      bkn=db->nFld;
    
    fseek(db->ff,0,SEEK_SET);

    db32.spy=db->spy;
    db32.vers=db->vers;
    db32.endian=db->endian;
    db32.nFld=db->nFld;
    db32.nRec=db->nRec;
    db32.recSz=db->recSz;
    db32.dtOff=db->dtOff;
    db32.fPos=db->fPos;
    db32.fBuff=0;//db->fBuff;
    db32.uPos=db->uPos;
    db32.uBuff=0;//db->uBuff;
    db32.emptyBuff=0;//db->emptyBuff;
    db32.readOnly=db->readOnly;
    db32.rBuffSz=db->rBuffSz;
    db32.swproc=0;//db->swproc;
    db32.reserved2=db->reserved2;
    db32.reserved3=db->reserved3;
    db32.reserved4=db->reserved4;
    db32.reserved5=db->reserved5;
    db32.reserved6=db->reserved6;
    db32.reserved7=db->reserved7;
    db32.reserved8=db->reserved8;
    db32.reserved9=db->reserved9;
    db32.reserved10=db->reserved10;
    db32.reserved11=db->reserved11;
    db32.reserved12=db->reserved12;
    db32.reserved13=db->reserved13;
    db32.reserved14=db->reserved14;
    db32.reserved15=db->reserved15;
    db32.reserved16=db->reserved16;
    db32.ff=0;//db->ff;
    db32.fu=0;//db->fu;
    db32.spy=db->spy;
    
//fprintf(stderr,"%d\n",db32.endian);
//fprintf(stderr,"%d\n",db32.nFld);
    DB_Swap32(&db32,bkn,db->swproc);
//fprintf(stderr,"%d\n",db32.endian);
//fprintf(stderr,"%d\n",db32.nFld);

    if(db32.fPos<0){
        db32.fPos=0;
    }
    if(db32.uPos<0){
        db32.uPos=0;
    }
    
    if(fwrite(&db32,sizeof(DB_info32),1,ff)!=1){
        return(-40);
    }
    
DB_fld *flds=malloc(bkn*sizeof(DB_fld));
    if(flds){
        for(long i=0;i<bkn;i++){
            flds[i]=db->flds[i];
        }
        DB_SwapFields(flds,bkn,db->swproc);
        if(fwrite(flds,bkn*sizeof(DB_fld),1,ff)!=1){
            free(flds);
            return(-41);
        }
        free(flds);
    }
    
    return 0;
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static int DB_writeHeaderInFile(DB_info* db, FILE* fp){
FILE* bkp=db->ff;
    db->ff=fp;
int status=DB_writeHeader(db);
    db->ff=bkp;
    return status;
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_fBuffLoad(int off, DB_info* db){
int	eof,sz;
	
// FSEEK A MODIFIER
	fseek(db->ff,0,SEEK_END);
// FTELL A MODIFIER
	eof=ftell(db->ff);
// FSEEK A MODIFIER
	fseek(db->ff,off,SEEK_SET);
	if((off+db->rBuffSz)>eof){
		sz=eof-off;
	}
	else{
		sz=db->rBuffSz;
	}
	db->fPos=off;
	return(fread(db->fBuff,sz,1,db->ff));
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_uBuffLoad(int off, DB_info* db){
int	eof,sz;
	
// FSEEK A MODIFIER
	fseek(db->fu,0,SEEK_END);
// FTELL A MODIFIER
	eof=ftell(db->fu);
// FSEEK A MODIFIER
	fseek(db->fu,off,SEEK_SET);
	if((off+db->rBuffSz)>eof){
		sz=eof-off;
	}
	else{
		sz=db->rBuffSz;
	}
	db->uPos=off;
	return(fread(db->uBuff,sz,1,db->fu));
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_fRead(void *p, int sz, int off, DB_info* db){	
    if(sz<=db->rBuffSz){
// on utilise le buffer pour accélérer la lecture des petits machins
		if((off+sz)>(db->fPos+db->rBuffSz)){
			DB_fBuffLoad(off,db);
		}
		else if(off<db->fPos){
			DB_fBuffLoad(off,db);
		}
		memmove(p,(void*)db->fBuff+off-db->fPos,sz);
	}
	else{
// pas la peine de se casser pour les gros, on lit tout d'un coup
// FSEEK A MODIFIER
		fseek(db->ff,off,SEEK_SET);
		fread(p,sz,1,db->ff);
	}
	return(sz);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static	size_t	DB_uRead(void	*p,	int	sz,	int	off,	DB_info* db){	
	if(sz<=db->rBuffSz){
// on utilise le buffer pour accélérer la lecture des petits machins
		if((off+sz)>(db->uPos+db->rBuffSz)){
			DB_uBuffLoad(off,db);
		}
		else if(off<db->uPos){
			DB_uBuffLoad(off,db);
		}
		memmove(p,(void*)db->uBuff+off-db->uPos,sz);
	}
	else{
// pas la peine de se casser pour les gros, on lit tout d'un coup
// FSEEK A MODIFIER
		fseek(db->fu,off,SEEK_SET);
		fread(p,sz,1,db->fu);
	}
	return(sz);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_fWrite(void* p, int sz, int off, DB_info* db){
int		start,end;
size_t	k;

// FSEEK A MODIFIER
	fseek(db->ff,off,SEEK_SET);
	k=fwrite(p,sz,1,db->ff);
	
	fflush(db->ff);// Il lui faut un flush pour valider la dernière modif??

	if((off+sz)<db->fPos){
		return(k);
	}
	if(off>(db->fPos+db->rBuffSz)){
		return(k);
	}
	
	start=off;
	end=start+sz;
	
	if(start<db->fPos){
		start=db->fPos;
	}
	if(end>(db->fPos+db->rBuffSz)){
		end=db->fPos+db->rBuffSz;
	}
	start-=db->fPos;
	end-=db->fPos;
	
	memmove((void*)db->fBuff+start,p,end-start);
	return(k);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_uWrite(void* p, int sz, int off, DB_info* db){
int		start,end;
size_t	k;
	
// FSEEK A MODIFIER
	fseek(db->fu,off,SEEK_SET);
	k=fwrite(p,sz,1,db->fu);
	
	fflush(db->fu);// Il lui faut un flush pour valider la dernière modif ??
	
	if((off+sz)<db->uPos){
		return(k);
	}
	if(off>(db->uPos+db->rBuffSz)){
		return(k);
	}
	
	start=off;
	end=start+sz;
	
	if(start<db->uPos){
		start=db->uPos;
	}
	if(end>(db->uPos+db->rBuffSz)){
		end=db->uPos+db->rBuffSz;
	}
	start-=db->uPos;
	end-=db->uPos;
	
	memmove((void*)db->uBuff+start,p,end-start);
	return(k);
}

//----------------------------------------------------------------------------
// 
//------------
static int DB_fu(DB_info* db){
	for(long i=0;i<db->nFld;i++){
		switch(db->flds[i].sign){
			case _binary:
			case _ivxs2:
			case _ivxs3:
			case _dvxs2:
			case _dvxs3:
				return(1);
				break;
		}
	}
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static int DB_CalcFieldSize(DB_fld* f){
	switch(f->sign){
		case _bool:
		case _int:
			return(sizeof(int));
			break;
		
		case _double:
		case _date:
		case _time:
			return(sizeof(double));
			break;
			
		case _ivx2:
			return(sizeof(i2dvertex));
			break;
		case _ivx3:
			return(sizeof(i3dvertex));
			break;
		case _ivxrect:
			return(sizeof(ivx_rect));
			break;
		case _ivxcube:
			return(sizeof(ivx_cube));
			break;
			
		case _dvx2:
			return(sizeof(d2dvertex));
			break;
		case _dvx3:
			return(sizeof(d3dvertex));
			break;
		case _dvxrect:
			return(sizeof(dvx_rect));
			break;
		case _dvxcube:
			return(sizeof(dvx_cube));
			break;
		
		case _binary:
		case _ivxs2:
		case _ivxs3:
		case _dvxs2:
		case _dvxs3:
			return(sizeof(DB_usz));
			break;
			
		case _char:{
				int	sz;
				
				sz=((f->length+1)/(sizeof(int)));
				if(sz){
					if((f->length+1)%(sizeof(int))){
						sz+=1;
					}
					sz*=(sizeof(int));
				}
				else{
					sz=sizeof(int);
				}
				return(sz);
			}
			break;
		default:
			return(0);
			break;
	}
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static DB_info*	DB_New(void){
DB_info*	db;
	
	db=(DB_info*)malloc(sizeof(DB_info));
	if(db){
		db->vers=_kDBCurrentVersion_;
		db->endian=1;
		db->spy=0;
		db->nRec=0;
		db->nFld=0;
		db->recSz=sizeof(int);
        db->dtOff=sizeof(DB_info32);
		db->fPos=0;
		db->uPos=0;
		db->rBuffSz=fcpybufsz_;
		db->fBuff=malloc(db->rBuffSz);
		if(!db->fBuff){
			free(db);
			return(NULL);
		}
		db->uBuff=malloc(db->rBuffSz);
		if(!db->uBuff){
			free(db->fBuff);
			free(db);
			return(NULL);
		}
		db->emptyBuff=malloc(db->recSz);
		if(!db->emptyBuff){
			free(db->fBuff);
			free(db->uBuff);
			free(db);
			return(NULL);
		}
		memset(db->emptyBuff,0,db->recSz);
		db->readOnly=0;
		db->ff=NULL;
		db->fu=NULL;
		db->swproc=dontswap;
	}
	return(db);
}

//----------------------------------------------------------------------------
// Pb si on veut ouvrir un ficher inexistant, on plante à l'ouverture suivante
// endian ok
//------------
static DB_info*	DB_OpenH(const char	*fName,	int	rw, int use_fu){
char*		fullName;
int			n;
FILE		*ff,*fu;
DB_info*	db;
DB_info32	db32;
    
	for(;;){
		db=DB_New();
		if(!db){
			break;
		}
		free(db->fBuff);
		free(db->uBuff);
		free(db->emptyBuff);
		db->fBuff=NULL;
		db->uBuff=NULL;
		db->emptyBuff=NULL;
	
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.fmt",fName);
		if(rw){	
			ff=fopen(fullName,"rb+");
		}
		else{
			ff=fopen(fullName,"rb");
		}
		free(fullName);
		if(!ff){
			break;
		}
		
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.unf",fName);
		if(rw){
			fu=fopen(fullName,"rb+");
		}
		else{
			fu=fopen(fullName,"rb");
		}
		free(fullName);
		if(!fu){
			break;
		}		
		
// FSEEK A MODIFIER
		fseek(ff,0,SEEK_SET);
        
        if(fread(&db32,sizeof(DB_info32),1,ff)!=1){
fprintf(stderr,"DB_OpenH : fread db failed\n");
			break;
		}
        
        
fprintf(stderr,"opening %s\n",fName);
       
        DB32_CheckEndian(&db32,db);
        
		n=db->nFld;
		if(n<0){
fprintf(stderr,"DB_OpenH : bad nFld %d\n",n);
			break;
		}
		db=(DB_info*)realloc(db,sizeof(DB_info)+n*sizeof(DB_fld));
		if(!db){
fprintf(stderr,"DB_OpenH : realloc failed\n");
			break;
		}
        
// FSEEK A MODIFIER
        fseek(ff,sizeof(DB_info32),SEEK_SET);
        if(n>0){
            if(fread(db->flds,n*sizeof(DB_fld),1,ff)!=1){
fprintf(stderr,"DB_OpenH : fread db failed for %d fields\n",n);
                break;
            }
        }
        DB_CheckEndianFields(db);
        
		/*fu*/
		if(use_fu==fuFit){
			use_fu=DB_fu(db);
		}
		if(!use_fu){
			fclose(fu);
			fu=NULL;
		}
		/*fu*/
		
		db->ff=ff;
		db->fu=fu;
		
		db->readOnly=(!rw);
		db->spy=1;
		
		db->fBuff=NULL;
		db->uBuff=NULL;
		db->emptyBuff=NULL;
		
		if(db->rBuffSz<=0){
			db->rBuffSz=fcpybufsz_;
		}

		db->fBuff=malloc(db->rBuffSz);
		if(!db->fBuff){
			db->rBuffSz=fcpybufsz_;
			db->fBuff=malloc(db->rBuffSz);
			if(!db->fBuff){
fprintf(stderr,"DB_OpenH : malloc failed for rBuffSz=%lu\n",db->rBuffSz);
				break;
			}
		}
		/*fu*/
		if(db->fu){
			db->uBuff=malloc(db->rBuffSz);
			if(!db->uBuff){
fprintf(stderr,"malloc failed for rBuffSz=%lu\n",db->rBuffSz);
				break;
			}
		}
		/*fu*/
		
		db->emptyBuff=malloc(db->recSz);
		if(!db->emptyBuff){
fprintf(stderr,"malloc failed for recSz=%lu\n",db->recSz);
			break;
		}
		memset(db->emptyBuff,0,db->recSz);

// FSEEK A MODIFIER
		fseek(db->ff,0,SEEK_SET);
		if(db->fu){
// FSEEK A MODIFIER
			fseek(db->fu,0,SEEK_SET);
		}
		
		if(!db->readOnly){
			int k=1;
			(*(db->swproc))(sizeof(int),1,&k);
			if(fwrite(&k,sizeof(int),1,db->ff)!=1){
fprintf(stderr,"DB_OpenH : fwrite failed\n");
				break;
			}
			/*fu*/
			if(db->fu){
				if(fwrite(&k,sizeof(int),1,db->fu)!=1){
fprintf(stderr,"DB_OpenH : fwrite failed\n");
					break;
				}
			}
			/*fu*/
		}
		
		DB_fBuffLoad(db->dtOff,db);
		/*fu*/
		if(db->fu){
			DB_uBuffLoad(sizeof(int),db);
		}
		/*fu*/


		return(db);
	}
	
	if(db){
		if(db->ff){
			fclose(db->ff);
		}
		if(db->fu){
			fclose(db->fu);
		}
		if(db->fBuff){
			free(db->fBuff);
		}
		if(db->uBuff){
			free(db->uBuff);
		}
		if(db->emptyBuff){
			free(db->emptyBuff);
		}
		free(db);
	}
	return(NULL);
}

//----------------------------------------------------------------------------
// Adapter la taille et les décimales en fonction du type
// no endian needed
//------------
static int DB_AddFieldInStruct(	DB_info** db,
								const char* nam,
								int sign,
								int length,
								int decs){
int	n;

	/*fu*/
	if(!(*db)->fu){
		switch(sign){
			case _binary:
			case _ivxs2:
			case _ivxs3:
			case _dvxs2:
			case _dvxs3:
				return(-20);
				break;
		}
	}
	/*fu*/
	
	if(DB_NamedFieldIdx((*db),nam,&n)>-1){
		return(-31);
	}
	n=sizeof(DB_info)+(((*db)->nFld+1)*sizeof(DB_fld));
	(*db)=(DB_info*)realloc((*db),n);
	if(!(*db)){
		return(-32);
	}
	
	(*db)->flds[(*db)->nFld].sign=sign;
	
	(*db)->flds[(*db)->nFld].fid=0;//
	
	strcpy((*db)->flds[(*db)->nFld].nam,nam);
	(*db)->flds[(*db)->nFld].attrs=0;
	(*db)->flds[(*db)->nFld].length=length;
	(*db)->flds[(*db)->nFld].decs=decs;
	(*db)->flds[(*db)->nFld].sz=DB_CalcFieldSize(&(*db)->flds[(*db)->nFld]);
	
	if((*db)->nFld>0){
		(*db)->flds[(*db)->nFld].off=(*db)->flds[(*db)->nFld-1].off+(*db)->flds[(*db)->nFld-1].sz;
	}
	else{
		(*db)->flds[(*db)->nFld].off=sizeof(int);
	}	
	
	(*db)->recSz+=(*db)->flds[(*db)->nFld].sz;
	(*db)->dtOff+=sizeof(DB_fld);
	
	(*db)->emptyBuff=realloc((*db)->emptyBuff,(*db)->recSz);
	if(!(*db)->emptyBuff){
		return(-4);
	}
	memset((*db)->emptyBuff,0,(*db)->recSz);	
	(*db)->nFld++;
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_AddFieldNoRec(	DB_info** db,
								const char* nam,
								int sign,
								int length,
								int decs){
int	n=DB_AddFieldInStruct(db,nam,sign,length,decs);
	if(n){
fprintf(stderr,"DB_AddFieldNoRec : DB_AddFieldInStruct failed %d\n",n);
		return(n);
	}

    n=DB_writeHeader(*db);
    if(n){
fprintf(stderr,"DB_AddFieldNoRec : DB_writeHeader failed %d\n",n);
        return(n);
    }
    
	return((*db)->nFld-1);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int	DB_AddFieldWithRec(	DB_info** db,
								const char* nam,
								int sign,
								int length,
								int decs){
FILE*	fp;
int		n;
long    i;
size_t  oldsz,oldoff;

    fp=tmpfile();
	if(!fp){
		return(-3);
	}

	oldoff=(*db)->dtOff;
	oldsz=(*db)->recSz;
	
	n=DB_AddFieldInStruct(db,nam,sign,length,decs);
	if(n){
		return(n);
	}
    
    n=DB_writeHeaderInFile(*db,fp);
    if(n){
fprintf(stderr,"DB_AddFieldWithRec : DB_writeHeaderInFile failed %d\n",n);
        return(n);
    }
    
	for(i=0;i<(*db)->nRec;i++){
// FSEEK A MODIFIER
		fseek((*db)->ff,oldoff+i*oldsz,SEEK_SET);
		fread((*db)->emptyBuff,oldsz,1,(*db)->ff);
		
// FSEEK A MODIFIER
		fseek(fp,(*db)->dtOff+i*(*db)->recSz,SEEK_SET);
		fwrite((*db)->emptyBuff,(*db)->recSz,1,fp);
	}
	
	filecpy((*db)->ff,fp);
	fclose(fp);
	
	return((*db)->nFld-1);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_RmvFieldFromStruct(DB_info** db, int idx){
int	n,sz;
	
	sz=(*db)->flds[idx].sz;

	pushOffsets((*db),idx+1,-sz);
	
	if(idx<(*db)->nFld-1){
		memmove(&(*db)->flds[idx],&(*db)->flds[idx+1],((*db)->nFld-(idx+1))*sizeof(DB_fld));
	}
	
	n=sizeof(DB_info)+(((*db)->nFld-1)*sizeof(DB_fld));
	(*db)=(DB_info*)realloc((*db),n);
	if(!(*db)){
		return(-2);
	}	
	
	(*db)->recSz-=sz;
	(*db)->dtOff-=sizeof(DB_fld);
	
	(*db)->emptyBuff=(char*)realloc((*db)->emptyBuff,(*db)->recSz);
	if(!(*db)->emptyBuff){
		return(-3);
	}
	memset((*db)->emptyBuff,0,(*db)->recSz);

	(*db)->nFld--;
	
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_RmvFieldNoRec(DB_info** db, int idx){
int	n=DB_RmvFieldFromStruct(db,idx);
	if(n){
		return(n);
	}
    
    n=DB_writeHeader(*db);
    if(n){
fprintf(stderr,"DB_RmvFieldNoRec : DB_writeHeader failed %d\n",n);
        return(n);
    }

	return((*db)->nFld);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_RmvFieldWithRec(DB_info** db, int idx){
FILE*	fp;
int		n;
long    i;
char*	buff;
size_t  oldsz,oldoff,sz1,sz2,o2;
    
	fp=tmpfile();
	if(!fp){
		return(-11);
	}
	
	sz1=sizeof(int);
	for(i=0;i<idx;i++){
		sz1+=(*db)->flds[i].sz;
	}
	o2=sz1+(*db)->flds[idx].sz;
	sz2=0;
	for(i=idx+1;i<(*db)->nFld;i++){
		sz2+=(*db)->flds[i].sz;
	}
	
	oldoff=(*db)->dtOff;
	oldsz=(*db)->recSz;
	buff=(char*)malloc(oldsz);
	if(!buff){
		return(-12);
	}
	
	n=DB_RmvFieldFromStruct(db,idx);
	if(n){
		return(n);
	}
    
    n=DB_writeHeaderInFile(*db,fp);
    if(n){
fprintf(stderr,"DB_RmvFieldWithRec : DB_writeHeaderInFile failed %d\n",n);
        return(n);
    }
    
	for(i=0;i<(*db)->nRec;i++){
// FSEEK A MODIFIER
		fseek((*db)->ff,oldoff+i*oldsz,SEEK_SET);
		fread(buff,oldsz,1,(*db)->ff);
		
// FSEEK A MODIFIER
		fseek(fp,(*db)->dtOff+i*(*db)->recSz,SEEK_SET);
		
		fwrite(buff,sz1,1,fp);
		
		if(sz2){
			fwrite(buff+o2,sz2,1,fp);
		}
	}
	
	filecpy((*db)->ff,fp);
	fclose(fp);
		
	return((*db)->nFld);
}

//----------------------------------------------------------------------------
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur 
// variable ne peut pas être converti
// no endian needed
//------------
static int DB_ChgFieldInStruct(	DB_info* db,
								int idx,
								const char* nam,
								int sign,
								int length,
								int decs){
int		szfrom,szto;
DB_fld	f;

// Modifs sur le champ	
	f=db->flds[idx];
	szfrom=f.sz;
	f.sign=sign;
	f.length=length;
	f.decs=decs;
	szto=DB_CalcFieldSize(&f);
	f=db->flds[idx];
	/*fu*/
	if(!db->fu){
		switch(sign){
			case _binary:
			case _ivxs2:
			case _ivxs3:
			case _dvxs2:
			case _dvxs3:
				return(-20);
				break;
		}
		switch(f.sign){
			case _binary:
			case _ivxs2:
			case _ivxs3:
			case _dvxs2:
			case _dvxs3:
				return(-20);
				break;
		}
	}
	/*fu*/
// Nom	
	if(strcmp(nam,f.nam)){
		strcpy(f.nam,nam);
	}
// Type de champ	
	if(f.sign!=sign){
		switch(sign){	
			case _bool:
			case _int:
				switch(f.sign){
					case _bool:
					case _int:
						break;
					
					case _date:
					case _time:
					case _double:
					case _char:
						f.decs=0;
						break;
					
					default:
						return(-1);
				}				
				break;
				
			case _double:
			case _date:
			case _time:
				switch(f.sign){
					case _bool:
					case _int:
					case _char:
						f.decs=decs;
						break;
					default:
						return(-1);
				}				
				break;
			
			case _char:
				switch(f.sign){
					case _bool:
					case _int:
					case _date:
					case _time:
					case _double:
						f.decs=0;
						break;
					default:
						return(-1);
				}				
				break;
				
			case _ivx2:
			case _ivx3:
			case _ivxrect:
			case _ivxcube:
			case _ivxs2:
			case _ivxs3:
			case _dvx2:
			case _dvx3:
			case _dvxrect:
			case _dvxcube:
			case _dvxs2:
			case _dvxs3:
			case _binary:
			default:
				return(-1);
				break;
				
		}
		f.sign=sign;
		f.sz=szto;
	}
// Longueur utilisateur
	if(length!=f.length){
		f.length=length;
		if(f.sign==_char){
			f.sz=szto;
		}
	}
// Décimales
	if((f.sign==_double)&&(decs!=f.decs)){
		f.decs=decs;
	}

	db->flds[idx]=f;

// Modifs sur la structure de la table
	pushOffsets(db,idx+1,szto-szfrom);
	db->recSz+=(szto-szfrom);	
	db->emptyBuff=(char*)realloc(db->emptyBuff,db->recSz);
	if(!db->emptyBuff){
		return(-2);
	}
	memset(db->emptyBuff,0,db->recSz);
	
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int	DB_ChgFieldNoRec(	DB_info* db,
								int idx,
								const char* nam,
								int sign,
								int length,
								int decs){	
int	n=DB_ChgFieldInStruct(db,idx,nam,sign,length,decs);
	if(n){
		return(n);
	}
	
    n=DB_writeHeader(db);
    if(n){
fprintf(stderr,"DB_ChgFieldNoRec : DB_writeHeader failed %d\n",n);
        return(n);
    }

	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_ChgFieldWithRec(	DB_info* db,
								int idx,
								const char* nam,
								int sign,
								int length,
								int decs){
FILE*	fp;
int		n,oldfsign;
long    i;
size_t  oldsz,oldoff,sz1,sz2,o2,oldfsz;
char	*buff;
void*	bf;
DB_fld	f;
	
	fp=tmpfile();
	if(!fp){
		return(-1);
	}
	
	sz1=sizeof(int);
	for(i=0;i<idx;i++){
		sz1+=db->flds[i].sz;
	}
	
	o2=sz1+db->flds[idx].sz;
	sz2=0;
	for(i=idx+1;i<db->nFld;i++){
		sz2+=db->flds[i].sz;
	}
	
	f=db->flds[idx];
	
	oldoff=db->dtOff;
	oldsz=db->recSz;
	buff=(char*)malloc(oldsz);
	if(!buff){
		return(-2);
	}
	
	/**/
	oldfsign=db->flds[idx].sign;
	oldfsz=db->flds[idx].sz;
	/**/
	
	n=DB_ChgFieldInStruct(db,idx,nam,sign,length,decs);
	if(n){
		return(n);
	}
	
	bf=malloc(db->flds[idx].sz);
	if(!bf){
		return(-3);
	}
	
	/**/
	if((sign==oldfsign)&&(db->flds[idx].sz==oldfsz)){
		fclose(fp);
		fp=db->ff;
	}
	/**/
	
    n=DB_writeHeader(db);
    if(n){
fprintf(stderr,"DB_ChgFieldWithRec : DB_writeHeader failed %d\n",n);
        return(n);
    }

	/**/
	if((sign==oldfsign)&&(db->flds[idx].sz==oldfsz)){
		free(buff);
		free(bf);
		return(0);
	}
	/**/
	
fprintf(stderr,"DB_ChgFieldWithRec : Data conversion\n");
fflush(stderr);

	for(i=0;i<db->nRec;i++){
// FSEEK A MODIFIER
		fseek(db->ff,oldoff+i*oldsz,SEEK_SET);
		fread(buff,oldsz,1,db->ff);
		
// FSEEK A MODIFIER
		fseek(fp,db->dtOff+i*db->recSz,SEEK_SET);
		
		fwrite(buff,sz1,1,fp);

		/**/
		switch(f.sign){
			case _bool:
			case _int:
				db->swproc(sizeof(int),1,buff+sz1);
				break;
			case _double:
			case _date:
			case _time:
				db->swproc(sizeof(double),1,buff+sz1);
				break;
			case _char:
				break;				
		}
		/**/
		
		
		switch(sign){
			case _bool:
			case _int:
				xToInt(f.sign,f.decs,buff+sz1,bf);
				db->swproc(sizeof(int),1,bf);
				break;
			case _double:
				xToDouble(f.sign,f.decs,buff+sz1,bf);
				db->swproc(sizeof(double),1,bf);
				break;
			case _date:
				xToDate(f.sign,f.decs,buff+sz1,bf);
				db->swproc(sizeof(double),1,bf);
				break;
			case _time:
				xToTime(f.sign,f.decs,buff+sz1,bf);
				db->swproc(sizeof(double),1,bf);
				break;
			case _char:
				xToChar(f.sign,f.decs,buff+sz1,bf);
				break;
		}
		
		
		/**/
		switch(f.sign){
			case _bool:
			case _int:
				db->swproc(sizeof(int),1,buff+sz1);
				break;
			case _double:
			case _date:
			case _time:
				db->swproc(sizeof(double),1,buff+sz1);
				break;
			case _char:
				break;				
		}
		/**/
		
		fwrite(bf,db->flds[idx].sz,1,fp);
		
		if(sz2){
			fwrite(buff+o2,sz2,1,fp);
		}
	}

fprintf(stderr,"DB_ChgFieldWithRec : Data conversion passed\n");
fflush(stderr);
	
	free(buff);
	free(bf);
	
	filecpy(db->ff,fp);
	fclose(fp);
		
	return(0);
}

//----------------------------------------------------------------------------
// Public APIs
//----------------------------------------------------------------------------
#pragma mark -
#pragma mark =>Public APIs
//----------------------------------------------------------------------------
// creating
// no endian needed
//------------
DB_info*	DB_Create(const char *fName, int use_fu){
char*		fullName;
FILE*		fp;
DB_info*	db;
	
	if(use_fu==fuFit){
		use_fu=1;
	}

	for(;;){
		db=DB_New();
		if(!db){
fprintf(stderr,"DB_Create : ERROR : DB_New return NULL\n");
			break;
		}
				
		db->spy=1;
		
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.fmt",fName);
    
		fp=fopen(fullName,"wb");
		if(!fp){
fprintf(stderr,"DB_Create : ERROR : fopen %s wb return NULL (%d)\n",fullName,errno);
			break;
		}
		fwrite(&db->spy,sizeof(int),1,fp);
		fclose(fp);
			
		fp=fopen(fullName,"rb+");
		free(fullName);
		if(!fp){
fprintf(stderr,"DB_Create : ERROR : fopen %s rb+ return NULL (%d)\n",fullName,errno);
			break;
		}
		db->ff=fp;		
		
// FSEEK A MODIFIER
		/*fseek(db->ff,0,SEEK_SET);
		fwrite(db,sizeof(DB_info),1,db->ff);*/
        DB_writeHeader(db);
		
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.unf",fName);
    
		fp=fopen(fullName,"wb");
		if(!fp){
fprintf(stderr,"DB_Create : ERROR : fopen %s wb return NULL (%d)\n",fullName,errno);
			break;
		}
		fwrite(&db->spy,sizeof(int),1,fp);
		fclose(fp);
			
		fp=fopen(fullName,"rb+");
		free(fullName);
		if(!fp){
fprintf(stderr,"DB_Create : ERROR : fopen %s rb+ return NULL (%d)\n",fullName,errno);
			break;
		}		
		
		/*fu*/
		if(!use_fu){
			fclose(fp);
			fp=NULL;
			free(db->uBuff);
			db->uBuff=NULL;
		}
		/*fu*/
		db->fu=fp;
        
		DB_fBuffLoad(db->dtOff,db);
		/*fu*/
		if(db->fu){
			DB_uBuffLoad(sizeof(int),db);
		}
		/*fu*/
		return(db);
	}	
	if(db){
		if(db->ff){
			fclose(db->ff);
		}
		if(db->fu){
			fclose(db->fu);
		}
		if(db->fBuff){
			free(db->fBuff);
		}
		if(db->uBuff){
			free(db->uBuff);
		}
		if(db->emptyBuff){
			free(db->emptyBuff);
		}
		free(db);
	}
	return(NULL);
}

//----------------------------------------------------------------------------
// opening read/write mode
// no endian needed
//------------
DB_info* DB_OpenRW(const char *fName, int use_fu){
DB_info* db;
// Pour éviter plantage : test si la table existe
	db=DB_OpenH(fName,0,use_fu);
	if(!db){
//fprintf(stderr,"DB_OpenH failed\n");
		return(NULL);
	}
	DB_Close(db);
	return(DB_OpenH(fName,1,use_fu));
}

//----------------------------------------------------------------------------
// opening readonly mode
// no endian needed
//------------
DB_info* DB_OpenRO(const char *fName, int use_fu){
	return(DB_OpenH(fName,0,use_fu));
}

//----------------------------------------------------------------------------
// endian ok
//------------
void DB_Close(DB_info* db){
	if(!db){
		return;
	}
	db->spy=0;

	(*(db->swproc))(sizeof(int),1,&db->spy);
// FSEEK A MODIFIER
	fseek(db->ff,0,SEEK_SET);
	if(!db->readOnly){
		fwrite(&db->spy,sizeof(int),1,db->ff);
	}
	fflush(db->ff);
	fclose(db->ff);
	
	/*fu*/
	if(db->fu){
	/*fu*/
/*ENDIAN*/
		(*(db->swproc))(sizeof(int),1,&db->spy);
// FSEEK A MODIFIER
		fseek(db->fu,0,SEEK_SET);
		if(!db->readOnly){
			fwrite(&db->spy,sizeof(int),1,db->fu);
		}
		fflush(db->fu);
		fclose(db->fu);
	/*fu*/
	}
	/*fu*/
	
	if(db->fBuff){
		free(db->fBuff);
	}
	if(db->uBuff){
		free(db->uBuff);
	}
	if(db->emptyBuff){
		free(db->emptyBuff);
	}
	free(db);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_NamedFieldIdx(DB_info	*db, const char *nam, int *idx){		
	for(long i=0;i<db->nFld;i++){
		if(!strcmp(nam,db->flds[i].nam)){
			(*idx)=i;
			return(0);
		}
	}
	return(-1);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_FieldSign(DB_info	*db,	int	iFld,	int	*sign){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*sign)=db->flds[iFld].sign;
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
/*int	DB_FieldID(DB_info	*db,	int	iFld,	int	*id){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*id)=db->flds[iFld].id;
	return(0);
}*/

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_FieldName(DB_info	*db,	int	iFld,	char	*name){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	strcpy(name,db->flds[iFld].nam);
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_FieldSize(DB_info	*db,	int	iFld,	int	*sz){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	switch(db->flds[iFld].sign){
		case _bool:
		case _int:
		case _double:
		case _date:
		case _time:
		case _ivx2:
		case _ivx3:
		case _ivxrect:
		case _ivxcube:
		case _dvx2:
		case _dvx3:
		case _dvxrect:
		case _dvxcube:
		case _char:
			(*sz)=db->flds[iFld].sz;
			break;
		
		case _binary:
			(*sz)=sizeof(DB_cont);
			break;
			
		case _ivxs2:
		case _ivxs3:
			(*sz)=sizeof(ivertices*);
			break;
		case _dvxs2:
		case _dvxs3:
			(*sz)=sizeof(dvertices*);
			break;
	
		default:
			return(-1);
			break;

	}	
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_FieldDecimals(DB_info	*db,	int	iFld,	int	*decs){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*decs)=db->flds[iFld].decs;
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_FieldLength(DB_info* db, int iFld, int* len){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*len)=db->flds[iFld].length;
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_CountFields(DB_info	*db){		
	return(db->nFld);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_CountRecords(DB_info	*db){		
	return(db->nRec);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_AddField(DB_info	**db, const char*	nam, int sign, int length, int decs){
int	status;	
	if((*db)->readOnly){
fprintf(stderr,"DB_AddField : readOnly\n");
        return(-1);
	}
	if(!SignTest(sign)){
fprintf(stderr,"DB_AddField : SignTest err\n");
		return(-1);
	}
	if((*db)->nRec==0){
		status=DB_AddFieldNoRec(db,nam,sign,length,decs);
	}
	else{
		status=DB_AddFieldWithRec(db,nam,sign,length,decs);
	}
    if(status<0){
fprintf(stderr,"DB_AddField : status = %d\n",status);
    }
	DB_fBuffLoad((*db)->dtOff,(*db));
	return(status);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_ChgField(DB_info	*db, int idx, const char* nam, int sign, int length, int decs){		
int	x,index,status;
	
	if(db->readOnly){
		return(-1);
	}
	if((idx<0)||(idx>=db->nFld)){
		return(-1);
	}
	if(!SignTest(sign)){
		return(-2);
	}
	x=DB_NamedFieldIdx(db,nam,&index);
	if((x==0)&&(index!=idx)){
		return(-3);
	}
	
	if(db->nRec==0){
		status=DB_ChgFieldNoRec(db,idx,nam,sign,length,decs);
	}
	else{
		status=DB_ChgFieldWithRec(db,idx,nam,sign,length,decs);
	}
	DB_fBuffLoad(db->dtOff,db);
	return(status);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_RmvField(DB_info **db, int idx){
int	status;
	
	if((*db)->readOnly){
fprintf(stderr,"DB_RmvField::ERROR : readOnly\n");		
		return(-21);
	}
	if((idx<0)||(idx>=(*db)->nFld)){
fprintf(stderr,"DB_RmvField::ERROR : index out of range\n");		
		return(-22);
	}
	if((*db)->nRec==0){
		status=DB_RmvFieldNoRec(db,idx);
	}
	else{
		status=DB_RmvFieldWithRec(db,idx);
	}
	DB_fBuffLoad((*db)->dtOff,(*db));
	return(status);
}


//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_ReadVal(DB_info* db, int iRec, int iFld, void* val){
size_t	off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-2);
	}
	off=db->dtOff+iRec*db->recSz+db->flds[iFld].off;
    
	switch(db->flds[iFld].sign){
		case _ivxs2:
		case _ivxs3:{
DB_usz          usz;
ivertices*      vxs;
ivertices32*    vxs32;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					(*((ivertices**)val))=NULL;
					return(0);
				}
				vxs32=(ivertices32*)malloc(usz.sz);
				if(!vxs32){
					(*((ivertices**)val))=NULL;
					return(-1);
				}
				DB_uRead(vxs32,usz.sz,usz.off,db);
				DB_Swap_ivx32(db,vxs32);
				vxs=ivs_unpack32(vxs32);
                free(vxs32);
				(*((ivertices**)val))=vxs;
			}
			break;
			
		case _char:
			DB_fRead(val,db->flds[iFld].length+1,off,db);
			break;
			
		case _bool:
		case _int:
		case _double:
		case _date:
		case _time:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(db->flds[iFld].sz,1,val);
			break;
		case _ivx2:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),2,val);
			break;
		case _ivx3:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),3,val);
			break;
		case _ivxrect:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),4,val);
			break;
		case _ivxcube:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),6,val);
			break;
		case _dvx2:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),2,val);
			break;
		case _dvx3:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),3,val);
			break;
		case _dvxrect:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),4,val);
			break;
		case _dvxcube:
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),6,val);
			break;

		case _dvxs2:
		case _dvxs3:{
DB_usz		usz;
dvertices*	vxs;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					(*((dvertices**)val))=NULL;
					return(0);
				}
				vxs=(dvertices*)malloc(usz.sz);
				if(!vxs){
					(*((dvertices**)val))=NULL;
					return(-1);
				}
				DB_uRead(vxs,usz.sz,usz.off,db);
				DB_Swap_dvx(db,vxs);
				dvs_unpack(vxs);
				(*((dvertices**)val))=vxs;
			}
			break;

		case _binary:{
DB_usz		usz;
void*		p;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					(*((void**)val))=NULL;
					return(0);
				}
				p=malloc(usz.sz);
				if(!p){					
					(*((void**)val))=NULL;
					return(-1);
				}
				DB_uRead(p,usz.sz,usz.off,db);
				(*((void**)val))=p;
			}
			break;
			
		default:
			break;
	}
	
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_WriteVal(DB_info	*db, int iRec, int iFld, void *val){
size_t	off;
	
	if(db->readOnly){
		return(-1);
	}
	if((iRec>db->nRec)||(iRec<0)){
		return(-2);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-3);
	}
	if(iRec==db->nRec){
		off=(db->dtOff)+((db->nRec*db->recSz));
		db->nRec++;
// FSEEK A MODIFIER
		fseek(db->ff,0,SEEK_SET);
        DB_writeHeader(db);
		memset(db->emptyBuff,0,db->recSz);
		DB_fWrite(db->emptyBuff,db->recSz,off,db);
	}
	off=(db->dtOff)+((iRec*db->recSz)+db->flds[iFld].off);
		
	switch(db->flds[iFld].sign){
		case _ivxs2:
		case _ivxs3:{
int             sz,h,v,o,bks,bkv,bko;
DB_usz          usz;
ivertices*      vxs;
ivertices32*    vxs32;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				vxs=(*((ivertices**)val));
				
				bks=vxs->sign;
				bkv=vxs->nv;
				bko=vxs->no;
				
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				sz=ivssize32(vxs,&h,&v,&o);
				if(sz>usz.sz){
// FSEEK A MODIFIER
					fseek(db->fu,0,SEEK_END);
// FTELL A MODIFIER
					usz.off=ftell(db->fu);
					usz.sz=sz;
					(*(db->swproc))(sizeof(int),2,&usz);
					DB_fWrite(&usz,sizeof(DB_usz),off,db);
					(*(db->swproc))(sizeof(int),2,&usz);
				}
            
                if(!ivs2ivs32(vxs,&vxs32)){
                    if(vxs->sign==_2D_VX){
                        (*(db->swproc))(sizeof(int),4+vxs->nv*2+vxs->no,vxs32);
                    }
                    else{
                        (*(db->swproc))(sizeof(int),4+vxs->nv*3+vxs->no,vxs32);
                    }
                    DB_uWrite(vxs32,h+v+o,usz.off,db);
                    free(vxs32);
                }
                else{
fprintf(stderr,"DB_WriteVal : ivs2ivs32 error\n");
                }
			}
			break;
			
		case _char:
			DB_fWrite(val,db->flds[iFld].length,off,db);
			break;
			
		case _bool:
		case _int:
		case _double:
		case _date:
		case _time:
			(*(db->swproc))(db->flds[iFld].sz,1,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(db->flds[iFld].sz,1,val);
			break;
			
		case _ivx2:
			(*(db->swproc))(sizeof(int),2,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),2,val);
			break;
		case _ivx3:
			(*(db->swproc))(sizeof(int),3,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),3,val);
			break;
		case _ivxrect:
			(*(db->swproc))(sizeof(int),4,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),4,val);
			break;
		case _ivxcube:
			(*(db->swproc))(sizeof(int),6,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(int),6,val);
			break;
		case _dvx2:
			(*(db->swproc))(sizeof(double),2,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),2,val);
			break;
		case _dvx3:
			(*(db->swproc))(sizeof(double),3,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),3,val);
			break;
		case _dvxrect:
			(*(db->swproc))(sizeof(double),4,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),4,val);
			break;
		case _dvxcube:
			(*(db->swproc))(sizeof(double),6,val);
			DB_fWrite(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),6,val);
			break;
		
		case _dvxs2:
		case _dvxs3:/*{
int			sz,h,v,o,bks,bkv,bko;
DB_usz		usz;
dvertices*	vxs;
				*//*fu*//*
				if(!db->fu){
					return(-20);
				}
				*//*fu*//*
				vxs=(*((dvertices**)val));

				bks=vxs->sign;
				bkv=vxs->nv;
				bko=vxs->no;

				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				sz=dvssize(vxs,&h,&v,&o);
				if(sz>usz.sz){
// FSEEK A MODIFIER
					fseek(db->fu,0,SEEK_END);
// FTELL A MODIFIER
					usz.off=ftell(db->fu);
					usz.sz=sz;
					(*(db->swproc))(sizeof(int),2,&usz);
					DB_fWrite(&usz,sizeof(DB_usz),off,db);
					(*(db->swproc))(sizeof(int),2,&usz);
				}
				DB_SwapAndBack_dvx(db,vxs,bks,bkv,bko);
				DB_uWrite(vxs,h+v,usz.off,db);
				if(o>0){
					DB_uWrite(vxs->offs,o,usz.off+h+v,db);
				}
				DB_SwapAndBack_dvx(db,vxs,bks,bkv,bko);
			}*/
			break;
		
		case _binary:{
int			sz;
DB_usz		usz;
DB_cont*	p;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				p=(DB_cont*)val;
				sz=p->sz;
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(sz>usz.sz){
// FSEEK A MODIFIER
					fseek(db->fu,0,SEEK_END);
// FTELL A MODIFIER
					usz.off=ftell(db->fu);
					usz.sz=sz;
					(*(db->swproc))(sizeof(int),2,&usz);
					DB_fWrite(&usz,sizeof(DB_usz),off,db);
					(*(db->swproc))(sizeof(int),2,&usz);
				}
				DB_uWrite(p,sz,usz.off,db);
			}
			break;
			
		default:
			break;
	}
	
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	DB_ReadStringVal(DB_info	*db, int iRec, int iFld, char *val){
int		r;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-1);
	}
	switch(db->flds[iFld].sign){
		case _char:
			r=DB_ReadVal(db,iRec,iFld,val);
			break;
		case _bool:
		case _int:{
int	iVal;
			r=DB_ReadVal(db,iRec,iFld,&iVal);
			if(!r){
				sprintf(val,"%d",iVal);
			}
			}
			break;
		case _double:{
double	dVal;
			r=DB_ReadVal(db,iRec,iFld,&dVal);
			if(!r){
				sprintf(val,"%.*f",db->flds[iFld].decs,dVal);		
			}
			}
			break;
		case _date:{
time_t	t;
double	dVal;
			r=DB_ReadVal(db,iRec,iFld,&dVal);
			if(!r){
				t=dVal;
				strcpy(val,asctime(localtime(&t)));
				val[strlen(val)-1]=0;	
			}
			}
			break;	
		default:
			strcpy(val,"NOT CONVERTED");
			r=0;
			break;
	}
	return(r);
}

//----------------------------------------------------------------------------
// no endian
//------------
int	DB_WriteStringVal(DB_info* db, int iRec, int iFld, char *val){
int     iVal,r;
double  dVal;

    if((iRec>db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-1);
	}
	
	switch(db->flds[iFld].sign){
		case _char:
			r=DB_WriteVal(db,iRec,iFld,val);
			break;
		case _bool:
		case _int:
			convertFromString(val,&db->flds[iFld],&iVal);
			r=DB_WriteVal(db,iRec,iFld,&iVal);
			break;
		case _double:
			convertFromString(val,&db->flds[iFld],&dVal);
			r=DB_WriteVal(db,iRec,iFld,&dVal);
			break;
		default:
			r=0;
			break;
	}
	return(r);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_AllocVal(DB_info* db, int iRec, int iFld, int* sz, void** val){
int	off;
	
	(*sz)=0;
	(*val)=NULL;
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-1);
	}
	off=db->dtOff+iRec*db->recSz+db->flds[iFld].off;	
	switch(db->flds[iFld].sign){
		case _ivxs2:
		case _ivxs3:{
DB_usz          usz;
ivertices*      vxs;
ivertices32*    vxs32;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				vxs32=(ivertices32*)malloc(usz.sz);
				if(!vxs32){
					return(-1);
				}
				DB_uRead(vxs32,usz.sz,usz.off,db);
				DB_Swap_ivx32(db,vxs32);
				vxs=ivs_unpack32(vxs32);
				(*sz)=usz.sz;
				(*val)=vxs;
			}
			break;
			
		case _char:{
int				size=db->flds[iFld].length+1;
				(*val)=malloc(size);
				if(!(*val)){
					return(-1);
				}
				(*sz)=size;
				DB_fRead((*val),size,off,db);
			}
			break;
			
		case _bool:
		case _int:
		case _double:
		case _date:
		case _time:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(db->flds[iFld].sz,1,val);
			break;
		
		case _ivx2:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),2,val);
			break;
		case _ivx3:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),3,val);
			break;
		case _ivxrect:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),4,val);
			break;
		case _ivxcube:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),6,val);
			break;
		case _dvx2:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),2,val);
			break;
		case _dvx3:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),3,val);
			break;
		case _dvxrect:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),4,val);
			break;
		case _dvxcube:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			DB_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),6,val);
			break;

		case _dvxs2:
		case _dvxs3:/*{
DB_usz		usz;
dvertices*	vxs;
				*//*fu*//*
				if(!db->fu){
					return(-20);
				}
				*//*fu*//*
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				vxs=(dvertices*)malloc(usz.sz);
				if(!vxs){
					return(-1);
				}
				DB_uRead(vxs,usz.sz,usz.off,db);
				DB_Swap_dvx(db,vxs);
				dvs_unpack(vxs);
				(*sz)=usz.sz;
				(*val)=vxs;
			}*/
			break;
		
		case _binary:{
DB_usz		usz;
void*		p;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				p=malloc(usz.sz);
				if(!p){
					return(-1);
				}
				DB_uRead(p,usz.sz,usz.off,db);
				(*sz)=usz.sz;
				(*val)=p;
			}
			break;
			
		default:
			break;
	}
	
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_KillRecord(DB_info* db, int iRec){
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
int     k=kDead_;
	(*(db->swproc))(sizeof(int),1,&k);
size_t  off=(db->dtOff)+((iRec*db->recSz));
	DB_fWrite(&k,sizeof(int),off,db);
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_UnkillRecord(DB_info	*db, int iRec){
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
int     k=kLive_;
	(*(db->swproc))(sizeof(int),1,&k);
size_t	off=(db->dtOff)+((iRec*db->recSz));
	DB_fWrite(&k,sizeof(int),off,db);
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_GetRecordState(DB_info	*db, int iRec){
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
size_t	off=(db->dtOff)+((iRec*db->recSz));
int     k;
	DB_fRead(&k,sizeof(int),off,db);
	(*(db->swproc))(sizeof(int),1,&k);	
	return(k);
}

//----------------------------------------------------------------------------
// Faudrait ptet écrire la taille
// endian needed
//------------
int	DB_SetBufferSize(DB_info	*db, int sz){
void	*bf1,*bf2;
		
	if(sz<=0){
		return(-1);
	}
	
	bf1=malloc(sz);
	if(!bf1){
		return(-2);
	}
	bf2=malloc(sz);
	if(!bf2){
		free(bf1);
		return(-3);
	}
	
	db->rBuffSz=sz;
	
	free(db->fBuff);
	free(db->uBuff);
	
	db->fBuff=bf1;
	db->uBuff=bf2;

	DB_fBuffLoad(db->fPos,db);
	DB_uBuffLoad(db->uPos,db);
	
    if(DB_writeHeader(db)){
        return(-4);
    }
// FSEEK A MODIFIER
	fseek(db->ff,db->fPos,SEEK_SET);

	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_Pack(DB_info	*db){
FILE*	fp;
long	i,j;
int     n,newn=0,count;
size_t  off,pos;
DB_usz	usz;
void*	data;

// Pack des enregistrements formatés
fprintf(stderr,"----------------------------------------\n");
fprintf(stderr,"DB_Pack : Packing formated data\n");
fprintf(stderr,"----------------------------------------\n");

	fp=tmpfile();
	if(!fp){
fprintf(stderr,"DB_Pack : ERROR : tmpfile failed\n");
		return(-1);
	}
	
    n=DB_writeHeaderInFile(db,fp);
    if(n){
fprintf(stderr,"DB_Pack : DB_writeHeaderInFile failed %d\n",n);
        return(-2);
    }

	count=0;
	for(i=0;i<db->nRec;i++){
		if(i%500==0){
fprintf(stderr,"DB_Pack : %ld of *.fmt processed (%.0f\%%)\n",i,(double)(i+1)/db->nRec*100.0);
		}
		
		if(DB_GetRecordState(db,i)==kDead_){
			count++;
			continue;
		}
		
// FSEEK A MODIFIER
		fseek(db->ff,db->dtOff+i*db->recSz,SEEK_SET);
		fread(db->emptyBuff,db->recSz,1,db->ff);
// FSEEK A MODIFIER
		fseek(fp,db->dtOff+newn*db->recSz,SEEK_SET);
		fwrite(db->emptyBuff,db->recSz,1,fp);
		
		newn++;
	}
	
// on remet le buffer d'init à 0
	memset(db->emptyBuff,0,db->recSz);	
	
	db->nRec=newn;
    n=DB_writeHeaderInFile(db,fp);
    if(n){
fprintf(stderr,"DB_Pack : DB_writeHeaderInFile failed %d\n",n);
        return(-2);
    }

	filecpy(db->ff,fp);		
	fclose(fp);
	
fprintf(stderr,"DB_Pack : %d records deleted, %d records alive\n",count,newn);

	DB_fBuffLoad(db->dtOff,db);

	if(!db->fu){// si pas fu on arrête la
		return(0);
	}

// Pack des enregistrements non formatés
fprintf(stderr,"----------------------------------------\n");
fprintf(stderr,"DB_Pack : Packing unformated data\n");
fprintf(stderr,"----------------------------------------\n");

	DB_uBuffLoad(sizeof(int),db);

	fp=tmpfile();
	if(!fp){
fprintf(stderr,"DB_Pack : ERROR : tmpfile failed\n");
		return(-1);
	}
	
// Crash flag
// FSEEK A MODIFIER
	fseek(fp,0,SEEK_SET);
	(*(db->swproc))(sizeof(int),1,&db->spy);
	fwrite(&db->spy,sizeof(int),1,fp);
	(*(db->swproc))(sizeof(int),1,&db->spy);
	pos=sizeof(int);
	
	for(i=0;i<db->nRec;i++){
		if(i%500==0){
fprintf(stderr,"DB_Pack : %ld of *.unf processed (%.0f\%%)\n",i,(double)(i+1)/db->nRec*100.0);
		}
		for(j=0;j<db->nFld;j++){
			switch(db->flds[j].sign){
				case _ivxs2:
				case _ivxs3:
				case _dvxs2:
				case _dvxs3:
				case _binary:
					off=db->dtOff+i*db->recSz+db->flds[j].off;
					DB_fRead(&usz,sizeof(DB_usz),off,db);
					(*(db->swproc))(sizeof(int),2,&usz);
					if(usz.sz<0){// On n'arrête pas sinon on perd tout
fprintf(stderr,"DB_Pack : ERROR : bad size : %d\n",usz.sz);
						usz.sz=0;
						usz.off=0;
						DB_fWrite(&usz,sizeof(DB_usz),off,db);
					}
					else if(usz.sz>0){
						data=malloc(usz.sz);
						if(!data){// On n'arrête pas sinon on perd tout
fprintf(stderr,"DB_Pack : ERROR : malloc failed for %d\n",usz.sz);
							usz.sz=0;
							usz.off=0;
							DB_fWrite(&usz,sizeof(DB_usz),off,db);
						}
						else{
							DB_uRead(data,usz.sz,usz.off,db);
// FSEEK A MODIFIER
							fseek(fp,pos,SEEK_SET);
							fwrite(data,usz.sz,1,fp);						
							usz.off=pos;
							(*(db->swproc))(sizeof(int),2,&usz);
							DB_fWrite(&usz,sizeof(DB_usz),off,db);
							(*(db->swproc))(sizeof(int),2,&usz);
							pos+=usz.sz;
							free(data);
						}
					}
					break;
					
				default:
					break;
			}
			
		}
	}

	filecpy(db->fu,fp);
	fclose(fp);

	DB_fBuffLoad(db->dtOff,db);
	DB_uBuffLoad(sizeof(int),db);

	return(0);
}

