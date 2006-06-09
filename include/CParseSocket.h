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

/** 	\file 	CParseSocket.h
	\author Neil "Superna" ARMSTRONG
	\date 	24/11/200
    $Id$
*/

// Include Protection

#ifndef _CPARSESOCKET_H_
#define _CPARSESOCKET_H_

// Includes
#include <wx/wx.h>
#include <wx/socket.h>


namespace nsNetwork
{
    class CParseSocket
    {
        public:
            CParseSocket(wxSocketClient * Socket);
        
            void Parse();
        
            size_t Lines();
            size_t LineSize(size_t line);
        
            wxString GetString(size_t line, size_t word);
        
            wxString & operator [] (size_t line);
            
        private:
            wxArrayString       m_vec;
            wxSocketClient *    m_sock;
            wxString            m_null;
            wxString            m_oldbuff;
        
    };
	
} // nsNetwork
		
#endif
	
