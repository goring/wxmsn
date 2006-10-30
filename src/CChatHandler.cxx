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

/** 	\file 	CChatHandler.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	25/12/2005
*/

// Includes

#include "CChatHandler.h"

// Namespaces

using namespace std;
using namespace nsChat;

// Class 
#define CLASS CChatHandler

BEGIN_EVENT_TABLE(CLASS, wxEvtHandler)
    EVT_SOCKET(-1, CLASS::SocketEvent)
END_EVENT_TABLE()

CLASS::CLASS(wxWindow * parent, nsCore::CConnec * con)
    : m_con(con), m_parent(parent)
{
    //Creates the Chat Frame
    m_chatframe = new CChatFrame(m_parent);
}
void CLASS::NewIncomingChat(wxString & contact, wxString & infos)
{
    
}

void CLASS::NewPersonnalChat(wxString & contact)
{
    /*CChatData data = new CChatData(this);
    
    m_array.Add(data);
    
    m_chatframe->AddNewTab(data.GetChatTab());*/
}

void CLASS::SocketEvent(wxSocketEvent & evt)
{
    //Retrieves the CChatData (no more id stories)
    CChatData * ChatData = (CChatData) evt.GetClientData();
    
    //Checks 
    switch(evt.GetSocketEvent())
    {
        case wxSOCKET_LOST:
        {
            //Lost connection
        }break;
        case wxSOCKET_INPUT :
            //May check Socket
            break;
        default:
            //Unknown event type
            break;
    }
}

#undef CLASS
#define CLASS CChatData

/*
        class CChatData 
        {
            public:
                CChatData();
            
                CChatTab * GetChatTab();
                wxSocketClient * GetSocket();
            
            private:
                wxString            m_main_contact;
                wxArrayString    m_invite;
                wxSocketClient  m_sock;
                CChatHandler    m_parent;
                CChatTab           m_chattab;
        };
*/

CLASS::CLASS(CChatHandler * parent)
    
{
    m_sock.SetClientData(this);
    m_sock.SetEventHandler(parent);
    
    m_chattab = new CChatTab(m_chatframe->GetTabParent(), m_chatframe, "New");
}

CChatTab * CLASS::GetChatTab()
{
    return m_chattab;
}

wxSocketClient * CLASS::GetSocket()
{
    return &m_sock;
}

#undef CLASS
#define CLASS CChatCommand
/*
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
*/
CLASS::CChatCommand(const wxString & type, const wxString & param1, const wxString & param2, const wxString & size)
{
    m_header = type + " " + param1 + " " + param2 + size;
}
CLASS::CChatCommand(const wxString & header)
    : m_header(header) {}

void CLASS::AddLine(const wxString & line)
{
    
}

wxString CLASS::GetFullData()
{
    
}

int CLASS::GetSize()
{
    
}



#undef CLASS
