//----------------------------------------------------------------------------
// File : bPGisTable.h
// Project : MacMapSuite
// Purpose : Header file : Postgis table class
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
// 28/06/2005 creation.
//----------------------------------------------------------------------------

#ifndef __bPGisTable__
#define __bPGisTable__

//----------------------------------------------------------------------------

#include "bStdTable.h"
#include "libpq-fe.h"
#include "vx.h"

//----------------------------------------------------------------------------

class bPGisTable : public bStdTable{
	public:
		bPGisTable						(	const char* path, 
											const char* name, 
											bool create, 
											double reso,
											int srid,
											int* status);
		bPGisTable						(	const char* path,
											const char* name,
											bool create, 
											double* reso,
											double* ox,
											double* oy,
											int* tsrid,
											int* asrid,
											int* status);
		virtual ~bPGisTable				(	);

		virtual int KillRecord			(	int o);
		virtual int UnkillRecord		(	int o);
		virtual int RecordKilled		(	int o);

		virtual int CountRecords		(	);
		virtual int CountFields			(	);
		virtual int AddField			(	const char* nam, 
											int sign, 
											int length, 
											int decs);
		virtual int RmvField			(	int f);
		virtual int ChgField			(	int f, 
											const char* nam, 
											int sign, 
											int length, 
											int decs);
		virtual int FieldSign			(	int f, 
											int* sign);
		virtual int FieldName			(	int f, 
											char* name);
		virtual int FieldDecimals		(	int f, 
											int* decs);
		virtual int FieldLength			(	int f, 
											int* len);
		virtual int FieldSize			(	int f, 
											int* sz);

		virtual int ReadVal				(	int o, 
											int f, 
											void* val);
		virtual int WriteVal			(	int o, 
											int f, 
											void* val);
		
		virtual int Pack				(	);
		
		virtual int signature			(	);

		
	protected:
	
	private:
		void rq_add						(	const char *part);
		int	exec						(	char* req, 
											int expect);

		int	load_current				(	);
		int	append						(	);
		
		int fields_property				(	const char* property_name,
											PGresult** res);
		int	field_property				(	int f, 
											const char* property_name, 
											char* property_value);
		int	natural_sign				(	int f);
		int	natural_geom_sign			(	int f);
		int	f_geom_tid					(	);
		
		void print_wkb_bytes			(	unsigned char *ptr, 
											unsigned int cnt, 
											size_t size);
		void print_wkb_double			(	double val);
		void print_wkb_byte				(	unsigned char val);
		void print_wkb_int				(	int val);
		
		void write_polygon				(	ivertices* vxs);
		void write_linestring			(	ivertices* vxs);
		void write_multipoint			(	ivertices* vxs);
		void write_point				(	i2dvertex* vx);
		void write_box2d				(	ivx_rect* vr);
		void write_box3d				(	ivx_cube* vr);

		ivertices* read_multipolygon2D	(	unsigned char* wkb);
		ivertices* read_polygon2D		(	unsigned char* wkb);
		ivertices* read_multiline2D		(	unsigned char* wkb);
		ivertices* read_line2D			(	unsigned char* wkb);
		ivertices* read_multipoint2D	(	unsigned char* wkb);
		i2dvertex read_point2D			(	unsigned char* wkb);
		ivx_rect read_box2d				(	char* str);
		ivx_cube read_box3d				(	char* str);

		ivertices* make_vertices		(	int nv, 
											double* x,
											double* y,
											int no,
											int* offs);

		char				_name[256];

		PGconn*				_cnx;
		
		PGresult*			_row_buf;
		int					_cur;
		
		char*				_dyn_rq;
		
		char				_stat_rq[512];
		int					_rq_sz;
		
		int					_geomid;
		int					_box2did;
		int					_box3did;

};

//----------------------------------------------------------------------------

#endif


