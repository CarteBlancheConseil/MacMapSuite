//----------------------------------------------------------------------------
// File : bStdBaseAccessor.cpp
// Project : MacMapSuite
// Purpose : CPP source file : Database base class
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

#include "bStdBaseAccessor.h"

// ---------------------------------------------------------------------------
// Constructeur
// ------------
bStdBaseAccessor	::bStdBaseAccessor(	){
	_vers=0;
}

// ---------------------------------------------------------------------------
// Destructeur
// -----------
bStdBaseAccessor::~bStdBaseAccessor(){
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::read(int o, int f, void* val){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::write(int o, int f, void* val){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::count(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::add(int kind, int len, int decs,	const char *name){
	return(0);
}

// ---------------------------------------------------------------------------
// USE FLD
// -----------
int	bStdBaseAccessor::modify(int f, int kind, int len, int decs, const char* name){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::remove(int f){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_kind(int	f,	int	*k){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_len(int	f,	int	*l){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_decs(int f,	int	*d){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_name(int f,	char* name){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_dummy(int f, char* fmt){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::set_dummy(int f, char* fmt){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_size(int f, int* sz){
	return(true);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::get_id(int f){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::get_id(const char* name){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::get_index(int fid){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::get_index(const char* name){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::count_constraints(int f){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::get_constraints_kind(int f){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_constraint(int f, int n, void* val){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::set_constraint(int f, int n, void* val){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::add_constraint(int f, int kind, void* val){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::rmv_constraint(int f, int n){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::get_constraint_index(int f, void* val){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::set_constraint_index(int f, int from, int to){
	return(false);
}

/*// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::build_constraints(int f){
	return(false);
}*/

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::has_default_value(int f){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_default_value(int f, void* val){
	return(false);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bStdBaseAccessor::set_default_value(int f, void* val){
	return(false);		
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bStdBaseAccessor::is_delprotected(int f){
	return(false);		
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bStdBaseAccessor::set_delprotected(int f, bool b){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::is_writeprotected(int f){
	return(false);
}

// ---------------------------------------------------------------------------
// USE _FLD
// -----------
bool bStdBaseAccessor::set_writeprotected(int f, bool b){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::is_hidden(int f){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::set_hidden(int f, bool b){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::is_dyn(int f){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::is_dyn_ref(int f){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::count_records(){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::kill_record(int r){
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::unkill_record(int r){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::killed_record(int r){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::h_read(int	o,	int	f,	void	*val){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int	bStdBaseAccessor::h_write(int	o,	int	f,	void	*val){	
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::count_param(const char* sub){
	return(0);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_param_name(const char* sub, int idx, char* name){	
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::get_param(const char* sub, const char* name, void** p, int* sz){	
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
bool bStdBaseAccessor::set_param(const char* sub, const char* name, void* p, int sz){	
	return(false);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::add_dyn(const char* path, const char* name, int bse_f, int tbl_f){
	return(-1);
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::signature(){
	return('NULL');
}

// ---------------------------------------------------------------------------
// 
// -----------
int bStdBaseAccessor::version(){
	return(_vers);
}

