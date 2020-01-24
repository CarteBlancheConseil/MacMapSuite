//----------------------------------------------------------------------------
// File : bTextLoader.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Text file parser class
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
// 29/06/2006 creation.
//----------------------------------------------------------------------------

#include "bTextLoader.h"
//#include "bTrace.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTextLoader::bTextLoader(FILE* fp, bool lfh){
//_bTrace_("bTextLoader::bTextLoader",true);
	_fp=fp;

	_hsh=(acf*)malloc(sizeof(acf));
	_hsh->start=0;
	_hsh->len=0;
	_hsh->lmax=0;
	_hsh->kind=kStringUndef;
	_hsh->kfirst=kStringUndef;

	_eol=kEOLUndef;
	_cols=1;
	_lines=0;
	_hdr=false;
	_lfh=lfh;
	
	_lbsz=_sbsz=bmax;
	_lb=(char*)malloc(_lbsz);
	_sb=(char*)malloc(_sbsz);
	
	fseek(_fp,0,SEEK_SET);
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTextLoader::~bTextLoader(){
	free(_hsh);
	free(_lb);
	free(_sb);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::kind_eol(){
	return(_eol);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::nb_lines(){
	return(_lines);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::nb_columns(){
	return(_cols);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bTextLoader::has_header(){
	return(_hdr);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::column_kind(int col){
	return(_hsh[col].kind);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::column_len(int col){
	return(_hsh[col].lmax);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bTextLoader::iterate(	void* up,
							int(*proc)
							(int,int,char*,void*)){
//_bTrace_("bTextLoader::iterate",true);
int		i,j,status=0;

	for(i=0;i<_lines;i++){
		memset(_lb,0,_lbsz);
		if(!get_line()){
			break;
		}
		hash();
		for(j=0;j<_cols;j++){
			status=proc(i,j,get_text(j),up);
			if(status){
				break;
			}
		}
		if(status){
			break;
		}
	}
	fseek(_fp,0,SEEK_SET);
}

// ---------------------------------------------------------------------------
// Dummy hash
// -----------
void bTextLoader::hash(){
//_bTrace_("bTextLoader::hash",false);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bTextLoader::analyse(){
//_bTrace_("bTextLoader::analyse",false);
int		i,k;

	while(true){
		memset(_lb,0,_lbsz);
		if(!get_line()){
			break;
		}
		_lines++;
		hash();
		for(i=0;i<_cols;i++){
			k=qualify(get_text(i));
			if(_lines==1){
				_hsh[i].kfirst=k;
			}
			else{
				switch(_hsh[i].kind){
					case kStringUndef:
						_hsh[i].kind=k;
						break;
					case kStringInt:
						if(k==kStringDouble){
							_hsh[i].kind=k;
						}
						else if((k==kStringInt)		||
								(k==kStringUndef)	){
						}
						else{
							_hsh[i].kind=kStringText;
						}
						break;
					case kStringDouble:
						if(	(k==kStringDouble)	||
							(k==kStringInt)		||
							(k==kStringUndef)	){
						}
						else{
							_hsh[i].kind=kStringText;
						}
						break;
					case kStringText:
						break;
				}
			}
			if(_hsh[i].lmax<_hsh[i].len){
				_hsh[i].lmax=_hsh[i].len;
			}
		}
	}
	if(_lfh){
		for(i=0;i<_cols;i++){
			if(_hsh[i].kind!=_hsh[i].kfirst){
				_hdr=true;
			}
			/**/
			if(_hsh[i].kind==kStringUndef){
				_hsh[i].kind=kStringText;
			}
			/**/
		}
	}
	fseek(_fp,0,SEEK_SET);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bTextLoader::qualify(char* str){
//_bTrace_("bTextLoader::qualify",false);
long	n=strlen(str);
	if(n==0){
		return(kStringUndef);
	}
char*	lstr=strdup(str);
	if(lstr[n-1]==13){
		lstr[n-1]=0;
		n--;
	}
	if(lstr[n-1]==10){
		lstr[n-1]=0;
		n--;
	}
	if(n==0){
		free(lstr);
		return(kStringUndef);
	}
	
char*	cbad=NULL;
	(void)strtol(lstr,&cbad,0);
	if(cbad==(lstr+n)){
		if(	((strlen(lstr)==11)&&(lstr[0]=='-')&&(lstr[1]>'2'))	||
			((strlen(lstr)>11)&&(lstr[0]=='-'))					){
			free(lstr);
			return(kStringDouble);
		}
		if(	((strlen(lstr)==10)&&(lstr[0]!='-')&&(lstr[0]>'2'))	||
			((strlen(lstr)>10)&&(lstr[0]!='-'))					){
			free(lstr);
			return(kStringDouble);
		}
		if((strlen(lstr)>=2)&&(lstr[0]=='0')){
			free(lstr);
			return(kStringText);
		}
		free(lstr);
		return(kStringInt);
	}
	
	if(cbad[0]==','){
		cbad[0]='.';
	}
	
	cbad=NULL;
	strtod(lstr,&cbad);
	if(cbad==(lstr+n)){
		free(lstr);
		return(kStringDouble);
	}
	free(lstr);
	return(kStringText);
}

// ---------------------------------------------------------------------------
// 
// -----------
char* bTextLoader::get_text(int n){
//_bTrace_("bTextLoader::get_text",false);
char*    tmp;
	if(_hsh[n].len>=_sbsz){
		if(_hsh[n].len>=bmax){
			_sbsz+=(_hsh[n].len+2);
		}
		else{
			_sbsz+=bmax;
		}
		tmp=(char*)realloc(_sb,_sbsz);
		if(!tmp){
//_te_("realloc failed "+_sbsz);
		}
        else{
            _sb=tmp;
        }
	}
	strncpy(_sb,&_lb[_hsh[n].start],_hsh[n].len);
	_sb[_hsh[n].len]=0;
//_tm_(_sb);
	return(_sb);
}

// ---------------------------------------------------------------------------
// 
// -----------
char* bTextLoader::get_line(){
//_bTrace_("bTextLoader::get_line",false);
int		i,len;
int		bksz=_lbsz;
char*	bbuff=_lb,*out;
int		c,flg=0,comp=0;
	
	if(_eol==kEOLWin){
		fseek(_fp,1,SEEK_CUR);
	}
	while((out=fgets(bbuff,bksz,_fp))){
//_tm_("len 1");
		len=strlen(bbuff);
		if(len==0){
			_eol=kEOLUnix;
			break;
		}
		if(len<(bksz-1)){
			if(feof(_fp)){
				bbuff[len]=0;
			}
			else{
				bbuff[len-1]=0;
			}
			if(	(_eol==kEOLUndef)&&
				(!feof(_fp))	){
				c=fgetc(_fp);
				if(c==EOF){
					return(NULL);
				}
				if(c==13){
					_eol=kEOLWin;
				}
				else{
					fseek(_fp,-1,SEEK_CUR);
					_eol=kEOLUnix;
				}
			}
			if(	(_eol==kEOLWin)	||
				(_eol==kEOLUnix)	){
				break;
			}
		}		
		switch(_eol){
			case kEOLUndef:
			case kEOLMac:
				for(i=0;i<len;i++){
					if(bbuff[i]==13){	
						_eol=kEOLMac;
						fseek(_fp,(i-len)+1,SEEK_CUR);
						flg=1;
						bbuff[i]=0;
						break;
					}
				}
				if(_eol==kEOLUndef){
					comp++;
					_lbsz+=bmax;
					_lb=(char*)realloc(_lb,_lbsz);
//					if(!_lb){
//_te_("realloc failed (_eol==kEOLUndef) "+_lbsz);
//					}
					bbuff=&_lb[(_lbsz-bmax)-comp];
					bksz=bmax;
				}
				break;
			case kEOLUnix:
			case kEOLWin:
				comp++;
				_lbsz+=bmax;
				_lb=(char*)realloc(_lb,_lbsz);
//				if(!_lb){
//_te_("realloc failed (kEOLUnix/kEOLWin) "+_lbsz);
//				}
				bbuff=&_lb[(_lbsz-bmax)-comp];
				bksz=bmax;
				break;
		}
		if(flg){
			break;
		}
	}
	if(!out){
//_tm_("len 2");
		if(bbuff&&strlen(bbuff)>0){
			out=_lb;
		}
	}
//if(out){
//_tm_(out);
//}
	return(out);
}

#pragma mark -
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bDelimitedTextLoader::bDelimitedTextLoader(FILE* fp, bool lfh, char del)
					:bTextLoader(fp,lfh){
//_bTrace_("bDelimitedTextLoader::bDelimitedTextLoader",true);
	_del=del;
	analyse();
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bDelimitedTextLoader::~bDelimitedTextLoader(){
}

// ---------------------------------------------------------------------------
// 
// -----------
void bDelimitedTextLoader::hash(){
//_bTrace_("bDelimitedTextLoader::hash",false);
int		k=0;
bool	chropen=false;

	_hsh[k].start=0;
	for(int i=0;i<=strlen(_lb);i++){
		switch(_lb[i]){
			case '"':
				if(!chropen){
					_hsh[k].start++;
				}
				chropen=!chropen;
				break;
			case 0:
			case 10:
			/**/
			case 13:
			/**/
				_hsh[k].len=i-_hsh[k].start;
				break;
			default:
				if(_lb[i]==_del){
					if(!chropen){
						_hsh[k].len=i-_hsh[k].start;
						if(_lb[i-1]=='"'){
							_hsh[k].len--;
						}
						k++;
						if(k>=_cols){
							_hsh=(acf*)realloc(_hsh,sizeof(acf)*(_cols+1));
							if(!_hsh){	
								break;
							}
							_hsh[k].start=0;
							_hsh[k].len=0;
							_hsh[k].lmax=0;
							_hsh[k].kind=kStringUndef;
							_hsh[k].kfirst=kStringUndef;
							_cols++;
						}
						_hsh[k].start=i+1;
					}
				}
				break;
		}
	}
}

#pragma mark -
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTabTextLoader	::bTabTextLoader(FILE* fp, bool lfh)
				:bDelimitedTextLoader(fp,lfh,9){
//_bTrace_("bTabTextLoader::bTabTextLoader",true);
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTabTextLoader::~bTabTextLoader(){
}

#pragma mark -
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bCSVTextLoader	::bCSVTextLoader(FILE* fp, bool lfh)
				:bDelimitedTextLoader(fp,lfh,';'){
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bCSVTextLoader::~bCSVTextLoader(){
}
