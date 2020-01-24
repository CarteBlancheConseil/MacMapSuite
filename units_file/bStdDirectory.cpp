//----------------------------------------------------------------------------
// File : bStdDirectory.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Directory class utils
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

#include "bStdDirectory.h"
#include "mms_config.h"
#include <unistd.h>
#if _DEVELOPMENT_
#include "bTrace.h"
#endif

#pragma mark bStdDirectory
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdDirectory::bStdDirectory(const char* path){
#if _DEVELOPMENT_
_bTrace_("bStdDirectory::bStdDirectory(char* path)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(strlen(path)==0){
		return;
	}
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
	strcpy(_path,path);
	if(_path[strlen(_path)-1]!='/'){
		strcat(_path,"/");
	}
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,_path,kCFStringEncodingMacRoman);
	CFStringGetCString(cfs,_path,PATH_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	if(chdir(_path)){
#if _DEVELOPMENT_
_te_("chdir failed for "+_path);
#endif
		_cwd[0]=0;
		_status=-1;
		return;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdDirectory::bStdDirectory(int fd){
#if _DEVELOPMENT_
_bTrace_("bStdDirectory::bStdDirectory(int fd)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
	if(fchdir(fd)==-1){
#if _DEVELOPMENT_
_te_("fchdir failed for "+fd);
#endif
		_status=-1;
		_cwd[0]=0;
		return;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdDirectory::bStdDirectory(CFURLRef url){
#if _DEVELOPMENT_
_bTrace_("bStdDirectory::bStdDirectory(CFURLRef url)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
	
CFStringRef	cfs=CFURLCopyFileSystemPath(url,kCFURLPOSIXPathStyle);
	if(!cfs){
#if _DEVELOPMENT_
_te_("bad cfs");
#endif
		_status=-1;
		return;
	}
	CFStringGetCString(cfs,_path,PATH_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	strcat(_path,"/");
	if(chdir(_path)){
#if _DEVELOPMENT_
_te_("chdir failed for "+_path);
#endif
		_cwd[0]=0;
		_status=-1;
		return;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdDirectory::~bStdDirectory(){	
	if(_cwd[0]!=0){
		chdir(_cwd);
	}
	if(_names){
		reset();
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdDirectory::status(){	
	return(_status);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdDirectory::mount(){
	mount(f_comp,f_select);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdDirectory::count(){
	return(_n);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdDirectory::name(int idx, char* name){
	if(idx<1||idx>_n){
		name[0]=0;
		return;
	}
	
CFStringRef	cfs=CFStringCreateWithCString(kCFAllocatorDefault,_names[idx-1]->d_name,kCFStringEncodingUTF8);
	CFStringGetCString(cfs,name,FILENAME_MAX,kCFStringEncodingMacRoman);
	CFRelease(cfs);
}

// ---------------------------------------------------------------------------
// 
// -----------
#if(BIGSYS)
void bStdDirectory::mount(int(*compare)(const struct dirent**,const struct dirent**),
                          int(*select)(const struct dirent* dp)){
#else
void bStdDirectory::mount(int(*compare)(const void*,const void*),
						  int(*select)(struct dirent* dp)){
#endif
	if(_names){
		reset();
	}
	if(chdir(_cwd)){
		_status=-2;
		return;
	}
	_n=scandir(_path,&_names,select,compare);
	if(chdir(_path)){
		_status=-3;
		return;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdDirectory::reset(){
	if(_names!=NULL){
		for(int i=0;i<_n;i++){
			free(_names[i]);
		}
		free(_names);
	}
	_n=0;
	_names=NULL;
}

// ---------------------------------------------------------------------------
// 
// -----------
#if BIGSYS
int bStdDirectory::f_select(const struct dirent* dp){
#else
int bStdDirectory::f_select(struct dirent* dp){
#endif
	if(!strcmp(".",dp->d_name)){
		return(false);
	}
	if(!strcmp("..",dp->d_name)){
		return(false);
	}
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
#if BIGSYS
int bStdDirectory::f_comp(const struct dirent **d1, const struct dirent **d2){
	return(strcmp((*d1)->d_name,(*d2)->d_name));
}
#else
int bStdDirectory::f_comp(const void *d1, const void *d2){
	struct dirent *dir1=*(struct dirent**)d1;
	struct dirent *dir2=*(struct dirent**)d2;
	return(strcmp(dir1->d_name,dir2->d_name));
}
#endif


#pragma mark bStdUTF8Directory
// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdUTF8Directory::bStdUTF8Directory(const char* path){
#if _DEVELOPMENT_
_bTrace_("bStdUTF8Directory::bStdUTF8Directory(char* path)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(strlen(path)==0){
		return;
	}
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
    
	strcpy(_path,path);
	if(_path[strlen(_path)-1]!='/'){
		strcat(_path,"/");
	}
	if(chdir(_path)){
#if _DEVELOPMENT_
_te_("chdir failed for "+_path);
#endif
		_cwd[0]=0;
		_status=-1;
		return;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdUTF8Directory::bStdUTF8Directory(int fd){
#if _DEVELOPMENT_
_bTrace_("bStdUTF8Directory::bStdUTF8Directory(int fd)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
	if(fchdir(fd)==-1){
#if _DEVELOPMENT_
_te_("fchdir failed for "+fd);
#endif
		_status=-1;
		_cwd[0]=0;
		return;
	}
}

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdUTF8Directory::bStdUTF8Directory(CFURLRef url){
#if _DEVELOPMENT_
_bTrace_("bStdUTF8Directory::bStdUTF8Directory(CFURLRef url)",false);
#endif
	_cwd[0]=0;
	_status=noErr;
	_names=NULL;
	_n=0;
	if(!getcwd(_cwd,PATH_MAX)){
#if _DEVELOPMENT_
_te_("getcwd failed");
#endif
		_status=-1;
		return;
	}
	
CFStringRef	cfs=CFURLCopyFileSystemPath(url,kCFURLPOSIXPathStyle);
	if(!cfs){
#if _DEVELOPMENT_
_te_("bad cfs");
#endif
		_status=-1;
		return;
	}
	CFStringGetCString(cfs,_path,PATH_MAX,kCFStringEncodingUTF8);
	CFRelease(cfs);
	strcat(_path,"/");
	if(chdir(_path)){
#if _DEVELOPMENT_
_te_("chdir failed for "+_path);
#endif
		_cwd[0]=0;
		_status=-1;
		return;
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdUTF8Directory::~bStdUTF8Directory(){	
	if(_cwd[0]!=0){
		chdir(_cwd);
	}
	if(_names){
		reset();
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdUTF8Directory::status(){	
	return(_status);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdUTF8Directory::mount(){
	mount(f_comp,f_select);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdUTF8Directory::count(){
	return(_n);
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdUTF8Directory::name(int idx, char* name){
	if(idx<1||idx>_n){
		name[0]=0;
		return;
	}
	sprintf(name,"%s",_names[idx-1]->d_name);
}

// ---------------------------------------------------------------------------
// 
// -----------
#if(BIGSYS)
void bStdUTF8Directory::mount(int(*compare)(const struct dirent**,const struct dirent**),
							  int(*select)(const struct dirent* dp)){
#else
void bStdUTF8Directory::mount(int(*compare)(const void*,const void*),
							  int(*select)(struct dirent* dp)){
#endif
	if(_names){
		reset();
	}
	if(chdir(_cwd)){
		_status=-2;
		return;
	}
	_n=scandir(_path,&_names,select,compare);
	if(chdir(_path)){
		_status=-3;
		return;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
void bStdUTF8Directory::reset(){
	if(_names!=NULL){
		for(int i=0;i<_n;i++){
			free(_names[i]);
		}
		free(_names);
	}
	_n=0;
	_names=NULL;
}

// ---------------------------------------------------------------------------
//
// -----------
#if BIGSYS
int bStdUTF8Directory::f_select(const struct dirent* dp){
#else
int bStdUTF8Directory::f_select(struct dirent* dp){
#endif
	if(!strcmp(".",dp->d_name)){
		return(false);
	}
	if(!strcmp("..",dp->d_name)){
		return(false);
	}
	return(true);
}

// ---------------------------------------------------------------------------
//
// -----------
#if BIGSYS
int bStdUTF8Directory::f_comp(const struct dirent **d1, const struct dirent **d2){
	return(strcmp((*d1)->d_name,(*d2)->d_name));
}
#else
int bStdUTF8Directory::f_comp(const void *d1, const void *d2){
	struct dirent *dir1=*(struct dirent**)d1;
	struct dirent *dir2=*(struct dirent**)d2;
	return(strcmp(dir1->d_name,dir2->d_name));
}
#endif
