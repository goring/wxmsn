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

/** 	\file 	CContactFrame.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	29/12/2005
*/

// Includes

#include "CContactFrame.h"
#include "wxMSN.xpm"


// Namespaces

using namespace std;
using namespace nsContact;
using namespace nsEvents;


// Class 

#define CLASS CContactFrame

#define FRAMENAME _("Connection")

BEGIN_EVENT_TABLE(CLASS, wxFrame)
    EVT_CLOSE(CLASS::OnClose)
    EVT_MENU(-1, CLASS::Menus)
    EVT_IDLE(CLASS::OnIdle)
END_EVENT_TABLE()

CLASS::CLASS(wxEvtHandler * corehandler, CData * contactdata)
    :    wxFrame(NULL, wxID_ANY, FRAMENAME, wxDefaultPosition, wxSize(350, 480), wxDEFAULT_FRAME_STYLE& ~(wxMAXIMIZE_BOX) )
{
	// Creating contacts list
	m_clist = new CContactList(this, contactdata);
    m_clist->SetContactSortingStyle(MIXED);
	m_clist->Redraw();
	
	// Creating profile management panel
	m_connectpanel = new CConnectPanel(this);

	// Show connect panel only
	m_clist->Hide();
	m_connectpanel->Show();
	
	// Creating status bar
	m_statusbar = new wxStatusBar(this, -1);
	
	// Sets the sizer	
	wxBoxSizer * upsizer = new wxBoxSizer(wxHORIZONTAL);
	upsizer->Add(m_clist, 1, wxEXPAND | wxALL);
	upsizer->Add(m_connectpanel, 1, wxALL | wxALIGN_CENTER_VERTICAL, 20);
	
	wxBoxSizer * framesizer = new wxBoxSizer(wxVERTICAL);
	framesizer->Add(upsizer, 1, wxEXPAND);
	framesizer->Add(m_statusbar);
	SetSizer(framesizer);
    framesizer->SetSizeHints(this);
	
	// Creating menu
	m_menu = new wxMenuBar();
    wxSize imageSize(16, 16);
    
    wxMenu * menu = new wxMenu();
	wxMenuItem * menuitem = new wxMenuItem(menu, ExitMenu, _("E&xit"));
    menuitem->SetBitmap(wxArtProvider::GetIcon(wxART_ERROR, wxART_MENU, imageSize));
    menu->Append(menuitem);
    #ifdef __WXMAC__
    menuitem = new wxMenuItem(menu, PrefMenu, _("Prefere&nces"));
    menu->Append(menuitem);
    menuitem = new wxMenuItem(menu, AboutMenu, _("&About"));
    menu->Append(menuitem);
    #endif
    m_menu->Append(menu, FileMenuName);
    
    menu = new wxMenu();
    menuitem = new wxMenuItem(menu, ImageMenu, _("Change Image"));
    menu->Append(menuitem);
	menuitem = new wxMenuItem(menu, FNameMenu, _("Change Friendly Name"));
    menu->Append(menuitem);
    menuitem = new wxMenuItem(menu, PStatusMenu, _("Change Personnal Status"));
    menu->Append(menuitem);
    menu->AppendSeparator();
	menuitem = new wxMenuItem(menu, DisconnectMenu, _("Log &Off"));
    menu->Append(menuitem);
    menu->AppendSeparator();
	menuitem = new wxMenuItem(menu, SetBusyMenu, _("Set Busy"));
    menu->Append(menuitem);
	menuitem = new wxMenuItem(menu, SetAvailableMenu, _("Set Available"));
    menu->Append(menuitem);
    m_menu->Append(menu, AccountMenuName);
    m_menu->EnableTop(AccountMenuID, false);

    menu = new wxMenu();
    #ifndef __WXMAC__
	menuitem = new wxMenuItem(menu, PrefMenu, _("Prefere&nces"));
    menuitem->SetBitmap(wxArtProvider::GetIcon(wxART_QUESTION, wxART_MENU, imageSize));
    menu->Append(menuitem);
    #endif
    m_menu->Append(menu, ToolsMenuName);
    
    menu = new wxMenu();
    #ifndef __WXMAC__
	menuitem = new wxMenuItem(menu, AboutMenu, _("&About"));
    menuitem->SetBitmap(wxArtProvider::GetIcon(wxART_HELP, wxART_MENU, imageSize));
    menu->Append(menuitem);
    #endif
    m_menu->Append(menu, HelpMenuName);

	// For debugging purpose
	menu = new wxMenu();
    menuitem = new wxMenuItem(menu, ByGroup, _("By group"));
	menu->Append(menuitem);
	menuitem = new wxMenuItem(menu, ByStatus, _("By status"));
	menu->Append(menuitem);
	menuitem = new wxMenuItem(menu, Mixed, _("Mixed"));
	menu->Append(menuitem);
	m_menu->Append(menu, wxT("LIST DEBUG"));
	
	SetMenuBar(m_menu);
    SetIcon(wxIcon(wxMSN_xpm));
    
    SetSize(350, 480);
	
	wxEvtHandler * evt = PopEventHandler();
	PushEventHandler(corehandler);
	PushEventHandler(evt);
}

void CLASS::OnClose(wxCloseEvent & event)
{
    wxExit();
}

void CLASS::Buttons(wxCommandEvent & event)
{
    /*switch(event.GetId())
    {
        case wxButtonConnect:
            m_clist->Show();
            m_buttpanel->Hide();
            break;
    }*/
}

void CLASS::Menus(wxCommandEvent & event)
{
    switch(event.GetId())
    {
        case ExitMenu:
            wxExit();
            break;
        case PrefMenu:
            OnConnected();
            break;
        case AboutMenu:
            {
                wxMessageDialog dialog(this, _("wxMsn\nAuthors :\n\tNeil 'Superna' Armstrong\n\tJulien 'Trapamoosch' Enche\n\nwxMsn is a MSN Messenger 'libre' clone for wxWidgets supported platforms.\nSource code is licensed the GPL2+ and can be built under Linux or Mac Os X.\n\nPlease see : http://wxmsn.starnux.net for more informations."), _("About wxMsn"), wxOK);
                dialog.ShowModal();
            }
            break;
		case DisconnectMenu:
		{
			CListEvent Event(wxListEventDisconnect);
			Event.SetEventObject(this);
			ProcessEvent(Event);
		}
		break;
		case SetBusyMenu:
		{
			CListEvent Event(wxListEventNewStatus);
			Event.SetStatus(BUSY);
			Event.SetEventObject(this);
			ProcessEvent(Event);
		}
		break;
		case SetAvailableMenu:
		{
			CListEvent Event(wxListEventNewStatus);
			Event.SetStatus(ONLINE);
			Event.SetEventObject(this);
			ProcessEvent(Event);
		}
		break;
	
		// DEBUG
		case Mixed:
		{
			m_clist->SetContactSortingStyle(MIXED);
			m_clist->Redraw();
			break;
		}
		case ByGroup:
		{
			m_clist->SetContactSortingStyle(BY_GROUP);
			m_clist->Redraw();
			break;
		}
		case ByStatus:
		{
			m_clist->SetContactSortingStyle(BY_STATUS);
			m_clist->Redraw();
			break;
		}
	}
}

void CLASS::OnConnected()
{
	m_connectpanel->SetAnim(false);
	m_connectpanel->Hide();
	m_clist->Show();
	SetTitle(_("Contact list"));
    m_menu->EnableTop(AccountMenuID, true);
	SetStatusText(_("Connected !"));
	Layout();
	RequestUserAttention(wxUSER_ATTENTION_INFO);
}


void CLASS::OnDisconnected()
{
	m_connectpanel->Show();
	m_clist->Hide();
	SetTitle(_("Connection"));
    m_menu->EnableTop(AccountMenuID, false);
	Layout();
}


void CLASS::Connecting(int status)
{
	m_connectpanel->SetAnim(true);
	SetStatusText(_("Connecting") + wxString((wxChar)'.', status));
}


void CLASS::OnError(int code)
{
	m_connectpanel->SetAnim(false);
	SetStatusText(_("Disconnected"));
}


void CLASS::SetStatusText(const wxString & txt)
{
	m_statusbar->SetStatusText(txt);
}


#undef CLASS
