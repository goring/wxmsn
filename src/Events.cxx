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

/** 	\file 	Events.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	06/01/2005
*/

// Includes

#include "Events.h"

// Namespaces

using namespace std;
using namespace nsEvents;

DEFINE_LOCAL_EVENT_TYPE(wxEVT_MSN)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_CHAT)

// Class 
#define CLASS wxMsnEvent

wxMsnEvent::wxMsnEvent(MsnEventTypes EventType)
                : wxEvent(EventType, wxEVT_MSN) {}
wxMsnEvent::wxMsnEvent(wxString Log)
                : wxEvent(wxMsnEventConnectionLog, wxEVT_MSN), m_str(Log) {}

#undef CLASS
                    
#define CLASS wxChatEvent

wxChatEvent::wxChatEvent(ChatEventTypes EventType)
                : wxEvent(EventType, wxEVT_CHAT) {}

#undef CLASS
