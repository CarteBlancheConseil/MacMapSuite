//----------------------------------------------------------------------------
// File : strget.h
// Project : MacMapSuite
// Purpose : Header file : Localization utils
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
// 17/01/2006 creation.
//----------------------------------------------------------------------------

#ifndef __strget__
#define __strget__

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

void strget			(	const char* msg_id, 
						char* string);

//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

#define	kMsgID				"id"
#define	kMsgCreator			"creator"
#define	kMsgDTOC			"dtoc"
#define	kMsgModifier		"modifier"
#define	kMsgDTOM			"dtom"
#define	kMsgState			"state"
#define	kMsgBounds			"bounds"
#define	kMsgGeom			"geom"
#define	kMsgName			"name"
#define	kMsgColor			"color"
#define	kMsgSubType			"subtype"
#define	kMsgFlag			"flag"
#define	kMsgDirection		"direction"

#define	kMsgSubTypeDefault	"defaultsubtype"
#define	kMsgBlack			"black"
#define	kMsgWhite			"white"
#define	kMsgRed				"red"
#define	kMsgGreen			"green"
#define	kMsgBlue			"blue"
#define	kMsgCyan			"cyan"
#define	kMsgMagenta			"magenta"
#define	kMsgYellow			"yellow"

#define	kMsgOk				"ok"
#define	kMsgCancel			"cancel"

#define	kMsgUpgradeMsg		"upgrademsg"
#define	kMsgUpgradeExp		"upgradeexp"

//----------------------------------------------------------------------------

#endif
