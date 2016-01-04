//----------------------------------------------------------------------------
// File : WMSCapabilitiesParser.cpp
// Project : MacMapSuite
// Purpose : CPP source file : WMS capabilities parser
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
// 06/07/2009 creation.
//----------------------------------------------------------------------------

#include "WMSCapabilitiesParser.h"
#include <MacMapSuite/bTrace.h>

// ---------------------------------------------------------------------------
// 
// -----------
static void GetValue(CFXMLTreeRef xmlTree, char* str){
int				n;
CFXMLTreeRef	xmlTreeNode;
CFXMLNodeRef	node;
CFStringRef		cfs;

	str[0]=0;
	n=CFTreeGetChildCount(xmlTree);
	if(n!=1){
		return;
	}
	xmlTreeNode=CFTreeGetChildAtIndex(xmlTree,0);
	node=CFXMLTreeGetNode(xmlTreeNode);
	if(CFXMLNodeGetTypeCode(node)!=kCFXMLNodeTypeText){
		return;
	}
	cfs=CFXMLNodeGetString(node);
	CFStringGetCString(cfs,str,4096,kCFStringEncodingMacRoman);
}

// ---------------------------------------------------------------------------
// 
// -----------
static void GetSRS(CFXMLTreeRef xmlTree, bArray* srs){
char	str[4096];
	GetValue(xmlTree,str);
int		srid;
char*	p=strstr(str,":");
	while(p){
		p++;
		srid=atoi(p);
		if(srid>0){
			srs->add(&srid);
		}
		p=strstr(p,":");
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
static void ParseOneLayer(CFXMLTreeRef xmlTree, bArray &arr, int level){
int				n;
CFXMLTreeRef	xmlTreeNode;
CFXMLNodeRef	node;
CFStringRef		cfs;
wmslayerdesc	desc={NULL,NULL,NULL,level};
char			str[2048];
//int				srs;

	desc.srs=new bArray(sizeof(int));
	n=CFTreeGetChildCount(xmlTree);
	for(int i=0;i<n;i++){
		xmlTreeNode=CFTreeGetChildAtIndex(xmlTree,i);
		node=CFXMLTreeGetNode(xmlTreeNode);
		cfs=CFXMLNodeGetString(node);
		if(CFStringCompare(cfs,CFSTR("Name"),0)==kCFCompareEqualTo){
			GetValue(xmlTreeNode,str);
			desc.name=strdup(str);
		}
		else if(CFStringCompare(cfs,CFSTR("Title"),0)==kCFCompareEqualTo){
			GetValue(xmlTreeNode,str);
			desc.title=strdup(str);
		}
		else if(CFStringCompare(cfs,CFSTR("SRS"),0)==kCFCompareEqualTo){
			GetSRS(xmlTreeNode,desc.srs);
		}
		else if(CFStringCompare(cfs,CFSTR("Layer"),0)==kCFCompareEqualTo){
			ParseOneLayer(xmlTreeNode,arr,level+1);
		}
	}
	arr.add(&desc);
}

// ---------------------------------------------------------------------------
// 
// -----------
static void ParseLayers(CFXMLTreeRef xmlTree, bArray &arr){
int				n;
CFXMLTreeRef	xmlTreeNode;
CFXMLNodeRef	node;
CFStringRef		cfs;
	
	n=CFTreeGetChildCount(xmlTree);
	for(int i=0;i<n;i++){
		xmlTreeNode=CFTreeGetChildAtIndex(xmlTree,i);
		node=CFXMLTreeGetNode(xmlTreeNode);
		if(CFXMLNodeGetTypeCode(node)!=kCFXMLNodeTypeElement){
			continue;
		}
		cfs=CFXMLNodeGetString(node);
		if(CFStringCompare(cfs,CFSTR("Layer"),0)==kCFCompareEqualTo){
			ParseOneLayer(xmlTreeNode,arr,0);
		}		
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
static void ParseCapabilities(CFXMLTreeRef xmlTree, bArray &arr){
int				n;
CFXMLTreeRef	xmlTreeNode;
CFXMLNodeRef	node;
CFStringRef		cfs;
	
	n=CFTreeGetChildCount(xmlTree);
	for(int i=0;i<n;i++){
		xmlTreeNode=CFTreeGetChildAtIndex(xmlTree,i);
		node=CFXMLTreeGetNode(xmlTreeNode);
		if(CFXMLNodeGetTypeCode(node)!=kCFXMLNodeTypeElement){
			continue;
		}
		cfs=CFXMLNodeGetString(node);
		if(CFStringCompare(cfs,CFSTR("Capability"),0)==kCFCompareEqualTo){
			ParseLayers(xmlTreeNode,arr);
		}		
	}
}


// ---------------------------------------------------------------------------
// 
// -----------
int WMSCapabilitiesParser(void* data, size_t sz, bArray &arr){
_bTrace_("WMSCapabilitiesParser",true);
int				n;
CFXMLTreeRef	xmlTree,xmlTreeNode;
CFXMLNodeRef	node;
CFStringRef		cfs;
int				res=-3;
CFDataRef		xmlData=CFDataCreate(kCFAllocatorDefault,(UInt8*)data,sz);

	if(xmlData==NULL){
_te_("xmlData==NULL");
		return(-1);
	}	
	xmlTree=CFXMLTreeCreateFromData(	kCFAllocatorDefault,
										xmlData, 
										NULL, 
										kCFXMLParserAllOptions,
										kCFXMLNodeCurrentVersion);
	if(xmlTree==NULL){
_te_("xmlTree==NULL");
		return(-2);
	}
	n=CFTreeGetChildCount(xmlTree);
	for(int i=0;i<n;i++){
		xmlTreeNode=CFTreeGetChildAtIndex(xmlTree,i);
		node=CFXMLTreeGetNode(xmlTreeNode);
		if(CFXMLNodeGetTypeCode(node)!=kCFXMLNodeTypeElement){
			continue;
		}
		cfs=CFXMLNodeGetString(node);
		CFShowStr(cfs);
		if(CFStringCompare(cfs,CFSTR("WMT_MS_Capabilities"),0)==kCFCompareEqualTo){
			ParseCapabilities(xmlTreeNode,arr);
			res=0;
		}
	}
	CFRelease(xmlData);
	return(res);
}

// ---------------------------------------------------------------------------
// 
// -----------
void WMSCapabilitiesFreeDescs(bArray &arr){
wmslayerdesc	desc;
	for(int i=1;i<=arr.count();i++){
		arr.get(i,&desc);
		if(desc.name){
			free(desc.name);
		}
		if(desc.title){
			free(desc.title);
		}		
		if(desc.srs){
			delete desc.srs;
		}
	}
	arr.reset();
}
