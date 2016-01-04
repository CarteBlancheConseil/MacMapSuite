//----------------------------------------------------------------------------
// File : bStdDirectory.h
// Project : MacMapSuite
// Purpose : Header file : Directory class utils
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

#ifndef __bStdDirectory__
#define __bStdDirectory__

//----------------------------------------------------------------------------

#include <dirent.h>
#include <stdio.h>
#include <limits.h>

#include <sys/types.h>
#include <dirent.h>
#include <AvailabilityMacros.h>

#define BIGSYS 1
//MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_9
//----------------------------------------------------------------------------

class bStdDirectory{
public:		
	bStdDirectory				(	const char* path);
	bStdDirectory				(	int fd);		
	bStdDirectory				(	CFURLRef ref);

	virtual ~bStdDirectory		(	);
	
	virtual int status			(	);
	
	virtual void mount			(	);
	
	virtual int count			(	);
	virtual void name			(	int idx, 
									char* name);
	

protected:
#if BIGSYS
	virtual void mount			(	int(*compare)(const struct dirent**,const struct dirent**),
									int(*select)(const struct dirent* dp));
#else
	virtual void mount			(	int(*compare)(const void*,const void*),
									int(*select)(struct dirent* dp));
#endif
	
	virtual void reset			(	);

#if BIGSYS
	static int f_select			(	const struct dirent* dp);
	static int f_comp			(	const struct dirent** d1,
									const struct dirent** d2);
#else
	static int f_select			(	struct dirent* dp);
	static int f_comp			(	const void *d1, 
									const void *d2);
#endif
				
	char			_cwd[PATH_MAX];
	char			_path[PATH_MAX];
	int				_status;
	int				_n;
	struct dirent**	_names;

private:
};

//----------------------------------------------------------------------------

class bStdUTF8Directory{
public:		
	bStdUTF8Directory			(	const char* path);
	bStdUTF8Directory			(	int fd);		
	bStdUTF8Directory			(	CFURLRef ref);
	
	virtual ~bStdUTF8Directory	(	);
	
	virtual int status			(	);
	
	virtual void mount			(	);
	
	virtual int count			(	);
	virtual void name			(	int idx, 
                                    char* name);
	
	
protected:
#if BIGSYS
	virtual void mount			(	int(*compare)(const struct dirent**,const struct dirent**),
								 int(*select)(const struct dirent* dp));
#else
	virtual void mount			(	int(*compare)(const void*,const void*),
								 int(*select)(struct dirent* dp));
#endif
	
	virtual void reset			(	);
	
#if BIGSYS
	static int f_select			(	const struct dirent* dp);
	static int f_comp			(	const struct dirent** d1,
								 const struct dirent** d2);
#else
	static int f_select			(	struct dirent* dp);
	static int f_comp			(	const void *d1, 
								 const void *d2);
#endif
	
	char			_cwd[PATH_MAX*3];
	char			_path[PATH_MAX*3];
	int				_status;
	int				_n;
	struct dirent**	_names;
	
private:
};

//----------------------------------------------------------------------------

#endif
