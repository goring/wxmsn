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

/** 	\file 	CChatTab.h
	\author Neil "Superna" ARMSTRONG
	\date 	29/11/2005
    $Id$
*/

// Include Protection

#ifndef _CCHATTAB_H_
#define _CCHATTAB_H_

// Includes

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/splitter.h>
#include <wx/event.h>
#include "defines.h"
#include "CChatArea.h"
#include "CEntryText.h"

namespace nsChat
{
    class CChatFrame;
    
    class CChatTab : public wxSplitterWindow
    {
        public:
		    CChatTab(wxWindow * parent, CChatFrame * chatframe, const wxString & topname);         
            
            wxString & GetTopName();
            void SetTopName(wxString str);
        
            void AddText(const wxString & address, const wxString & txt);
            void AddMessage(const wxString & txt);
            
	    private:
		
            void OnCloseTab(wxCommandEvent & event);
            void OnSendText(wxCommandEvent & event);
			void OnTabShow(wxCommandEvent & event);

            CChatFrame      * m_chatframe;
            wxSocketClient  * m_sock;
            CChatArea       * m_text;
			CEntryText      * m_entry;
			wxButton        * m_send;
			wxStatusBar     * m_statusbar;
            wxString        m_topname;

            DECLARE_EVENT_TABLE()
    };
} // nsChat
		
#endif
