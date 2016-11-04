//----------------------------------------------------------------------------
// File : bTrace.h
// Project : MacMapSuite
// Purpose : Header file : Trace class
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

#ifndef __bTrace__
#define __bTrace__

//----------------------------------------------------------------------------

#include <stdio.h>
#include <MacMapSuite/bString.h>

//----------------------------------------------------------------------------

#define _bTrace_(a,b) 		 bTrace __trc__(a,b);bString __str__("")

#define _msg_(a)			__trc__.msg(a)
#define _err_(a)			__trc__.err(a)
#define _wrn_(a)			__trc__.wrn(a)

#define _tm_(a)				__trc__.msg(__str__.reset()+a)
#define _te_(a)				__trc__.err(__str__.reset()+a)
#define _tw_(a)				__trc__.wrn(__str__.reset()+a)

#define _me_(e,a)			e?__trc__.msg(__str__.reset()+a):__str__.reset()
#define _ee_(e,a)			e?__trc__.err(__str__.reset()+a):__str__.reset()
#define _we_(e,a)			e?__trc__.wrn(__str__.reset()+a):__str__.reset()

#define _strr_				__str__.reset()

//----------------------------------------------------------------------------

#define _trrect_(r)			"("+r.left+":"+r.top+":"+r.right+":"+r.bottom+")"
#define _trxysz_(r)			"(("+r.origin.x+":"+r.origin.y+");("+r.size.width+":"+r.size.height+"))"
#define _trxy_(xy)			"("+xy.x+":"+xy.y+")"
#define _trhv_(hv)			"("+hv.h+":"+hv.v+")"

//----------------------------------------------------------------------------

extern int _USE_STDERR_;

//----------------------------------------------------------------------------

class bTrace{
public:
    bTrace					(	const char* name, 
                                bool show);
    virtual ~bTrace			(	);

    virtual void err		(	bString& str);
    virtual void msg		(	bString& str);
    virtual void wrn		(	bString& str);
    
    virtual void set_on		(	bool b);
    virtual bool get_on		(	);
    virtual void set_flush	(	bool b);
    virtual bool get_flush	(	);

    virtual void sep		(	);
    virtual void wtime		(	);

protected:
    virtual void enter		(	);
    virtual void leave		(	);
    virtual void indent		(	);

private:
    virtual void init		(	);
    virtual void close		(	);
    
    static int		_indent;
    static bool		_on;
    static bool		_flush;
    static bTrace*	_inst;
    static FILE*	_fp;
    
    char			_fct[256];
    bool			_show;
		
};

//----------------------------------------------------------------------------

#endif
