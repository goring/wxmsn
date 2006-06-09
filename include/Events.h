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

/** 	\file  Events.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	13/10/2005
	Local Events
    $Id$
*/

// Include Protection

#ifndef _CMSNEVENT_H_
#define _CMSNEVENT_H_

// Includes
#include <wx/event.h>
#include "defines.h"

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_MSN, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_CHAT, 2)
END_DECLARE_EVENT_TYPES()

#define EVT_MSN(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_MSN, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  (wxMsnEventFunction) (& func ), (wxObject *) NULL ),
#define EVT_CHAT(func)  DECLARE_EVENT_TABLE_ENTRY( wxEVT_CHAT, wxID_ANY, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  (wxChatEventFunction) (& func ), (wxObject *) NULL ),

namespace nsEvents
{
    
    class wxMsnEvent : public wxEvent, public wxArrayString
    {
        public:
            wxMsnEvent(MsnEventTypes EventType = wxMsnEventRefreshList);
            wxMsnEvent(wxString Log);
                    
            wxString & GetString() { return m_str; }
            wxInt32 & GetInt() { return m_int; }
            
            void SetString(wxString Str) { m_str = Str; }
            void SetInt(wxInt32 Num) { m_int = Num; }
                        
            virtual wxEvent *Clone() const { return new wxMsnEvent(*this); }
        
        private:
            wxString    m_str;
            wxInt32     m_int;
        
    };
    
    class wxChatEvent : public wxEvent, public wxArrayString
    {
        public:
            wxChatEvent(ChatEventTypes EventType = wxChatEventAddText);
                    
            wxString & GetString() { return m_str; }
            wxInt32 & GetInt() { return m_int; }
            
            void SetString(wxString Str) { m_str = Str; }
            void SetInt(wxInt32 Num) { m_int = Num; }
                        
            virtual wxEvent *Clone() const { return new wxChatEvent(*this); }
        
        private:
            wxString            m_str;
            wxInt32             m_int;
            
        };
        
    typedef void (wxEvtHandler::*wxMsnEventFunction)(wxMsnEvent&);
    typedef void (wxEvtHandler::*wxChatEventFunction)(wxChatEvent&);
	
} // nsConsole
		
#endif
