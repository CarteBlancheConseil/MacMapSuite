//----------------------------------------------------------------------------
// File : mem_db.c
// Project : MacMapSuite
// Purpose : C source file : DB_Lib memory storage utils
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

#include <string.h>
#include <time.h>

#include "vx.h"
#include "vx_utils.h"
#include "vx_allocation.h"

#include "mem_db.h"
#include "db_types.h"
#include "db_utils.h"
#include "valconv.h"

//----------------------------------------------------------------------------
#pragma mark =>Utils
//----------------------------------------------------------------------------
// Utilities
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Conversion from text
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur
// variable ne peut pas être converti
//------------
static void convertFromString(char* s, MEM_fld* f, void* v){
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
// IO utilities
//----------------------------------------------------------------------------
#pragma mark -
#pragma mark =>IO utilities

//----------------------------------------------------------------------------
// 
//------------
static size_t MEM_read(void *p, long f, long r, MEM_info* db){
    memmove(p,db->flds[f].data+r*db->flds[f].sz,db->flds[f].sz);
//    memmove(p,(void*)(((int)db->flds[f].data)+(r*db->flds[f].sz)),db->flds[f].sz);
	return(db->flds[f].sz);
}

//----------------------------------------------------------------------------
// 
//------------
static size_t MEM_write(void *p, long f, long r, MEM_info* db){
    memmove(db->flds[f].data+r*db->flds[f].sz,p,db->flds[f].sz);
//    memmove((void*)(((int)db->flds[f].data)+(r*db->flds[f].sz)),p,db->flds[f].sz);
	return(db->flds[f].sz);
}

//----------------------------------------------------------------------------
// 
//------------
static	int	MEM_CalcFieldSize(MEM_fld	*f){
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
			return(sizeof(void*));
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
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur 
// variable ne peut pas être converti
//------------
static int MEM_ChgFieldInStruct(	MEM_info* db,
									int idx,
									const char* nam,
									int sign,
									int length,
									int decs){
int		/*szfrom,*/szto;
MEM_fld	f;

// Modifs sur le champ	
	f=db->flds[idx];
//	szfrom=f.sz;
	f.sign=sign;
	f.length=length;
	f.decs=decs;
	szto=MEM_CalcFieldSize(&f);
	f=db->flds[idx];
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
	
	return(0);
}

//----------------------------------------------------------------------------
// Pour l'instant, on considère qu'un champ structure ou un champ à longueur 
// variable ne peut pas être converti
//------------
static int MEM_ChgFieldWithRecord(	MEM_info* db,
									int idx,
									const char* nam,
									int sign,
									int length,
									int decs){
	
long	i;
size_t	nsz=MEM_CalcFieldSize(&db->flds[idx]);
void*	data=malloc(db->nRec*nsz);

	if(!data){
		return(-1);
	}
	
	for(i=0;i<db->nRec;i++){
		switch(db->flds[idx].sign){
			case _bool:
			case _int:
				intToX(		sign,
							0,
							*(int*)(db->flds[idx].data+i*db->flds[idx].sz),
							(void*)(data+i*nsz));
				break;
			case _double:
				doubleToX(	sign,
							0,
                            *(double*)(db->flds[idx].data+i*db->flds[idx].sz),
                            (void*)(data+i*nsz));
                /**(double*)(((int)db->flds[idx].data)+i*db->flds[idx].sz),
                (void*)(((int)data)+i*nsz));*/
				break;
			case _date:
				dateToX(	sign,
							0,
                            *(double*)(db->flds[idx].data+i*db->flds[idx].sz),
                            (void*)(data+i*nsz));
                /**(double*)(((int)db->flds[idx].data)+i*db->flds[idx].sz),
                (void*)(((int)data)+i*nsz));*/
				break;
			case _time:
				timeToX(	sign,
							0,
                            *(double*)(db->flds[idx].data+i*db->flds[idx].sz),
                            (void*)(data+i*nsz));
                /**(double*)(((int)db->flds[idx].data)+i*db->flds[idx].sz),
                (void*)(((int)data)+i*nsz));*/
				break;
			case _char:
				charToX(	sign,
							0,
                            (char*)(db->flds[idx].data+i*db->flds[idx].sz),
                            (void*)(data+i*nsz));
                /*(char*)(((int)db->flds[idx].data)+i*db->flds[idx].sz),
                (void*)(((int)data)+i*nsz));*/
				break;
		}	
	}
	
	return(0);
}

//----------------------------------------------------------------------------
// Public APIs
//----------------------------------------------------------------------------
#pragma mark -
#pragma mark =>Public APIs
//----------------------------------------------------------------------------
// creating
//------------
MEM_info*	MEM_Create(void){
MEM_info*	db;
	
	db=(MEM_info*)malloc(sizeof(MEM_info));
	if(db){
		db->nRec=0;
		db->nFld=0;
	}
	return(db);
}

//----------------------------------------------------------------------------
// endian ok
//------------
void MEM_Close(MEM_info* db){
long	i;
	for(i=db->nFld-1;i>=0;i--){
		MEM_RmvField(&db,i);
	}
	free(db);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_NamedFieldIdx(MEM_info	*db, const char *nam, int *idx){		
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
// 
//------------
int	MEM_FieldSign(MEM_info	*db,	int	iFld,	int	*sign){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*sign)=db->flds[iFld].sign;
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_FieldName(MEM_info	*db,	int	iFld,	char	*name){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	strcpy(name,db->flds[iFld].nam);
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_FieldSize(MEM_info	*db,	int	iFld,	int	*sz){		
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
		case _ivxs2:
		case _ivxs3:
		case _dvxs2:
		case _dvxs3:
			(*sz)=sizeof(void*);
			break;
	
		default:
			return(-1);
			break;

	}	
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_FieldLength(MEM_info* db, int iFld, int* len){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*len)=db->flds[iFld].length;
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_FieldDecimals(MEM_info	*db,	int	iFld,	int	*decs){		
	if((iFld<0)||(iFld>=db->nFld)){
		return(-1);
	}
	(*decs)=db->flds[iFld].decs;
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_CountFields(MEM_info	*db){		
	return(db->nFld);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_CountRecords(MEM_info	*db){		
	return(db->nRec);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_AddField(MEM_info	**db, const char*	nam, int sign, int length, int decs){	
	if(!SignTest(sign)){
		return(-1);
	}
	
int	n;
	if(MEM_NamedFieldIdx((*db),nam,&n)>-1){
		return(-2);
	}
	n=sizeof(MEM_info)+(((*db)->nFld+1)*sizeof(MEM_fld));
	(*db)=(MEM_info*)realloc((*db),n);
	if(!(*db)){
		return(-3);
	}
	
	(*db)->flds[(*db)->nFld].sign=sign;
	strcpy((*db)->flds[(*db)->nFld].nam,nam);
	(*db)->flds[(*db)->nFld].length=length;
	(*db)->flds[(*db)->nFld].decs=decs;
	(*db)->flds[(*db)->nFld].sz=MEM_CalcFieldSize(&(*db)->flds[(*db)->nFld]);
	
	if((*db)->nRec>0){
		(*db)->flds[(*db)->nFld].data=malloc((*db)->nRec*(*db)->flds[(*db)->nFld].sz);
		memset((*db)->flds[(*db)->nFld].data,0,(*db)->nRec*(*db)->flds[(*db)->nFld].sz);
	}
	else{
		(*db)->flds[(*db)->nFld].data=NULL;
	}
	
	(*db)->nFld++;

	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_ChgField(MEM_info	*db, int idx, const char* nam, int sign, int length, int decs){		
int	x,index,status;

	if((idx<0)||(idx>=db->nFld)){
		return(-1);
	}
	if(!SignTest(sign)){
		return(-2);
	}
	x=MEM_NamedFieldIdx(db,nam,&index);
	if((x==0)&&(index!=idx)){
		return(-3);
	}
	
	if(db->nRec==0){
		status=MEM_ChgFieldInStruct(db,idx,nam,sign,length,decs);
		if(status){
			return(status);
		}
	}
	else{
		status=MEM_ChgFieldWithRecord(db,idx,nam,sign,length,decs);
		if(status){
			return(status);
		}
		status=MEM_ChgFieldInStruct(db,idx,nam,sign,length,decs);
		if(status){
			return(status);
		}
	}
	return(0);
}

//----------------------------------------------------------------------------
// no endian needed
//------------
int	MEM_RmvField(MEM_info **db, int idx){
	if((idx<0)||(idx>=(*db)->nFld)){
		return(-2);
	}
	
	if((*db)->flds[idx].data){
		free((*db)->flds[idx].data);
	}
		
	if(idx<(*db)->nFld-1){
		memmove(&(*db)->flds[idx],&(*db)->flds[idx+1],((*db)->nFld-((long)idx+1))*sizeof(MEM_fld));
	}
	
int	n=sizeof(MEM_info)+(((*db)->nFld-1)*sizeof(MEM_fld));
	(*db)=(MEM_info*)realloc((*db),n);
	if(!(*db)){
		return(-3);
	}	

	(*db)->nFld--;
	
	return(0);
}


//----------------------------------------------------------------------------
// endian ok
//------------
int	MEM_ReadVal(MEM_info* db, int iRec, int iFld, void* val){	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-2);
	}
	MEM_read(val,iFld,iRec,db);
	return(0);
}

//----------------------------------------------------------------------------
// endian ok
//------------
int	MEM_WriteVal(MEM_info* db, int iRec, int iFld, void* val){	
	if((iRec>db->nRec)||(iRec<0)){
		return(-2);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-3);
	}
	if(iRec==db->nRec){
int		i;
void*	buff;
		for(i=0;i<db->nFld;i++){
			if(db->flds[i].data){
				buff=realloc(db->flds[i].data,db->flds[i].sz*(db->nRec+1));
			}
			else{
				buff=malloc(db->flds[i].sz);
			}
			if(!buff){
				return(-4);
			}
			db->flds[i].data=buff;
			
//            memset((void*)(((int)db->flds[i].data)+((db->nRec)*db->flds[i].sz)),0,db->flds[i].sz);
            memset(db->flds[i].data+db->nRec*db->flds[i].sz,0,db->flds[i].sz);
		}
		db->nRec++;
	}
	MEM_write(val,iFld,iRec,db);
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_ReadStringVal(MEM_info	*db, int iRec, int iFld, char *val){
int		r;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-1);
	}
	switch(db->flds[iFld].sign){
		case _char:
			r=MEM_ReadVal(db,iRec,iFld,val);
			break;
		case _bool:
		case _int:{
int	iVal;
			r=MEM_ReadVal(db,iRec,iFld,&iVal);
			if(!r){
				sprintf(val,"%d",iVal);
			}
			}
			break;
		case _double:{
double	dVal;
				
			r=MEM_ReadVal(db,iRec,iFld,&dVal);
			if(!r){
				sprintf(val,"%.*f",db->flds[iFld].decs,dVal);		
			}
			}
			break;
		case _date:{
time_t	t;
double	dVal;
				
			r=MEM_ReadVal(db,iRec,iFld,&dVal);
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
int	MEM_WriteStringVal(MEM_info* db, int iRec, int iFld, char *val){
int	iVal,dVal,r;
	
	if((iRec>db->nRec)||(iRec<0)){
		return(-1);
	}
	if((iFld>=db->nFld)||(iFld<0)){
		return(-1);
	}
	
	switch(db->flds[iFld].sign){
		case _char:
			r=MEM_WriteVal(db,iRec,iFld,val);
			break;
		case _bool:
		case _int:
			convertFromString(val,&db->flds[iFld],&iVal);
			r=MEM_WriteVal(db,iRec,iFld,&iVal);
			break;
		case _double:
			convertFromString(val,&db->flds[iFld],&dVal);
			r=MEM_WriteVal(db,iRec,iFld,&dVal);
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
int	MEM_AllocVal(MEM_info* db, int iRec, int iFld, int* sz, void** val){
/*int	off;
	
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
MEM_usz		usz;
ivertices*	vxs;
				
				MEM_fRead(&usz,sizeof(MEM_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				vxs=(ivertices*)malloc(usz.sz);
				if(!vxs){
					return(-1);
				}
				MEM_uRead(vxs,usz.sz,usz.off,db);
				MEM_Swap_ivx(db,vxs);
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
				MEM_fRead((*val),size,off,db);
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
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(db->flds[iFld].sz,1,val);
			break;
		
		case _ivx2:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),2,val);
			break;
		case _ivx3:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),3,val);
			break;
		case _ivxrect:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),4,val);
			break;
		case _ivxcube:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(int),6,val);
			break;
		case _dvx2:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),2,val);
			break;
		case _dvx3:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),3,val);
			break;
		case _dvxrect:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead((*val),(*sz),off,db);
			(*(db->swproc))(sizeof(double),4,val);
			break;
		case _dvxcube:
			(*val)=malloc(db->flds[iFld].sz);
			if(!(*val)){
				return(-1);
			}
			(*sz)=db->flds[iFld].sz;
			MEM_fRead(val,db->flds[iFld].sz,off,db);
			(*(db->swproc))(sizeof(double),6,val);
			break;

		case _dvxs2:
		case _dvxs3:{
MEM_usz		usz;
dvertices*	vxs;
				
				MEM_fRead(&usz,sizeof(MEM_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				vxs=(dvertices*)malloc(usz.sz);
				if(!vxs){
					return(-1);
				}
				MEM_uRead(vxs,usz.sz,usz.off,db);
				MEM_Swap_dvx(db,vxs);
				dvs_unpack(vxs);
				(*sz)=usz.sz;
				(*val)=vxs;
			}
			break;
					
		case _binary:{
MEM_usz		usz;
void*		p;
				
				MEM_fRead(&usz,sizeof(MEM_usz),off,db);
				(*(db->swproc))(sizeof(int),2,&usz);
				if(usz.sz==0){
					return(0);
				}
				p=malloc(usz.sz);
				if(!p){
					return(-1);
				}
				MEM_uRead(p,usz.sz,usz.off,db);
				(*sz)=usz.sz;
				(*val)=p;
			}
			break;
			
		default:
			break;
	}
	*/
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_KillRecord(MEM_info* db, int iRec){
/*int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	k=kDead_;
	(*(db->swproc))(sizeof(int),1,&k);
	off=(db->dtOff)+((iRec*db->recSz));
	MEM_fWrite(&k,sizeof(int),off,db);*/
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_UnkillRecord(MEM_info	*db, int iRec){
/*int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	k=kLive_;
	(*(db->swproc))(sizeof(int),1,&k);
	off=(db->dtOff)+((iRec*db->recSz));
	MEM_fWrite(&k,sizeof(int),off,db);*/
	return(0);
}

//----------------------------------------------------------------------------
// 
//------------
int	MEM_GetRecordState(MEM_info	*db, int iRec){
/*int	k,off;
	
	if((iRec>=db->nRec)||(iRec<0)){
		return(-1);
	}
	off=(db->dtOff)+((iRec*db->recSz));
	MEM_fRead(&k,sizeof(int),off,db);
	(*(db->swproc))(sizeof(int),1,&k);	
	return(k);*/
	return(0);
}
