//----------------------------------------------------------------------------
// File : bTrace.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Trace class
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
// 03/12/2004 creation.
//----------------------------------------------------------------------------

#include "bTrace.h"
#include "bStdDirectory.h"
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <errno.h>

// ---------------------------------------------------------------------------
// 
// ------------
int		_USE_STDERR_=1;

int bTrace::_indent=0;
bool bTrace::_on=true;
bool bTrace::_flush=true;
bTrace* bTrace::_inst=NULL;
FILE* bTrace::_fp=NULL;

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bTrace::bTrace(const char *name, bool show){
	_show=show;
	strcpy(_fct,name);
	if(_inst==NULL){
		_inst=this;
		init();
	}
	if(_show){
		enter();
	}
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bTrace::~bTrace(){
	if(_inst==this){
		close();
	}
	if(_show&&(_fct[0]!=0)){
		leave();
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::enter(){
	if(!_on){
		return;
	}
	fprintf(_fp,">\n");
	sep();
	indent();
	fprintf(_fp,"{ ENTER %s\n",_fct);
	if(_flush){
		fflush(_fp);
	}
	_indent++;
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::leave(){
	if(!_on){
		return;
	}
	_indent--;
	indent();
	fprintf(_fp,"} LEAVE %s\n",_fct);
	sep();
	if(_flush){
		fflush(_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::sep(){
	if(!_on){
		return;
	}
	fprintf(_fp,">---------------------------------------------------------------------------\n");
	if(_flush){
		fflush(_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::set_on(bool b){
	if(b!=_on){
		_on=b;
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bTrace::get_on(){
	return(_on);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::set_flush(bool b){
	if(b!=_flush){
		_flush=b;
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
bool bTrace::get_flush(){
	return(_flush);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::indent(){	
	fprintf(_fp,">");

	for(int i=0;i<_indent;i++){
		fprintf(_fp,"\t");
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::wtime(){
time_t	t=time(NULL);
char*	c=asctime(localtime(&t));
	fprintf(_fp,">%s",c);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::init(){
char	path[SHRT_MAX]="";
struct	passwd *pwp=getpwuid(getuid());

	if(_USE_STDERR_==0){
/*FSRef		fs;
		if(FSFindFolder(kUserDomain,kLogsFolderType,kDontCreateFolder,&fs)==noErr){
bStdDirectory dr(&fs);
			sprintf(path,"%s.log",_PROCESS_NAME_);
			_fp=fopen(path,"w");
		}
		else{
			_fp=NULL;
		}*/
		
		if(pwp){
			sprintf(path,"/users/%s/Library/Logs/macmap.log",pwp->pw_name);
			_fp=fopen(path,"w");
		}
		if(_fp==NULL){
			_fp=stderr;
fprintf(_fp,">Log init failed for : %s\n",path);
		}
	}
	else{
		_fp=stderr;
	}
		
	wtime();
	getcwd(path,SHRT_MAX);
	fprintf(_fp,">process path : %s\n",path);
	fprintf(_fp,">process name : %s\n",_fct);

	if(pwp){
		fprintf(_fp,">user : %s\n",pwp->pw_name);
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::close(){
	wtime();
	fprintf(_fp,">quit…\n");
	if(_fp!=stderr){
		fclose(_fp);
		_fp=NULL;
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::err(bString& str){
	if(!_on){
		return;
	}	
	fprintf(_fp,">!!! ERROR ");	
	if(_fct[0]){
		fprintf(_fp,"in ");
		fprintf(_fp,_fct);
		fprintf(_fp," ");
	}
	fprintf(_fp,": ");
	fprintf(_fp,str.string());
	fprintf(_fp," !!!\n");
		
	fflush(_fp);
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::msg(bString& str){
	if(!_on){
		return;
	}
	indent();
	if(_fct[0]){
		fprintf(_fp,_fct);
		fprintf(_fp," : ");
	}
	fprintf(_fp,str.string());
	fprintf(_fp,"\n");
	if(_flush){
		fflush(_fp);
	}
}

// ---------------------------------------------------------------------------
// 
// ------------
void bTrace::wrn(bString& str){
	if(!_on){
		return;
	}	
	fprintf(_fp,">!!! WARNING ");	
	if(_fct[0]){
		fprintf(_fp,"in ");
		fprintf(_fp,_fct);
		fprintf(_fp," ");
	}
	fprintf(_fp,": ");
	fprintf(_fp,str.string());
	fprintf(_fp," !!!\n");
		
	fflush(_fp);
}



