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
// 31/03/2016 porting from CFXMLTree deprecated API to libxml2.
//----------------------------------------------------------------------------

#include "WMSCapabilitiesParser.h"
#include "C_Utils.h"

#include <MacMapSuite/bTrace.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

// ---------------------------------------------------------------------------
//
// -----------
static void GetValue(xmlNode *root, char* str){
xmlNode*    cur_node=root->children;

    str[0]=0;
    if(!cur_node){
        return;
    }
    if(cur_node->type!=XML_TEXT_NODE){
        return;
    }
    if(!cur_node->content){
        return;
    }
CFStringRef cfs=CFStringCreateWithCString(kCFAllocatorDefault,(char*)cur_node->content,kCFStringEncodingUTF8);
    CFStringGetCString(cfs,str,4096,kCFStringEncodingMacRoman);
    CFRelease(cfs);
}

// ---------------------------------------------------------------------------
//
// -----------
static void GetSRS(xmlNode *root, bArray* srs){
 char	str[4096];
	GetValue(root,str);
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
static void ParseOneLayer(xmlNode *root, bArray &arr, int level){
wmslayerdesc	desc={NULL,NULL,NULL,level};
char			str[2048];
xmlNode*        cur_node=NULL;
char*           tmp;
   
    desc.srs=new bArray(sizeof(int));
    for(cur_node=root;cur_node;cur_node=cur_node->next){
        if(cur_node->type==XML_ELEMENT_NODE){
            if(cur_node->name){
                tmp=strdup((const char*)cur_node->name);
                strupper(tmp);
                if(strcmp(tmp,"NAME")==0){
                    GetValue(cur_node,str);
                    desc.name=strdup(str);
                }
                else if(strcmp(tmp,"TITLE")==0){
                    GetValue(cur_node,str);
                    desc.title=strdup(str);
                }
                else if(strcmp(tmp,"SRS")==0){
                    GetSRS(cur_node,desc.srs);
                }
                else if(strcmp(tmp,"LAYER")==0){
                   ParseOneLayer(cur_node->children,arr,level+1);
                }
                free(tmp);
            }
        }
    }
    arr.add(&desc);
}

// ---------------------------------------------------------------------------
//
// -----------
static void ParseLayers(xmlNode *root, bArray &arr){
xmlNode*    cur_node=NULL;
char*       tmp;
    
    for(cur_node=root;cur_node;cur_node=cur_node->next){
        if(cur_node->type==XML_ELEMENT_NODE){
            if(cur_node->name){
                tmp=strdup((const char*)cur_node->name);
                strupper(tmp);
                if(strcmp(tmp,"LAYER")==0){
                    ParseOneLayer(cur_node->children,arr,0);
                }
                free(tmp);
            }
        }
    }
}

// ---------------------------------------------------------------------------
//
// -----------
static void ParseCapabilities(xmlNode *root, bArray &arr){
_bTrace_("ParseCapabilities",true);
xmlNode*    cur_node=NULL;
char*       tmp;
    
    for(cur_node=root;cur_node;cur_node=cur_node->next){
        if(cur_node->type==XML_ELEMENT_NODE){
            if(cur_node->name){
                tmp=strdup((const char*)cur_node->name);
                strupper(tmp);
                if(strcmp(tmp,"CAPABILITY")==0){
                    ParseLayers(cur_node->children,arr);
                }
                free(tmp);
           }
        }
    }
}

// ---------------------------------------------------------------------------
//
// -----------
int WMSCapabilitiesParser(void* data, size_t sz, bArray &arr){
_bTrace_("WMSCapabilitiesParser",true);
xmlDoc *doc=xmlReadMemory((const char*)data,sz,"noname.xml",NULL,XML_PARSE_NOBLANKS);
    if(doc==NULL){
_te_("xmlReadMemory failed");
        return -1;
    }
xmlNode *root=xmlDocGetRootElement(doc);
    if(root==NULL){
_te_("xmlDocGetRootElement returned NULL");
        xmlFreeDoc(doc);
        return -2;
    }
    if(root->type!=XML_ELEMENT_NODE) {
_te_("root->type!=XML_ELEMENT_NODE");
        xmlFreeDoc(doc);
        return -3;
    }
xmlNode*    cur_node=NULL;
char*       tmp;

    for(cur_node=root;cur_node;cur_node=cur_node->next){
        if(cur_node->type==XML_ELEMENT_NODE){
            if(cur_node->name){
                tmp=strdup((const char*)cur_node->name);
                strupper(tmp);
                if(strcmp(tmp,"WMT_MS_CAPABILITIES")==0){
                    ParseCapabilities(cur_node->children,arr);
                }
                free(tmp);
            }
        }
    }
    xmlFreeDoc(doc);
    return 0;
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
