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

/** 	\file 	CListEvent.h
	\author Neil "Superna" ARMSTRONG
	\date 	06/01/200
*/

// Include Protection

#ifndef _CLISTEVENT_H_
#define _CLISTEVENT_H_

// Includes
#include <wx/event.h>
#include "defines.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_CLIST, 1)
END_DECLARE_EVENT_TYPES()

namespace nsEvents
{
    class CListEvent : public wxEvent
    {
        public:
            CListEvent(int id = 0);
        
            wxString & GetString() { return m_str; }
	    wxString & GetMail() { return m_str; }
	    wxString & GetPass() { return m_str2; }
	    
	    ONLINE_STATUS GetStatus() { return m_status; }

            void SetString(wxString Str) { m_str = Str; }
	    void SetMail(wxString Str) { m_str = Str; }
	    void SetPass(wxString Str) { m_str2 = Str; }

	    void SetStatus(ONLINE_STATUS status) { m_status = status; }
	    
            virtual wxEvent *Clone() const { return new CListEvent(*this); }

        private:
            wxString            m_str, m_str2;
	     ONLINE_STATUS m_status;
    };
    
    typedef void (wxEvtHandler::*wxCListEventFunction)(CListEvent&);
    
    #define wxCListEventHandler(func)(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCListEventFunction, &func)
    
    #define EVT_CLIST(func)  wx__DECLARE_EVT0(wxEVT_CLIST, wxCListEventHandler(func))
        
} // ns
		
#endif
	
