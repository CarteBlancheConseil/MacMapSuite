//----------------------------------------------------------------------------
// File : base_def.h
// Project : MacMapSuite
// Purpose : Header file : Database defines
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
// 06/02/2006 creation.
//----------------------------------------------------------------------------

#ifndef __base_def__
#define __base_def__

//----------------------------------------------------------------------------

#include <MacMapSuite/vx.h>

//----------------------------------------------------------------------------

// Base storage kind
enum{
	kBaseLocalDB		='bLDB',
	kBaseLocalDBRaw		='0002',
	kBaseLocalDBID		=2,
	kBasePostGIS		='bPGi',
	kBasePostGISRaw		='0010',
	kBasePostGISID		=10
};

// Base geometry kind
enum{
	kBaseNoKind			=kVXNoKind,
	kBaseKindPoint		=kVXPoint,
	kBaseKindPolyline	=kVXPolyline,
	kBaseKindText		=3,
	kBaseKindPolygon	=kVXPolygon,
	kBaseKindRaster		=5
};

// Objects table std field indexes
enum{
	kOBJ_ID_		=1,
	kOBJ_CrID_		=2,
	kOBJ_CrDate_	=3,
	kOBJ_MdID_		=4,
	kOBJ_MdDate_	=5,
	kOBJ_State_		=6,
	kOBJ_Rect_		=7,
	kOBJ_Vertices_	=8,
	kOBJ_Name_		=9,
	kOBJ_SubType_	=10,
	kOBJ_Color_		=11,
	kOBJ_Dir_		=12  
};

// Header table field indexes
enum{
	kHDR_id_		=1,
	kHDR_CrID_		=2,
	kHDR_CrDate_	=3,
	kHDR_state_		=4,
	kHDR_kind_		=5,
	kHDR_bounds_	=6,
	kHDR_name_		=7,
	kHDR_precs_		=8,
	kHDR_unit2m_	=9,
	kHDR_shortLib_	=10,
	kHDR_longLib_	=11,
	kHDR_alive_		=12,
	kHDR_killed_	=13,
	kHDR_srid_		=14,
	kHDR_fid_		=15,
	kHDR_oid_		=16,
	kHDR_lid_		=17
};

// Field table field indexes
enum{
	kFLD_id_				=1,
	kFLD_CrID_				=2,
	kFLD_CrDate_			=3,
	kFLD_MdID_				=4,
	kFLD_MdDate_			=5,
	kFLD_kind_				=6,
	kFLD_size_				=7,
	kFLD_len_				=8,
	kFLD_decs_				=9,
	kFLD_state_				=10,
	kFLD_name_				=11,
	kFLD_constKind_			=12,
	kFLD_default_			=13,
	kFLD_format_			=14,
	kFLD_index_				=15,
	kFLD_tbl_id_			=16
};

// Constraint table field indexes
enum{
	kCNST_fid_				=1,
	kCNST_idx_				=2,
	kCNST_value_			=3
};

// Links table field indexes
enum{
	kLNK_id_				=1,
	kLNK_path_				=2,
	kLNK_name_				=3,
	kLNK_tfield_			=4,
	kLNK_bfield_			=5
};

// Std length
#define _FIELDNAME_MAX_		63
#define _kIntLen			16
#define _kDoubleLen			32
#define _kDateTimeLen		24

// Bit field constraint kind signature 
// WARNING : bit constraint is not a "real" constraint : values
// could be outside of the constraints range
#define	_bit				'BIT_'

// Field state
#define	_kFieldEditLock_	0x00000001
#define	_kFieldDeleteLock_	0x00000002
#define	_kFieldMasked_		0x00000008
#define	_kFieldDyn_			0x00000010
#define	_kFieldDynRef_		0x00000020


#define	_kBaseLock_			0x00000001
#define	_kBaseExportLock_	0x00000002


#define	_kVersion300		300
#define	_kVersion310		310

//----------------------------------------------------------------------------

#endif


