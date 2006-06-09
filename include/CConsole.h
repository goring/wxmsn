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

/** 	\file 	console.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	09/10/2005
    $Id$
*/

// Include Protection

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

// Includes

#include <wx/wx.h>
#include "defines.h"
#include "Events.h"
#include "CListEvent.h"
#include "msncon.h"
#include "CContactList.h"
#include "CData.h"

namespace nsConsole
{
	
    class CConsole : public wxFrame
    {
        public:
            CConsole(wxEvtHandler * parent, wxMsnCon * msncon, CContactList * clist, CData * data/*, MsnData * msndata*/);
            wxTextCtrl * getConsole() { return m_console; }
            void setFName(const wxString & Str) { m_fname->SetValue(Str); }
            void setPStatus(const wxString & Str) { m_pstatus->SetValue(Str); }
            void setGauge(const int value) { m_gauge->SetValue(value); }
	
        protected:
            
            void OnClose(wxCloseEvent & event);
            void Buttons(wxCommandEvent& event);
            void MsnEvent(nsEvents::wxMsnEvent& event);
            void OnTimer(wxTimerEvent& event);
        
            wxTextCtrl  *   m_console,
                        *   m_mail,
                        *   m_password,
                        *   m_fname,
                        *   m_pstatus;
        
            wxMsnCon    *   m_MSNCon;
            CContactList* m_clist;
            CData		* m_contactdata;
            //MsnData     * m_data;
        
            wxEvtHandler     *   m_pl;
            wxTimer     m_timer;
            wxString	    m_images_path;
            wxGauge     * m_gauge;
        
        DECLARE_EVENT_TABLE()
    };
	
} // nsConsole
		
#endif
