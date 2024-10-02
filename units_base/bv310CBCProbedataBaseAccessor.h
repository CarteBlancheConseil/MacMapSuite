//----------------------------------------------------------------------------
// File : bv310CBCProbedataBaseAccessor.h
// Project : MacMapSuite
// Purpose : Header file : Arcinfo ASCII Grid database base class accessor version 3.1.0
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
// 04/07/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bv310CBCProbedataBaseAccessor__
#define __bv310CBCProbedataBaseAccessor__

//----------------------------------------------------------------------------

#include "bv310BaseAccessor.h"
#include "wgs84.h"

enum{
	kBaseCBCProbedata		='bCBP',
    kBaseCBCProbedataRaw	='9990',
    kBaseCBCProbedataID		=9990
};

//typedef struct __attribute__ ((packed)){
typedef struct probe_point{
    unsigned char   mobile_id[32];
    int             lat;
    int             lon;
    unsigned short  speed;
    unsigned short  heading;
    time_t          timestamp;
//    unsigned char   vehicle_t;
//    unsigned char   error_code;
} probe_point;


//----------------------------------------------------------------------------

class bv310CBCProbedataBaseAccessor : public bv310BaseAccessor{
	public:
		bv310CBCProbedataBaseAccessor			(	const char* hpath,
													int wanted_srid,
													double wanted_reso,
													int* status);
		bv310CBCProbedataBaseAccessor			(	const char* hpath,
													int* status,
													int kind,
													int srid,
													double reso,
													double u2m,
													const char* name,												
													const char* data);
		virtual ~bv310CBCProbedataBaseAccessor	(	);

		virtual int signature					(	);
		
		virtual int	read						(	int o, 
													int f, 
													void *val);
		virtual int	write						(	int o, 
													int f, 
													void *val);
	
		virtual int count_records				(	);
		virtual bool killed_record				(	int r);
	
        virtual int h_read                      (   int o,
                                                    int f,
                                                    void *val);
        virtual int h_write                     (   int o,
                                                    int f,
                                                    void *val);

	protected:
		virtual int lnk_open					(	);
		virtual void lnk_close					(	);
    
        virtual int load                        (   int wanted_srid);
        /*virtual int load_infos                  (   );*/
		virtual int load_fields					(	);
		/*virtual int load_constraints			(	);
		virtual int load_dyn					(	);*/
	
		virtual bool load_data					(	const char* hpath);
		virtual ivertices* make_vertices		(	long o);
	
	private:
		/*long		_nc,_nl;
		double		_ox,_oy;
		double		_csz;
		double		_ndv;
		double*		_arr;*/
		dvertices	_dvs;
    
       // bStdTable*      _dat;
        probe_point*    _prbs;
        size_t          _nprb;
};

//----------------------------------------------------------------------------

#endif


