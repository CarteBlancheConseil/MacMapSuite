//----------------------------------------------------------------------------
// File : C_Utils.h
// Project : MacMapSuite
// Purpose : C cource file : C utils
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
// 01/01/2002 creation.
//----------------------------------------------------------------------------

#include "C_Utils.h"
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------
//
// ------------
static const char	_LWR[]={'Ä','Å','á','à','â','ä','ã','å','À','Ã','Â','Á','É','é','è','ê','ë','Ê','Ë','È','í','ì','î','ï','¡','Í','Î','Ï','Ì','Ö','ó','ò','ô','ö','õ','Ø','Õ','Ó','Ô','Ò','Ü','ú','ù','û','ü','Ú','Û','Ù','ÿ','Ÿ','Ç','ç','Ñ','ñ'};
static const char	_UPR[]={'A','A','A','A','A','A','A','A','A','A','A','A','E','E','E','E','E','E','E','E','I','I','I','I','I','I','I','I','I','O','O','O','O','O','O','O','O','O','O','O','O','U','U','U','U','U','U','U','Y','Y','C','C','N','N'};
static int			_CMX=53;

// ---------------------------------------------------------------------------
// 
// ------------
int mtoupper(int c){
int	i;
	if(/*c<128*/c>0){
		if(!isupper(c)){
			c=toupper(c);
		}
	}
	else{
		for(i=0;i<_CMX;i++){
			if(c==_LWR[i]){
				c=_UPR[i];
			}
		}
	}
	return(c);	
}

// ---------------------------------------------------------------------------
// 
// ------------
int mtolower(int c){
int	i;
	if(/*c<128*/c>0){
		if(!islower(c)){
			c=tolower(c);
		}
	}
	else{
		for(i=0;i<_CMX;i++){
			if(c==_UPR[i]){
				c=_LWR[i];
			}
		}
	}
	return(c);	
}

// ---------------------------------------------------------------------------
// 
// ------------
void strupperfcw(char* str){
int	i,first=1;
	for(i=0;i<strlen(str);i++){
		switch(str[i]){
			case ' ':
			case '-':
			case '\'':
			case '(':
				first=1;
				break;
			default:
				if(first){
					str[i]=mtoupper(str[i]);
				}
				else{
					str[i]=mtolower(str[i]);
				}
				first=0;
				break;
		}
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void strupperfcs(char* str){
int	i,first=1;
	for(i=0;i<strlen(str);i++){
		switch(str[i]){
			case ' ':
			case '-':
			case '\'':
			case '(':
				first=1;
				break;
			default:
				if(first){
					str[i]=mtoupper(str[i]);
				}
				else{
					str[i]=mtolower(str[i]);
				}
				first=0;
				break;
		}
		if(!first){
			break;
		}
	}
}

// ---------------------------------------------------------------------------
//
// ------------
void strupper(char* str){
int i,n=strlen(str);
	
	for(i=0;i<n;i++){
		str[i]=mtoupper(str[i]);
	}
}

// ---------------------------------------------------------------------------
//
// ------------
void strlower(char* str){
int i,n=strlen(str);
	
	for(i=0;i<n;i++){
		str[i]=mtolower(str[i]);
	}
}

// ---------------------------------------------------------------------------
//
// ------------
void strinsert(char* str, const char* ins, int k){
int n=strlen(str),ni=strlen(ins);
	if((k<0)||(k>n)){
		return;
	}
	memmove(&str[k+ni],&str[k],(n-k)+1);
	memmove(&str[k],ins,ni);
}

// ---------------------------------------------------------------------------
//
// ------------
void strrep(char* str, const char* pat, const char* rp){
	if(strstr(rp,pat)){
fprintf(stderr,"WARNING ! strrep : pattern (%s) found in replacement (%s)\n",pat,rp);
		return;
	}
int		k=0,np=strlen(pat),nt;
char*	p=(char*)strstr(str,pat);
	
	while(p){
		k++;
		if(k>256){
			break;
		}
		nt=strlen(p);
		memmove(&p[0],&p[np],(nt-np)+1);
		strinsert(p,rp,0);
		p=(char*)strstr(p,pat);
	}
}

// ---------------------------------------------------------------------------
//
// ------------
int strfloat(char* str, int sign){
int		i,n=strlen(str);
int		b=1;
	
	for(i=0;i<n;i++){
		if(str[i]>57||str[i]<48){
			switch(str[i]){
				case '.':
					break;
				case ',':
					str[i]='.';
					break;
				case '-':
				case '+':
					if(!sign){
						b=0;
					}
					break;
				default:
					b=0;
					break;
			}
		}
	}
	return(b);
}

// ---------------------------------------------------------------------------
//
// ------------
int strint(char* str, int sign){
int		i,n=strlen(str);
int		b=1;
	
	for(i=0;i<n;i++){
		if(str[i]>57||str[i]<48){
			if(sign){
				if((str[i]=='-')||(str[i]=='+')){
					continue;
				}
			}
			b=0;
		}
	}
	return(b);
}

// ---------------------------------------------------------------------------
//
// ------------
char* strcatr(char* a, char* b){
char*	res;
	if(a==NULL){
		res=strdup(b);
	}
	else{
int	na=strlen(a);
int	nb=strlen(b);
		a=realloc(a,na+nb+1);
		strcpy(&a[na],b);
	}
	return(a);
}

// ---------------------------------------------------------------------------
//
// ------------
void relpath(char* base, char* abs, char* rel){
int		i;
int		nb=(int)strlen(base);
int		na=(int)strlen(abs);
int		n=(na<nb)?na:nb;
char	*pb=base,*pa=abs;
int		sa=0,sb=0;

	for(i=0;i<n;i++){
//fprintf(stderr,"%d\n",i);
		if(base[i]!=abs[i]){
//			if(base[i-1]=='/'){
//				pb=&base[i];
//				pa=&abs[i];
//			}
//			else{
//				pb=&base[i-1];
//				pa=&abs[i-1];
//			}
			for(;(i>0)&&(base[i-1]!='/');i--){
			}
			pb=&base[i];
			pa=&abs[i];
//fprintf(stderr,"arrété à %d\n",i);
			break;
		}
	}

	pb=&base[i];
	pa=&abs[i];

	na=strlen(pa);
	for(i=0;i<na;i++){
		if(pa[i]=='/'){
			sa++;
		}
	}
	
	nb=strlen(pb);
	for(i=0;i<nb;i++){
		if(pb[i]=='/'){
			sb++;
		}
	}

//fprintf(stderr,"pa=%s\n",pa);
//fprintf(stderr,"pb=%s\n",pb);
//fprintf(stderr,"sa=%d, sb=%d\n",sa,sb);
	
	strcpy(rel,"");
	for(i=1;i<=sb;i++){
		strcat(rel,"../");
	}
	strcat(rel,pa);
	
	fprintf(stderr,"base=%s\n",base);
	fprintf(stderr,"abs=%s\n",abs);
	fprintf(stderr,"rel=%s\n",rel);
}

// ---------------------------------------------------------------------------
//
// ------------
void splitPath(const char* fullPath,
               char* path,
               char* name){
char*   s=strrchr(fullPath,'/');
    if(s){
        s++;
        strcpy(name,s);
        strcpy(path,fullPath);
        s=strrchr(path,'/');
        s++;
        s[0]=0;
    }
    else{
        strcpy(path,"");
        strcpy(name,fullPath);
    }
}


// ---------------------------------------------------------------------------
//
// ------------
double matof(char* str){
	strrep(str,",",".");
	return(atof(str));
}
