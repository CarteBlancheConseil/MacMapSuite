//----------------------------------------------------------------------------
// File : mmurl.c
// Project : MacMapSuite
// Purpose : C source file : lib_curl utils
// Author : Benoit Ogier,benoit.ogier@macmap.com
//
// Copyright (C) 1997-2015 Carte Blanche Conseil.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation,either version 3 of the License,or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not,see <http://www.gnu.org/licenses/>.
//
// See the COPYING.lesser file for more information.
//
//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// 19/05/2009 creation.
//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include "mmurl.h"

// ---------------------------------------------------------------------------
//
// ------------
typedef struct mmurl{
	char*	user;
	char*	url;
	char*	proxy;
// File
	FILE*	fp;
	char*	fname;
// Memory
	size_t	buffersz;
	void*	buffer;
}mmurl;

// ---------------------------------------------------------------------------
// To memory
// ------------
static int mwriteFunction(void *ptr,size_t size,size_t nmemb,mmurl *mmu){
	if(!mmu->buffer){
		mmu->buffer=malloc(size*nmemb);
	}
	else{
		mmu->buffer=realloc(mmu->buffer,mmu->buffersz+(size*nmemb));
	}
	if(mmu->buffer){
		memmove((void*)(((size_t)mmu->buffer)+mmu->buffersz),ptr,size*nmemb);
		mmu->buffersz+=(size*nmemb);
		return(nmemb);
	}
	return(0);
}

// ---------------------------------------------------------------------------
// To file
// ------------
static int fwriteFunction(void *ptr,size_t size,size_t nmemb,mmurl *mmu){
	if(!mmu->fp){
		if(mmu->fname != NULL && mmu->fname[0] != 0){
			mmu->fp=fopen(mmu->fname,"wb");
		}
	}
	if(mmu->fp){
		return(fwrite(ptr,size,nmemb,mmu->fp));
	}
	return 0;
}

// ---------------------------------------------------------------------------
//
// ------------
static int request(mmurl *mmu){
CURL*       cuh;
CURLcode	ret;

	cuh=curl_easy_init();
	if(cuh==NULL) {
		return(CURLE_FAILED_INIT);
	}
    if((mmu->fname!=NULL)&&(mmu->fname[0]!=0)){
		mmu->fp=fopen(mmu->fname,"wb");
	}
	curl_easy_setopt(cuh,CURLOPT_USERAGENT,"MacMapSuite");
    curl_easy_setopt(cuh,CURLOPT_BUFFERSIZE,500);
	curl_easy_setopt(cuh,CURLOPT_LOW_SPEED_LIMIT,10);
    curl_easy_setopt(cuh,CURLOPT_LOW_SPEED_TIME,5);
	if((mmu->proxy!=NULL)&&(mmu->proxy[0]!=0)){
		curl_easy_setopt(cuh,CURLOPT_PROXY,mmu->proxy);
	}
    curl_easy_setopt(cuh,CURLOPT_AUTOREFERER,1);
    curl_easy_setopt(cuh,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_setopt(cuh,CURLOPT_URL,mmu->url);
	curl_easy_setopt(cuh,CURLOPT_WRITEDATA,mmu);
	if((mmu->fname!=NULL)&&(mmu->fname[0]!=0)){
		curl_easy_setopt(cuh,CURLOPT_WRITEFUNCTION,fwriteFunction);
	}
	else{
		curl_easy_setopt(cuh,CURLOPT_WRITEFUNCTION,mwriteFunction);
	}
    if(mmu->user){
        curl_easy_setopt(cuh,CURLOPT_HTTPAUTH,CURLAUTH_DIGEST);
		curl_easy_setopt(cuh,CURLOPT_USERPWD,mmu->user);
	}
    ret=curl_easy_perform(cuh);
	if(mmu->fp){
		fclose(mmu->fp);
		mmu->fp=NULL;
	}
	curl_easy_cleanup(cuh);
	return(ret); 	
}


// ---------------------------------------------------------------------------
//
// ------------
int mmurl_get(char *url,
              char *proxy,
              char *Filename,
              void** buffer,
              size_t* buffersz){
int 	ret;
mmurl	mmu;

	mmu.user=NULL;
	mmu.url=url;
	mmu.proxy=proxy;
	mmu.fp=NULL;
    mmu.fname=Filename?strdup(Filename):NULL;
	mmu.buffersz=0;
	mmu.buffer=NULL;

	ret=request(&mmu);

	if(mmu.fp){
		fclose(mmu.fp);
		mmu.fp=NULL;
	}
	else if(mmu.buffer){
		*buffer=mmu.buffer;
		*buffersz=mmu.buffersz;
	}
    if(mmu.fname){
        free(mmu.fname);
    }

	return ret;
}

// ---------------------------------------------------------------------------
//
// ------------
int mmurl_userGet(char *usr,
				  char *url,
				  char *proxy,
				  char *Filename,
				  void** buffer,
				  size_t* buffersz){
int 	ret;
mmurl	mmu;
    
    mmu.user=usr;
    mmu.url=url;
    mmu.proxy=proxy;
    mmu.fp=NULL;
    mmu.fname=Filename?strdup(Filename):NULL;
    mmu.buffersz=0;
    mmu.buffer=NULL;
	
    ret=request(&mmu);
	
    if(mmu.fp){
        fclose(mmu.fp);
        mmu.fp=NULL;
    }
    else if(mmu.buffer){
        *buffer=mmu.buffer;
        *buffersz=mmu.buffersz;
    }
    if(mmu.fname){
        free(mmu.fname);
    }
	
	return ret;
}
