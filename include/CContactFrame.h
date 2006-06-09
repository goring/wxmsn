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

/** 	\file 	CContactFrame.h
	\author Neil "Superna" ARMSTRONG
	\date 	29/12/2005
    $Id$
*/

// Include Protection

#ifndef _CCONTACTFRAME_H_
#define _CCONTACTFRAME_H_

// Includes
#include <wx/wx.h>
#include <wx/artprov.h>
#include "CContactList.h"
#include "CConnectPanel.h"
#include "defines.h"
#include "CListEvent.h"


namespace nsContact
{
    class CContactFrame : public wxFrame
    {
        public:
            CContactFrame(wxEvtHandler * corehandler, CData * contactdata);
            CContactList * GetCList() { return  m_clist; }
            
			void OnConnected();
			void OnDisconnected();
			void Connecting(int status);
			void OnError(int code);
        	void SetStatusText(const wxString & txt);
			
        private:
            void OnClose(wxCloseEvent & event);
            void Menus(wxCommandEvent & event);
            void Buttons(wxCommandEvent & event);
        
			CConnectPanel   * m_connectpanel;
            CContactList    * m_clist;
            wxPanel         * m_buttpanel;
        	wxMenuBar       * m_menu;
			wxStatusBar     * m_statusbar;
        DECLARE_EVENT_TABLE()
    };
	
} // nsContact

#endif
