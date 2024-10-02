//----------------------------------------------------------------------------
// File : wproj.cpp
// Project : MacMapSuite
// Purpose : CPP source file : bStdProj C wrappers
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
// 11/01/2006 creation.
//----------------------------------------------------------------------------

#include "wproj.h"
#include "bStdProj.h"
#include "lambert93.h"
#include "wgs84.h"
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 
// ------------
wproj wprj_alloc(int sgn, const char* prm){
bStdProj*	prj=NULL;
bool		init;
	switch(sgn){
		case kPRJWithParam:
			prj=new bStdProj(prm);
			break;
			prj=new wgs84;
			break;
		case kPRJLambert93:
			prj=new lambert93;
			break;
		default:// with srid
			prj=new bStdProj(sgn,&init);
			if(!init){
				delete prj;
				prj=NULL;
			}
			break;
	}
	return(prj);
}

// ---------------------------------------------------------------------------
// 
// ------------
void wprj_free(wproj prj){
	delete (bStdProj*)prj;
}

// ---------------------------------------------------------------------------
// 
// ------------
int wprj_transform(wproj prj, wproj src, dvertices* vxs){
	return(((bStdProj*)prj)->transform((*((bStdProj*)src)),vxs));
}

// ---------------------------------------------------------------------------
// 
// ------------
wtable wprj_gettable(void){
CFBundleRef bndl=CFBundleGetBundleWithIdentifier(CFSTR("com.cbconseil.macmapsuite.framework"));
	if(!bndl){
fprintf(stderr,"wprj_gettable : NULL bndl\n");
		return(NULL);
	}
CFURLRef	url=CFBundleCopyResourceURL(bndl,CFSTR("proj"),CFSTR("txt"),NULL);
	if(!url){
fprintf(stderr,"wprj_gettable : NULL url\n");
		return(NULL);
	}
CFStringRef	cfs=CFURLCopyFileSystemPath(url,kCFURLPOSIXPathStyle);
	CFRelease(url);
	if(!cfs){
fprintf(stderr,"wprj_gettable : NULL cfs\n");
		return(NULL);
	}
char		path[SHRT_MAX];
	CFStringGetCString(cfs,path,SHRT_MAX,kCFStringEncodingMacRoman);
	CFRelease(cfs);

	for(int i=strlen(path);i>0;i--){
		if(path[strlen(path)-1]!='/'){
			path[strlen(path)-1]=0;
		}
		else{
			break;
		}
	}

int		bf=0;
wtable	tbl=wtbl_alloc(kTableTabText,path,"proj.txt",false,0,-1,&bf);
fprintf(stderr,"wprj_gettable : path = %s\n",path);

	if(bf){
fprintf(stderr,"wprj_gettable : status=%d\n",bf);
		wtbl_free(tbl);
		return(NULL);
	}
	return(tbl);
}

