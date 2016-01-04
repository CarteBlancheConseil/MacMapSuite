//----------------------------------------------------------------------------
// File : 110531db.c
// Project : MacMapSuite
// Purpose : C_source_file##EXPL_NEEDED##
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

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "vx.h"
#include "vx_allocation.h"
#include "vx_utils.h"

#include "db.h"
#include "db_types.h"
#include "db_utils.h"

//----------------------------------------------------------------------------

//#define	fcpybufsz_	65536
#define	fcpybufsz_	1048576

//----------------------------------------------------------------------------
#pragma mark =>Utils
//----------------------------------------------------------------------------
// Utilities
//----------------------------------------------------------------------------
// File duplication
//------------
static int filecpy(FILE *dst, FILE *src){
int		i,szf,szb,pos;
void	*buff;

	buff=malloc(fcpybufsz_);
	if(!buff){
fprintf(stderr,"filecpy::ERROR : malloc failed for %d\n",fcpybufsz_);
		return(-1);
	}

	fflush(src);

//	ftruncate(dst->_file,0);

	fseek(src,0,SEEK_END);
	szf=ftell(src);
	
	szb=fcpybufsz_;
		
	if(szb>szf){
		szb=szf;
	}
	
	i=0;
	do{
		fseek(src,i,SEEK_SET);
		fread(buff,szb,1,src);
		
		fseek(dst,i,SEEK_SET);
		fwrite(buff,szb,1,dst);
		
		i+=szb;
		
		pos=ftell(src);
		
		if(szb>(szf-pos)){
			szb=szf-pos;
		}
fprintf(stderr,"filecpy::%d processed (%.0f\%%)\n",i,(double)i/szf*100.0);
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
int	i;	
	for(i=from;i<db->nFld;i++){
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
void	swapword(int sz, void* word){
int				i;
unsigned char	buff;

	for(i=0;i<sz/2;i++){
		buff=((unsigned char*)word)[i];
		((unsigned char*)word)[i]=((unsigned char*)word)[sz-i-1];
		((unsigned char*)word)[sz-i-1]=buff;
	}
}

//----------------------------------------------------------------------------
// endian swap proc
//------------
static void	swap(int sz, int n, void* arr){
int	i;
    for(i=0;i<n;i++){
		swapword(sz,(void*)(((int)arr)+(i*sz)));
    }
}

//----------------------------------------------------------------------------
// no swap proc 
//------------
static void	dontswap(int sz, int n, void* arr){
}

//----------------------------------------------------------------------------
// Header swap utilities
//------------
//----------------------------------------------------------------------------
//
//------------
static void	DB_CheckEndian(DB_info* db){
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
}

//----------------------------------------------------------------------------
//
//------------
static void	DB_CheckEndianFields(DB_info* db){
int		i;
	for(i=0;i<db->nFld;i++){
//fprintf(stderr,"DB_CheckEndianFields %d\n",i);
//fflush(stderr);
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].sign));
		(*(db->swproc))(sizeof(int),1,&(db->flds[i].id));
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
static void	DB_Swap(DB_info* db, int nFld){
int		i;

	(*(db->swproc))(sizeof(int),1,&db->spy);
	(*(db->swproc))(sizeof(int),1,&db->vers);
	(*(db->swproc))(sizeof(int),1,&db->nFld);
	(*(db->swproc))(sizeof(int),1,&db->nRec);
	(*(db->swproc))(sizeof(int),1,&db->recSz);
	(*(db->swproc))(sizeof(int),1,&db->dtOff);
	(*(db->swproc))(sizeof(int),1,&db->fPos);
	(*(db->swproc))(sizeof(int),1,&db->uPos);
	(*(db->swproc))(sizeof(int),1,&db->readOnly);
	(*(db->swproc))(sizeof(int),1,&db->rBuffSz);

	for(i=0;i<nFld;i++){
		(*(db->swproc))(sizeof(int),1,&db->flds[i].sign);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].id);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].attrs);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].length);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].decs);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].sz);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].off);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].reserved0);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].reserved1);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].reserved2);
		(*(db->swproc))(sizeof(int),1,&db->flds[i].reserved3);
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
static size_t DB_fBuffLoad(int off, DB_info* db){
int	eof,sz;
	
	fseek(db->ff,0,SEEK_END);
	eof=ftell(db->ff);
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
	
	fseek(db->fu,0,SEEK_END);
	eof=ftell(db->fu);
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
		memmove(p,(void*)(((int)db->fBuff)+(off-db->fPos)),sz);
	}
	else{
// pas la peine de se casser pour les gros, on lit tout d'un coup
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
		memmove(p,(void*)(((int)db->uBuff)+(off-db->uPos)),sz);
	}
	else{
		// pas la peine de se casser pour les gros, on lit tout d'un coup
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

	fseek(db->ff,off,SEEK_SET);
	k=fwrite(p,sz,1,db->ff);
	
	fflush(db->ff);// Il lui faut un flush pour valider la dernière modif

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
	
	memmove((void*)(((int)db->fBuff)+start),p,end-start);
	return(k);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
static size_t DB_uWrite(void* p, int sz, int off, DB_info* db){
int		start,end;
size_t	k;
	
	fseek(db->fu,off,SEEK_SET);
	k=fwrite(p,sz,1,db->fu);
	
	fflush(db->fu);// Il lui faut un flush pour valider la dernière modif
	
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
	
	memmove((void*)(((int)db->uBuff)+start),p,end-start);
	return(k);
}

//----------------------------------------------------------------------------
// 
//------------
static int DB_fu(DB_info* db){
int	i;
	
	for(i=0;i<db->nFld;i++){
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
		db->dtOff=sizeof(DB_info);
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
//swapper		swp;
	
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
		
		fseek(ff,0,SEEK_SET);
		if(fread(db,sizeof(DB_info),1,ff)!=1){
fprintf(stderr,"fread db failed\n");
			break;
		}
		DB_CheckEndian(db);
		n=db->nFld;
		if(n<0){
fprintf(stderr,"bad nFld %d\n",n);
			break;
		}
		db=(DB_info*)realloc(db,sizeof(DB_info)+n*sizeof(DB_fld));
		if(!db){
fprintf(stderr,"realloc failed\n");
			break;
		}
		fseek(ff,0,SEEK_SET);
		if(fread(db,sizeof(DB_info)+n*sizeof(DB_fld),1,ff)!=1){
fprintf(stderr,"fread db failed for %d fields\n",n);
			break;
		}
		DB_CheckEndian(db);
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
fprintf(stderr,"malloc failed for rBuffSz=%d\n",db->rBuffSz);
				break;
			}
		}
		/*fu*/
		if(db->fu){
		/*fu*/
			db->uBuff=malloc(db->rBuffSz);
			if(!db->uBuff){
fprintf(stderr,"malloc failed for rBuffSz=%d\n",db->rBuffSz);
				break;
			}
		/*fu*/
		}
		/*fu*/
		
		db->emptyBuff=malloc(db->recSz);
		if(!db->emptyBuff){
fprintf(stderr,"malloc failed for recSz=%d\n",db->recSz);
			break;
		}
		memset(db->emptyBuff,0,db->recSz);

		fseek(db->ff,0,SEEK_SET);
		if(db->fu){
			fseek(db->fu,0,SEEK_SET);
		}
		
		if(!db->readOnly){
			int k=1;
			(*(db->swproc))(sizeof(int),1,&k);
			if(fwrite(&k,sizeof(int),1,db->ff)!=1){
fprintf(stderr,"fwrite failed\n");
				break;
			}
			/*fu*/
			if(db->fu){
			/*fu*/
				if(fwrite(&k,sizeof(int),1,db->fu)!=1){
fprintf(stderr,"fwrite failed\n");
					break;
				}
			/*fu*/
			}
			/*fu*/
		}
		
		DB_fBuffLoad(db->dtOff,db);
		/*fu*/
		if(db->fu){
		/*fu*/
			DB_uBuffLoad(sizeof(int),db);
		/*fu*/
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
	
	(*db)->flds[(*db)->nFld].id=0;//
	
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
		return(n);
	}
	n=sizeof(DB_info)+(((*db)->nFld)*sizeof(DB_fld));
	fseek((*db)->ff,0,SEEK_SET);
int	bkn=(*db)->nFld;
	DB_Swap(*db,bkn);
	if(fwrite((*db),n,1,(*db)->ff)!=1){
		DB_Swap(*db,bkn);
		return(-4);
	}
	DB_Swap(*db,bkn);
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
int		i,n,oldsz,oldoff;
char	tn[FILENAME_MAX];
//long long		i;

	tmpnam(tn);
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
	
	n=sizeof(DB_info)+(((*db)->nFld)*sizeof(DB_fld));
	fseek(fp,0,SEEK_SET);
int	bkn=(*db)->nFld;
	DB_Swap(*db,bkn);
	if(fwrite((*db),n,1,fp)!=1){
		DB_Swap(*db,bkn);
		return(-4);
	}
	DB_Swap(*db,bkn);
		
	for(i=0;i<(*db)->nRec;i++){
		fseek((*db)->ff,oldoff+i*oldsz,SEEK_SET);
		fread((*db)->emptyBuff,oldsz,1,(*db)->ff);
		
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
	
	n=sizeof(DB_info)+(((*db)->nFld)*sizeof(DB_fld));
	fseek((*db)->ff,0,SEEK_SET);
int	bkn=(*db)->nFld;
	DB_Swap(*db,bkn);
	if(fwrite((*db),n,1,(*db)->ff)!=1){
		DB_Swap(*db,bkn);
		return(-4);
	}
	DB_Swap(*db,bkn);
	return((*db)->nFld);
}

//----------------------------------------------------------------------------
// endian ok
//------------
static int DB_RmvFieldWithRec(DB_info** db, int idx){
FILE*	fp;
int		i,n,oldsz,oldoff,sz1,sz2,o2;
char	tn[FILENAME_MAX];
char*	buff;
	
	tmpnam(tn);
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
	
	n=sizeof(DB_info)+(((*db)->nFld)*sizeof(DB_fld));
	fseek(fp,0,SEEK_SET);
int	bkn=(*db)->nFld;
	DB_Swap(*db,bkn);
	if(fwrite((*db),n,1,fp)!=1){
		DB_Swap(*db,bkn);
		return(-14);
	}
	DB_Swap(*db,bkn);
	
	for(i=0;i<(*db)->nRec;i++){
		fseek((*db)->ff,oldoff+i*oldsz,SEEK_SET);
		fread(buff,oldsz,1,(*db)->ff);
		
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
	
	n=sizeof(DB_info)+((db->nFld)*sizeof(DB_fld));
	fseek(db->ff,0,SEEK_SET);
int	bkn=db->nFld;
	DB_Swap(db,bkn);
	if(fwrite(db,n,1,db->ff)!=1){
		DB_Swap(db,bkn);
		return(-4);
	}
	DB_Swap(db,bkn);
	
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
int		i,n,oldsz,oldoff,sz1,sz2,o2,oldfsign,oldfsz;
char	tn[FILENAME_MAX];
char	*buff;
void*	bf;
DB_fld	f;
	
	tmpnam(tn);
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
	
	n=sizeof(DB_info)+(db->nFld*sizeof(DB_fld));
	fseek(fp,0,SEEK_SET);
int	bkn=db->nFld;
	DB_Swap(db,bkn);
	if(fwrite(db,n,1,fp)!=1){
		DB_Swap(db,bkn);
fprintf(stderr,"DB_ChgFieldWithRec error -4\n");
fflush(stderr);
		return(-4);
	}
	DB_Swap(db,bkn);
	
	/**/
	if((sign==oldfsign)&&(db->flds[idx].sz==oldfsz)){
		free(buff);
		free(bf);
		return(0);
	}
	/**/
	
fprintf(stderr,"Datas conversion\n");
fflush(stderr);

	for(i=0;i<db->nRec;i++){
		fseek(db->ff,oldoff+i*oldsz,SEEK_SET);
		fread(buff,oldsz,1,db->ff);
		
		fseek(fp,db->dtOff+i*db->recSz,SEEK_SET);
		
		fwrite(buff,sz1,1,fp);
		
		//convertToString(buff+sz1,&f,tmp);
		//convertFromString(tmp,&db->flds[idx],bf);
		
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

fprintf(stderr,"Datas conversion passed\n");
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
			break;
		}
				
		db->spy=1;
		
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.fmt",fName);
    
		fp=fopen(fullName,"wb");
		if(!fp){
			break;
		}
		fwrite(&db->spy,sizeof(int),1,fp);
		fclose(fp);
			
		fp=fopen(fullName,"rb+");
		free(fullName);
		if(!fp){
			break;
		}
				
		db->ff=fp;		
		
		fseek(db->ff,0,SEEK_SET);
		fwrite(db,sizeof(DB_info),1,db->ff);
		
		fullName=(char*)malloc(strlen(fName)+5);
		sprintf(fullName,"%s.unf",fName);
    
		fp=fopen(fullName,"wb");
		if(!fp){
			break;
		}
		fwrite(&db->spy,sizeof(int),1,fp);
		fclose(fp);
			
		fp=fopen(fullName,"rb+");
		free(fullName);
		if(!fp){
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
		/*fu*/
			DB_uBuffLoad(sizeof(int),db);
		/*fu*/
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

//	TEST FOPEN_MAX
//	db->ff=freopen("toto","rb+",db->ff);
//	freopen(db->fu,"rb+");

	(*(db->swproc))(sizeof(int),1,&db->spy);
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
int	i;
	for(i=0;i<db->nFld;i++){
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
		return(-1);
	}
//fprintf(stderr,"%.4s\n",&sign);
	if(!SignTest(sign)){
		return(-1);
	}
	if((*db)->nRec==0){
		status=DB_AddFieldNoRec(db,nam,sign,length,decs);
	}
	else{
		status=DB_AddFieldWithRec(db,nam,sign,length,decs);
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
int	off;
	
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
DB_usz		usz;
ivertices*	vxs;
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
				vxs=(ivertices*)malloc(usz.sz);
				if(!vxs){
					(*((ivertices**)val))=NULL;
					return(-1);
				}
				DB_uRead(vxs,usz.sz,usz.off,db);
				DB_Swap_ivx(db,vxs);
				ivs_unpack(vxs);
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
int	off;
	
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
		fseek(db->ff,0,SEEK_SET);
int	bkn=db->nFld;
		DB_Swap(db,bkn);
		fwrite(db,sizeof(DB_info),1,db->ff);
		DB_Swap(db,bkn);
		memset(db->emptyBuff,0,db->recSz);
		DB_fWrite(db->emptyBuff,db->recSz,off,db);
	}
	off=(db->dtOff)+((iRec*db->recSz)+db->flds[iFld].off);
		
	switch(db->flds[iFld].sign){
		case _ivxs2:
		case _ivxs3:{
int			sz,h,v,o,bks,bkv,bko;
DB_usz		usz;
ivertices*	vxs;
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
				sz=ivssize(vxs,&h,&v,&o);
				if(sz>usz.sz){
					fseek(db->fu,0,SEEK_END);
					usz.off=ftell(db->fu);
					usz.sz=sz;
					(*(db->swproc))(sizeof(int),2,&usz);
					DB_fWrite(&usz,sizeof(DB_usz),off,db);
					(*(db->swproc))(sizeof(int),2,&usz);
				}
				DB_SwapAndBack_ivx(db,vxs,bks,bkv,bko);
				DB_uWrite(vxs,h+v,usz.off,db);
				if(o>0){
					DB_uWrite(vxs->offs,o,usz.off+h+v,db);
				}
				DB_SwapAndBack_ivx(db,vxs,bks,bkv,bko);
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
		case _dvxs3:{
int			sz,h,v,o,bks,bkv,bko;
DB_usz		usz;
dvertices*	vxs;
				/*fu*/
				if(!db->fu){
					return(-20);
				}
				/*fu*/
				vxs=(*((dvertices**)val));

				bks=vxs->sign;
				bkv=vxs->nv;
				bko=vxs->no;

				DB_fRead(&usz,sizeof(DB_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				sz=dvssize(vxs,&h,&v,&o);
				if(sz>usz.sz){
					fseek(db->fu,0,SEEK_END);
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
			}
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
					fseek(db->fu,0,SEEK_END);
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
int	iVal,dVal,r;
	
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
DB_usz		usz;
ivertices*	vxs;
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
				vxs=(ivertices*)malloc(usz.sz);
				if(!vxs){
					return(-1);
				}
				DB_uRead(vxs,usz.sz,usz.off,db);
				DB_Swap_ivx(db,vxs);
				ivs_unpack(vxs);
				(*sz)=usz.sz;
				(*val)=vxs;
			}
			break;
			
		case _char:{
				int size;
				
				size=db->flds[iFld].length+1;
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
int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	k=kDead_;
	(*(db->swproc))(sizeof(int),1,&k);
	off=(db->dtOff)+((iRec*db->recSz));
	DB_fWrite(&k,sizeof(int),off,db);
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_UnkillRecord(DB_info	*db, int iRec){
int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	k=kLive_;
	(*(db->swproc))(sizeof(int),1,&k);
	off=(db->dtOff)+((iRec*db->recSz));
	DB_fWrite(&k,sizeof(int),off,db);
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_GetRecordState(DB_info	*db, int iRec){
int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	off=(db->dtOff)+((iRec*db->recSz));
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
	
	fseek(db->ff,0,SEEK_SET);
int	bkn=db->nFld;
	DB_Swap(db,bkn);
	if(fwrite(db,sizeof(DB_info),1,db->ff)!=1){
		DB_Swap(db,bkn);
		return(-4);
	}
	DB_Swap(db,bkn);
	fseek(db->ff,db->fPos,SEEK_SET);

	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	DB_Pack(DB_info	*db){
FILE*	fp;
int		i,j,n,newn=0,bkn,off,pos,count;
char	tn[FILENAME_MAX];
DB_usz	usz;
void*	data;

// Pack des enregistrements formatés
fprintf(stderr,"----------------------------------------\n");
fprintf(stderr,"DB_Pack::Packing formated data\n");
fprintf(stderr,"----------------------------------------\n");

	tmpnam(tn);
	fp=tmpfile();
	if(!fp){
fprintf(stderr,"DB_Pack::ERROR : tmpfile failed\n");
		return(-1);
	}
	
	n=sizeof(DB_info)+(db->nFld*sizeof(DB_fld));
	fseek(fp,0,SEEK_SET);
	bkn=db->nFld;
	DB_Swap(db,bkn);
	if(fwrite(db,n,1,fp)!=1){
		DB_Swap(db,bkn);
fprintf(stderr,"DB_Pack::ERROR : fwrite failed\n");
		return(-2);
	}
	DB_Swap(db,bkn);
	
	count=0;
	for(i=0;i<db->nRec;i++){
		if(i%500==0){
fprintf(stderr,"DB_Pack::%d of *.fmt processed (%.0f\%%)\n",i,(double)(i+1)/db->nRec*100.0);
		}
		
		if(DB_GetRecordState(db,i)==kDead_){
			count++;
			continue;
		}
		
		fseek(db->ff,db->dtOff+i*db->recSz,SEEK_SET);
		fread(db->emptyBuff,db->recSz,1,db->ff);
		fseek(fp,db->dtOff+newn*db->recSz,SEEK_SET);
		fwrite(db->emptyBuff,db->recSz,1,fp);
		
		newn++;
	}
	
// on remet le buffer d'init à 0
	memset(db->emptyBuff,0,db->recSz);	
	
	db->nRec=newn;

	fseek(fp,0,SEEK_SET);
	DB_Swap(db,bkn);
	if(fwrite(db,n,1,fp)!=1){
		DB_Swap(db,bkn);
fprintf(stderr,"DB_Pack::ERROR : fwrite failed\n");
		return(-3);
	}
	DB_Swap(db,bkn);
	
	filecpy(db->ff,fp);		
	fclose(fp);
	
fprintf(stderr,"DB_Pack::%d records deleted, %d records alive\n",count,newn);

	DB_fBuffLoad(db->dtOff,db);

	if(!db->fu){// si pas fu on arrête la
		return(0);
	}

// Pack des enregistrements non formatés
fprintf(stderr,"----------------------------------------\n");
fprintf(stderr,"DB_Pack::Packing unformated data\n");
fprintf(stderr,"----------------------------------------\n");

	DB_uBuffLoad(sizeof(int),db);

	tmpnam(tn);
	fp=tmpfile();
	if(!fp){
fprintf(stderr,"DB_Pack::ERROR : tmpfile failed\n");
		return(-1);
	}
	
// Crash flag
	fseek(fp,0,SEEK_SET);
	(*(db->swproc))(sizeof(int),1,&db->spy);
	fwrite(&db->spy,sizeof(int),1,fp);
	(*(db->swproc))(sizeof(int),1,&db->spy);
	pos=sizeof(int);
	
	for(i=0;i<db->nRec;i++){
		if(i%500==0){
fprintf(stderr,"DB_Pack::%d of *.unf processed (%.0f\%%)\n",i,(double)(i+1)/db->nRec*100.0);
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
fprintf(stderr,"DB_Pack::ERROR : bad size : %d\n",usz.sz);
						usz.sz=0;
						usz.off=0;
						DB_fWrite(&usz,sizeof(DB_usz),off,db);
					}
					else if(usz.sz>0){
						data=malloc(usz.sz);
						if(!data){// On n'arrête pas sinon on perd tout
fprintf(stderr,"DB_Pack::ERROR : malloc failed for %d\n",usz.sz);
							usz.sz=0;
							usz.off=0;
							DB_fWrite(&usz,sizeof(DB_usz),off,db);
						}
						else{
							DB_uRead(data,usz.sz,usz.off,db);
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

