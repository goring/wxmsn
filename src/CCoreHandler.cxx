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

/** 	\file 	CCoreHandler.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	31/12/2005
*/

// Includes

#include <wx/tokenzr.h>
#include "CCoreHandler.h"
#include "defines.h"
#include "Utils.h"
#include "wxMSN.xpm"
#include "CChatHandler.h"
#include "tabimages.xpm"
#include "SCPrefs.h"

// Namespaces

using namespace std;
using namespace nsCore;
using namespace nsEvents;
using namespace nsConsole;
using namespace nsContact;
using namespace nsChat;
using namespace nsUtils;

// Class
#define CLASS CCoreHandler

BEGIN_EVENT_TABLE(CLASS, wxEvtHandler)
    EVT_MSN(CLASS::MsnEvent)
    EVT_CLIST(CLASS::ListEvent)
    EVT_TIMER(wxTimerID, CLASS::OnTimer)
    EVT_TIMER(SongTimer, CLASS::ReadSong)
    EVT_TIMER(IdleTimer, CLASS::IdleEvent)
    EVT_MOTION(CLASS::MouseEvent)
    EVT_SET_FOCUS(CLASS::FocusEvent)
    EVT_TASKBAR_RIGHT_UP(CLASS::TBRight)
    EVT_TASKBAR_LEFT_UP(CLASS::TBLeft)
END_EVENT_TABLE()

CLASS::CLASS()
        : m_contactdata(), m_msncon(this, &m_contactdata)
{
    CreateContactList();
    CreateConsole();
    CreateChat();

    //Data
    m_images_path = RC_PATH;
    m_timer.SetOwner(this, wxTimerID);
    //m_timer.Start(50000);
    m_songtimer.SetOwner(this, SongTimer);
    m_songtimer.Start(5000);
    m_idletimer.SetOwner(this, IdleTimer);
    m_idletimer.Start(IDLETIME);
    lastpsm = _T("[none]");

    //TrayIcon
    m_taskbar = new wxTaskBarIcon();
    m_taskbar->SetIcon(wxIcon(wxmsn_grey), _("wxMSN Disconnected"));
    m_taskbar->SetNextHandler(this);

}

void CLASS::CreateConsole()
{
    m_consoleframe = new CConsole(this, &m_msncon, m_clist, &m_contactdata/*, &m_data*/);
    m_console = m_consoleframe->getConsole();
    m_consoleframe->Show(TRUE);
}

void CLASS::CreateContactList()
{
    m_contact = new CContactFrame(this, &m_contactdata);
    m_clist = m_contact->GetCList();
}

void CLASS::CreateChat()
{
    //m_chat = new CChatHandler(m_contact, &m_msncon);
}

void CLASS::IdleEvent(wxTimerEvent & event)
{
    wxLogMessage(_("Idle Event"));
    m_msncon.SetStatus(IDLE);
}

void CLASS::MouseEvent(wxMouseEvent & event)
{
    m_idletimer.Start(IDLETIME);
}

void CLASS::FocusEvent(wxFocusEvent & event)
{
    m_idletimer.Start(IDLETIME);
}

void CLASS::ListEvent(CListEvent& event)
{

    wxLogMessage(_("Received ListEven ID:%i"), event.GetId());

    switch(event.GetId())
    {
    case wxListEventConnect:
        {
            m_contactdata.SetMail(event.GetMail());
            m_contactdata.SetPassword(event.GetPass());
            m_msncon.Connect();
        }
        break;
    case wxListEventDisconnect:
        {
            m_msncon.Disconnect();
        }
        break;
    case wxListEventDblClick:
        {
            wxLogMessage(_("LIST : Double Click for %s"), event.GetString().c_str());
        }
        break;
    case wxListEventBlock:
        {
            wxLogMessage(_("LIST : Blocking asked for %s"), event.GetString().c_str());
        }
        break;
    case wxListEventAdd:
        break;
    case wxListEventNewFName:
        {
            m_msncon.SetFName(event.GetString());
        }
        break;
    case wxListEventNewStatus:
        {
            m_msncon.SetStatus(event.GetStatus());
        }
        break;
    case wxListEventNewPStatus:
        {
            m_msncon.SetPStatus(event.GetString());
        }
        break;
    }
}

void CLASS::MsnEvent(wxMsnEvent& event)
{
    switch(event.GetId())
    {
    case    wxMsnEventRefreshList:
        {
			if( ! m_contactdata.IsFinalConnected() )
			{
				//First QNG received
				m_timer.Start(50000);
				m_contactdata.SetFinalConnected(true);	
			}
			else
			{
				*m_console << _("Refreshing...\n");
				m_clist->Redraw();
			}
        }
        break;
    case    wxMsnEventConnectionLog:
        {
            *m_console << event.GetString() + _("\n");
        }
        break;
    case    wxMsnEventConnectionFailed:
        {
            *m_console << _("Connection Failed\n");
            m_consoleframe->setGauge(0);
            m_taskbar->SetIcon(wxIcon(wxmsn_red), _("wxMSN Connection Failes"));
            m_contact->OnError(event.GetInt());
			m_timer.Stop();
			m_contactdata.SetFinalConnected(false);
        }
        break;
    case    wxMsnEventConnectionP1:
        {
            m_taskbar->SetIcon(wxIcon(wxmsn_blue), _("wxMSN Connecting"));
            m_consoleframe->setGauge(2);
            m_contact->Connecting(2);
        }
        break;
    case    wxMsnEventConnectionP2:
        {
            m_consoleframe->setGauge(3);
            m_contact->Connecting(3);
        }
        break;
    case    wxMsnEventConnectionP3:
        {
            m_msncon.Sync();
            m_consoleframe->setGauge(4);
            m_contact->Connecting(4);
        }
        break;
    case    wxMsnEventConnectionP4:
        {
            m_msncon.SetStatus();
            m_consoleframe->setGauge(5);
            m_contact->Connecting(5);
        }
        break;
    case    wxMsnEventConnected:
        {
            *m_console << _("Connected\n");
            wxLogMessage(_("New self status %i"), GetNumStatus(event.GetString()));
            m_contactdata.SetContactStatus(wxT("self"), GetNumStatus(event.GetString()));
            m_consoleframe->setGauge(6);
            m_taskbar->SetIcon(wxIcon(wxmsn_normal), _("wxMSN Connected"));
            m_contact->OnConnected();
            if( m_contactdata.IsFinalConnected() )
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventDisconnected:
        {
            *m_console << _("Disconnected\n");
            m_contactdata.Clear();
            m_clist->Redraw();
            m_contact->OnDisconnected();
            m_consoleframe->setGauge(0);
            m_contact->OnError(0);
            m_taskbar->SetIcon(wxIcon(wxmsn_grey), _("wxMSN Disconnected"));
			m_timer.Stop();
			m_contactdata.SetFinalConnected(false);
        }
        break;
    case    wxMsnEventPersoInfo:
        {
            *m_console << wxString::Format(wxT("Personnal Friendly Name: %s\n"),
                                           event.GetString().c_str() );
            wxString Name = event.GetString();
            if(m_contactdata.FindContact(wxT("self")) == NULL)
            {
                m_contactdata.AddContact(wxT("self"), UrlDecode(Name), wxT(""), ONLINE,
                                         m_images_path + wxT("resources/avatar-mid.png"), 0);
            }
            else
            {
                m_contactdata.SetContactName(wxT("self"), UrlDecode(Name));
            }
            m_consoleframe->setFName(UrlDecode(Name));
			if( m_contactdata.IsFinalConnected() )
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventPersoPStatus:
        {
            *m_console << wxString::Format(	_("Personnal Status Message: %s\n"),
                                            event.GetString().c_str() );
            wxString Name = event.GetString();
            m_consoleframe->setPStatus(Name);
            m_contactdata.SetContactPersonalMessage(wxT("self"), Name);
			if( m_contactdata.IsFinalConnected() )	
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventNewGroup:
        {
            if(event.GetCount() < 2)
                break;
            m_contactdata.AddGroup(event[1], UrlDecode(event[0]), false);
            if( m_contactdata.IsFinalConnected() )	
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventNewContact:
        {
            if(event.GetCount() < 3)
                break;

            long ListBit;
            event[1].ToLong(&ListBit);
            wxString Name = event[2];
            if(Name.size()==0)
                Name = event[0];

            wxLogMessage(_("New Contact : %s %i %s"), event[0].c_str(), ListBit,  UrlDecode(Name).c_str());

            m_contactdata.AddContact(event[0], UrlDecode(Name), wxT(""), OFFLINE,
                                     m_images_path + wxT("resources/avatar-small.png"), wx_static_cast(int, ListBit));
            if( m_contactdata.IsFinalConnected() )	
				if(ListBit&1 == 1)
					m_clist->Redraw();

        }
        break;
    case    wxMsnEventAssignGroup:
        {
            wxStringTokenizer Tok(event.GetString(), wxT(";"));
            while(Tok.HasMoreTokens())
            {
                wxString GID = Tok.GetNextToken();
                *m_console << wxString::Format(_("Adding to GID: %s %s\n"), event[0].c_str(), GID.c_str() );
                m_contactdata.PutContactInGroup(event[0], GID);
            }
        }
        break;
    case    wxMsnEventContactConnected:
    case    wxMsnEventContactStatus:
        {
            if(event.GetCount() < 3)
                break;
            wxLogMessage(_("Status Changed : %s %s %s"), event[0].c_str(), event[1].c_str(), event[2].c_str());
            m_contactdata.SetContactName(event[0], UrlDecode(event[2]));
            m_contactdata.SetContactStatus(event[0], GetNumStatus(event[1]));
            if( m_contactdata.IsFinalConnected() )	
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventContactPStatus:
        {
            if(event.GetCount() < 2)
                break;
            m_contactdata.SetContactPersonalMessage(event[0], UrlDecode(event[1]));
            if( m_contactdata.IsFinalConnected() )	
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventContactDisconnected:
        {
            *m_console << wxString::Format(_("Contact Disconnected : %s\n"), event.GetString().c_str() );
            m_contactdata.SetContactStatus(event.GetString(), OFFLINE);
            if( m_contactdata.IsFinalConnected() )	
				m_clist->Redraw();
        }
        break;
    case    wxMsnEventChatInvitation:
        {
            if(event.GetCount() < 5)
                break;
            wxLogMessage(_("New Chat Invitation : [%s %s] to [%s,%s,%s]"), event[3].c_str(), event[4].c_str(), event[1].c_str(), event[0].c_str(), event[2].c_str());
            //m_chat->NewIncomingChat(event[0],event[1],event[2],event[3]);
        }
        break;
    };
}

void CLASS::OnTimer(wxTimerEvent& event)
{
    m_msncon.Refresh();
}

void CLASS::ReadSong(wxTimerEvent& event)
{
    //Song check
    if(m_contactdata.IsConnected())
    switch(SCPrefs::CheckPlayedSong())
    {
        case PrefsNoNewSong: break;
        case PrefsSongError:
        case PrefsNoSong:
        {
            //Remet l'ancien PSM
            if(lastpsm.Cmp(_T("[none]"))!=0)
                m_msncon.SetPStatus(lastpsm);
            lastpsm = _T("[none]");
        } break;
        case PrefsNewSong:
        {
            if(lastpsm.Cmp(_T("[none]"))==0)
                lastpsm = m_contactdata.FindContact(wxT("self"))->GetPersonalMessage();
            m_msncon.SetCurrentMedia(lastpsm, SCPrefs::GetPlayedTitle(), SCPrefs::GetPlayedArtist());
            wxLogMessage(_("New Song played : %s - %s"), 
                SCPrefs::GetPlayedTitle().c_str(), SCPrefs::GetPlayedArtist().c_str());
            //Met la nouvelle chanson
        } break;
    }
}

void CLASS::TBRight(wxTaskBarIconEvent & event)
{
    wxLogMessage(_("TaskBar Right Clicked"));
}

void CLASS::TBLeft(wxTaskBarIconEvent & event)
{
    m_contact->Raise();
}

#undef CLASS
