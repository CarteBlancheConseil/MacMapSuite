//----------------------------------------------------------------------------
// File : bStdFile.cpp
// Project : MacMapSuite
// Purpose : CPP source file : File class utils
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
// 07/02/2005 creation.
//----------------------------------------------------------------------------

#include "bStdFile.h"
#include "mms_config.h"
#if _DEVELOPMENT_
#include "bTrace.h"
#endif

#pragma mark ->bFile
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bFile::bFile(){
	_status=noErr;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bFile::~bFile(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int bFile::status(){
	return(_status);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bFile::mount(char** buffer, int* sz){
	(*buffer)=NULL;
	(*sz)=0;
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bFile::next_line(char* buffer, int len_max){
	buffer[0]=0;
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bFile::write(const char* fmt){
}

// ---------------------------------------------------------------------------
// 
// -----------
void bFile::write(void* buffer, int sz){
}

#pragma mark ->bStdFile
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdFile::bStdFile(const char* path, const char* perm)
		:bFile(){
#if _DEVELOPMENT_
_bTrace_("bStdFile::bStdFile(char* path, char* perm)",false);
#endif
	_fp=NULL;
	_close_it=true;
	_path[0]=0;
	
	check_path(path);
	strcpy(_perm,perm);

	_fp=fopen(_path,_perm);
	if(!_fp){
#if _DEVELOPMENT_
_te_("fopen failed for "+_path+" with permission "+_perm+" (ERRNO == "+errno+")");
#endif
		_status=errno;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdFile::~bStdFile(){
	if(_fp&&_close_it){
		fclose(_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::mount(char** buffer, int* sz){
	(*buffer)=NULL;
	if(!_fp){
		return;
	}
	fseek(_fp,0,SEEK_END);
	(*sz)=ftell(_fp);
	fseek(_fp,0,SEEK_SET);
	(*buffer)=(char*)malloc(*sz);
	if(!(*buffer)){
		return;
	}
	fread((*buffer),(*sz),1,_fp);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdFile::next_line(char* buffer, int len_max){
	buffer[0]=0;
	if(!_fp){
		return(false);
	}
char* r;	
	while(true){
		buffer[0]=0;
		r=fgets(buffer,len_max,_fp);
		if(r==NULL){
			break;
		}
		if(strlen(buffer)>0){
			if((buffer[strlen(buffer)-1]=='\n')||(buffer[strlen(buffer)-1]=='\r')){
				buffer[strlen(buffer)-1]=0;
			}
			else{
				buffer[strlen(buffer)]=0;
			}
			break;
		}
	}
	return(r!=NULL);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::write(const char* fmt){
	if(_fp){
		fprintf(_fp,"%s",fmt);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::write(void* buffer, int sz){
	if(_fp){
		fwrite(buffer,sz,1,_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::change_name(const char* path){
	if(!_fp){
		return;
	}	
char	old[FILENAME_MAX];
	strcpy(old,_path);	
	check_path(path);
	if(rename(old,_path)){
		_status=-4;
		strcpy(_path,old);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::copy(bStdFile& fin){
char*	buff;
int		sz;
	
	fin.mount(&buff,&sz);
	if(!buff){
		return;
	}
	fwrite(buff,sz,1,_fp);
	free(buff);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::erase(){
	if(_fp){
		fclose(_fp);
		_fp=NULL;
		if(remove(_path)){
			_status=-5;
			_fp=fopen(_path,_perm);
		}
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
FILE* bStdFile::entity(){
	return(_fp);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::set_close(bool b){
	_close_it=b;
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdFile::check_path(const char* path){
	strcpy(_path,path);
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,_path,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,_path,FILENAME_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
}

#pragma mark ->bMemFile
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bMemFile::bMemFile()
		:bFile(){
	_buffer=NULL;
	_sz=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bMemFile::~bMemFile(){
	if(_buffer){
		free(_buffer);
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bMemFile::mount(char** buffer, int* sz){
	*buffer=(char*)malloc(_sz);
	memmove(*buffer,_buffer,_sz);
	*sz=_sz;
}

// ---------------------------------------------------------------------------
// 
// -----------
void bMemFile::write(const char* fmt){
	strcpy(_ln,fmt);
	write((void*)_ln,strlen(_ln));
}

// ---------------------------------------------------------------------------
// 
// -----------
void bMemFile::write(void* buffer, int sz){
	if(sz<=0){
		return;
	}
	if(!_buffer){
		_buffer=(char*)malloc(sz);
		if(!_buffer){
			_status=-1;
			return;
		}
		memmove(_buffer,buffer,sz);
		_sz=sz;
	}
	else{
char*	tmp;
		tmp=(char*)realloc(_buffer,_sz+sz);
		if(!tmp){
			_status=-1;
			return;
		}
		_buffer=tmp;
		memmove(_buffer+_sz,buffer,sz);
		_sz+=sz;
	}
}
