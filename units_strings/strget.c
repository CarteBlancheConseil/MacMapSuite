//----------------------------------------------------------------------------
// File : strget.c
// Project : MacMapSuite
// Purpose : C source file : Localization utils
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
// 17/01/2006 creation.
//----------------------------------------------------------------------------

#include "strget.h"

// ---------------------------------------------------------------------------
// 
// -----------
int _APP_WORLD_=1;

// ---------------------------------------------------------------------------
// 
// -----------
void strget(const char* msg_id, char* string){
CFBundleRef bndl=CFBundleGetBundleWithIdentifier(CFSTR("com.cbconseil.macmapsuite.framework"));
CFStringRef	cfid=CFStringCreateWithCString(kCFAllocatorDefault,msg_id,kCFStringEncodingMacRoman);
CFStringRef msg=CFBundleCopyLocalizedString(bndl,cfid,CFSTR("MESSAGES"),NULL);	

	string[0]=0;
	CFRelease(cfid);
	if(!msg){
		return;
	}
	CFStringGetCString(msg,string,256,kCFStringEncodingMacRoman);
	CFRelease(msg);
}

