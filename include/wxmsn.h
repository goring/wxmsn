/*  Copyright (c) 2005 Neil "Superna" ARMSTRONG
    This file is part of wxMSN.

    wxMSN is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    wxMSN is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wxMSN; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/** 	\file 	wxmsn.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	14/10/2005
    $Id$
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define APP_NAME    _T("wxMSN")
#define APP_VER       _T("0.01a")

#define MSN_DS _T("messenger.hotmail.com")
#define MSN_DS_PORT _T("1863")

#define PROTO_VER _T("VER %i MSNP11 CVR0\r\n")
#define PROTO_CVR _T("CVR %i 0x0409 linux 2.6.13 amd64 WXMSN alpha1 MSMSGS %s\r\n")
#define PROTO_USR _T("USR %i TWN I %s\r\n")
#define PROTO_FUSR _T("USR %i TWN S %s\r\n")
#define PROTO_SBXFR _T("XFR %i SB\r\n")

#define PROTO_UUX _T("UUX %i %i\r\n%s")

#define PROTO_PRP_MFN _T("PRP %i MFN %s\r\n")

#define MAIL _T("superna@wanadoo.fr")

#define NEXUS _T("nexus.passport.com:https")
#define NEXUS_80 _T("nexus.passport.com:http")
#define NEXUS_REQUEST _T("GET /rdr/pprdr.asp HTTP/1.0\r\n\n\n")

#define  MSN_CHALLENGE_ID _T("CFHUR$52U_{VIX5T")
#define  MSN_CHALLENGE_PROD _T("PROD0101{0RM?UBW")

#endif


/*
Trucs sympas :
m_parent->RequestUserAttention(wxUSER_ATTENTION_INFO);
*/
