//----------------------------------------------------------------------------
// File : bStdTable.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Table base class
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
// 13/01/2006 creation.
//----------------------------------------------------------------------------

#include "bStdTable.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdTable::bStdTable(	double reso,
						double ox,
						double oy,
						int tsrid,
						int asrid,
						int* status){
	_reso=reso;
	_ox=ox;
	_oy=oy;
	_tsrid=tsrid;
	_asrid=asrid;
	_tp=NULL;
	_ap=NULL;
bool	init;
	if(_tsrid>=0){
		_tp=new bStdProj(_tsrid,&init);
		if(init==false){
			*status=-100;
			delete _tp;
			_tp=NULL;
		}
	}
	if(_asrid>=0){
		_ap=new bStdProj(_asrid,&init);
		if(init==false){
			*status=-101;
			delete _ap;
			_ap=NULL;
		}
	}	
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdTable::~bStdTable(){
	if(_tp){
		delete _tp;
	}
	if(_ap){
		delete _ap;
	}
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::KillRecord(int	o){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::UnkillRecord(int	o){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::RecordKilled(int	o){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::CountRecords(){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::CountFields(){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::AddField(const char *nam, int sign, int length, int decs){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::RmvField(int f){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::ChgField(int	f, const char	*nam, int sign, int length, int decs){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::FieldSign(int	f,	int *sign){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::FieldName(int	f,	char *name){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::FieldDecimals(int f, int* decs){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::FieldLength(int f, int* len){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::FieldSize(int f, int* sz){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::ReadVal(int	o,	int	f, void*	val){
	return(0);	
}

/*// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::AllocVal(int	o,	int	f, void**	val){
	return(0);	
}*/

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::WriteVal(int	o,	int	f, void*	val){
	return(0);	
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdTable::Pack(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
void	bStdTable::transform_a2t(dvertices* vxs){
	if(transform_ready()==false){
		return;
	}
	_tp->transform(*_ap,vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
void	bStdTable::transform_t2a(dvertices* vxs){
	if(transform_ready()==false){
		return;
	}
	_ap->transform(*_tp,vxs);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdTable::transform_ready(){
	if(_tsrid==_asrid){
		return(false);
	}
	if(_tp==NULL){
		return(false);
	}
	if(_ap==NULL){
		return(false);
	}
	return(true);
}
