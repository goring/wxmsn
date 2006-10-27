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

/** 	\file 	CCoreHandler.h
	\author Neil "Superna" ARMSTRONG
	\date 	31/12/2005
*/

// Include Protection

#ifndef _CCOREHANDLER_H_
#define _CCOREHANDLER_H_

// Includes
#include <wx/wx.h>
#include <wx/taskbar.h>
#include "CConsole.h"
#include "CContactList.h"
#include "CContactFrame.h"
#include "CChatHandler.h"
#include "CData.h"
#include "CConnec.h"
#include "Events.h"
#include "CListEvent.h"

namespace nsCore
{
    class CCoreHandler : public wxEvtHandler
    {
        public:
            CCoreHandler();
	    ~CCoreHandler() { delete m_taskbar; }

            nsConsole::CConsole * getConsole() { return m_consoleframe; }
            nsContact::CContactFrame * getContactList() { return m_contact; }
        
        private:
            
            void CreateConsole();
            void CreateContactList();
            void CreateChat();
        
            void MsnEvent(nsEvents::wxMsnEvent& event);
            void ListEvent(nsEvents::CListEvent& event);
            void OnTimer(wxTimerEvent& event);
            void ReadSong(wxTimerEvent& event);
            void MouseEvent(wxMouseEvent & event);
            void FocusEvent(wxFocusEvent & event);
            void IdleEvent(wxTimerEvent & event);
            void TBRight(wxTaskBarIconEvent & event);
            void TBLeft(wxTaskBarIconEvent & event);
        
            CData		m_contactdata;
            nsConsole::wxMsnCon    m_msncon;
        
            wxTextCtrl * m_console;
            wxTaskBarIcon  * m_taskbar;
            nsContact::CContactFrame * m_contact;
            nsConsole::CConsole     *   m_consoleframe;
            nsChat::CChatHandler  * m_chat;
            CContactList* m_clist;
            wxTimer     m_timer, m_songtimer, m_idletimer;
            wxString	    m_images_path, lastpsm;
        
        DECLARE_EVENT_TABLE()
    };
	
} // ns
		
#endif
	
