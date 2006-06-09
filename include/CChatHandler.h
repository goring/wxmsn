/*  Copyright (c) 2005 Neil "Superna" Armstrong
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

/** 	\file 	CChatHandler.h
	\author Neil "Superna" Armstrong
	\date 	17/12/2005
    $Id$
*/

// Include Protection

#ifndef _CCHATHANDLER_H_
#define _CCHATHANDLER_H_

// Includes
#include <wx/wx.h>
#include "CChatFrame.h"
#include "CChatTab.h"
#include "msncon.h"
#include "arraydeclaration.h"
#include "Events.h"

namespace nsChat
{
    class CChatData;
    WX_DEFINE_ARRAY(CChatData*, ChatDataArray);
    
    class CChatHandler : public wxEvtHandler
    {
        public:
            CChatHandler(wxWindow * parent, nsConsole::wxMsnCon * con);
            // All is contained in the events, RNG infos
            void NewIncomingChat(nsEvents::wxMsnEvent & event);
            // Add a chat, will ask for a SB session
            void NewPersonnalChat(wxArrayString & contacts);
            // Here is for the switchboard session (XFR)
            void AddSbSession(nsEvents::wxMsnEvent & event);
            // Tells of a chat session already exists with this only contact, not conference
            bool ExistsChatSession(wxString & contact);
        
        
        private:
            
            void InitChat(CChatData * Data);
        
            void SocketEvent(wxSocketEvent & evt);
            
            nsConsole::wxMsnCon * m_con;
            wxWindow    * m_parent;
            CChatFrame * m_chatframe;
            //array de connections
            ChatDataArray m_array;
        
            friend class CChatData;
    
        DECLARE_EVENT_TABLE()
    
    };
    
    class CChatCommand : public wxArrayString
    {
        public:
            CChatCommand(const wxString & type, const wxString & param1, const wxString & param2, const wxString & size);
            CChatCommand(const wxString & header);
        
            void AddLine(const wxString & line);
            wxString GetFullData();
            int GetSize();
        
        private:
            int m_size;
            wxString m_header;
    };
    
    class CChatData 
    {
        public:
            CChatData(CChatHandler * parent);
        
            CChatTab * GetChatTab();
            wxSocketClient * GetSocket();
        
        private:
            wxString            m_main_contact;
            wxArrayString    m_invite;
            wxSocketClient  m_sock;
            CChatTab           * m_chattab;
    };
	
} // nsChat
		
#endif
