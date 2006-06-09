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

/** 	\file 	console.cxx
	\author 	Neil "Superna" ARMSTRONG
	\date 	09/10/2005
*/

// Includes

#include "CConsole.h"
#include "CContactFrame.h"
#include "Utils.h"
#include "wxMSN.xpm"
#include <wx/taskbar.h>
#include <wx/tokenzr.h>

// Namespaces

using namespace std;
using namespace nsConsole;
using namespace nsUtils;
using namespace nsEvents;
using namespace nsContact;
      
// Class 
#define CLASS CConsole

BEGIN_EVENT_TABLE(CLASS, wxFrame)
    EVT_BUTTON(wxButtonConnect, CLASS::Buttons)
    EVT_BUTTON(wxButtonDisconnect, CLASS::Buttons)
    EVT_BUTTON(wxButtonClear, CLASS::Buttons)
    EVT_BUTTON(wxButtonPStatus, CLASS::Buttons)
    EVT_BUTTON(wxButtonFName, CLASS::Buttons)
    EVT_TEXT_ENTER(wxButtonConnect, CLASS::Buttons)
    EVT_CLOSE(CLASS::OnClose)
//    EVT_MSN(CLASS::MsnEvent)
//    EVT_TIMER(wxTimerID, CLASS::OnTimer)
END_EVENT_TABLE()

CLASS::CLASS(wxEvtHandler * parent, wxMsnCon * msncon, CContactList * clist, CData * data/*, MsnData * msndata*/)
    : wxFrame(NULL, -1, _("Console"), wxPoint(50, 50), wxSize(800, 600), wxDEFAULT_FRAME_STYLE& ~(wxCLOSE_BOX | wxMAXIMIZE_BOX) ), m_MSNCon(msncon),
    m_clist(clist), m_contactdata(data)/*, m_data(msndata)*/, m_pl(parent), m_timer(this, wxTimerID)
{
    SetMinSize(wxSize(700,500));
    SetIcon(wxIcon(wxMSN_xpm));
    
    wxPanel * panel = new wxPanel(this, -1);
    
	wxBoxSizer  *   TotalSizer  = new wxBoxSizer( wxVERTICAL ),
                *   MailSizer   = new wxBoxSizer( wxHORIZONTAL ),
                *   PassSizer   = new wxBoxSizer( wxHORIZONTAL ),
                *   FNameSizer   = new wxBoxSizer( wxHORIZONTAL ),
                *   PStatusSizer   = new wxBoxSizer( wxHORIZONTAL ),
                *   ButtSizer   = new wxBoxSizer( wxHORIZONTAL );
   
	//Controls Initialisation
    m_console	= new wxTextCtrl(	panel, -1, 
									_T("Enter Passport/Password and click Connect !\n"),
                                    wxDefaultPosition, wxSize(200,200),
                                    wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH );
    m_mail		= new wxTextCtrl(   panel, -1, 
									_T("wxmsn@na-prod.com"),
                                    wxDefaultPosition, wxDefaultSize);
    m_password  = new wxTextCtrl(   panel, wxButtonConnect, _T("superna"),
                                    wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
    m_fname		= new wxTextCtrl(   panel, -1, _T(""),
                                    wxDefaultPosition, wxDefaultSize);
    m_pstatus	= new wxTextCtrl(   panel, -1, _T(""),
                                    wxDefaultPosition, wxDefaultSize);
	 
	// Adding controls to Sizers
	// -- Mail
    MailSizer->Add( new wxStaticText(	panel, -1, _T("Email :"), 
										wxDefaultPosition, wxDefaultSize, 
										wxALIGN_CENTER_VERTICAL),
					0, wxALL, 3 );
    MailSizer->Add( m_mail, 1, wxEXPAND | wxLEFT | wxRIGHT, 5 );
	// -- Password
    PassSizer->Add( new wxStaticText(	panel, -1, _T("Password :"), 
										wxDefaultPosition, wxDefaultSize, 
										wxALIGN_CENTER_VERTICAL),
					0, wxALL, 3 );
    PassSizer->Add( m_password, 1, wxEXPAND | wxLEFT | wxRIGHT, 5 );
	// -- Friendly Name
    FNameSizer->Add( new wxStaticText(	panel, -1, _T("Friendly Name :"), 
										wxDefaultPosition, wxDefaultSize, 
										wxALIGN_CENTER_VERTICAL),
					0, wxALL, 3 );
    FNameSizer->Add( m_fname, 1, wxEXPAND | wxLEFT | wxRIGHT, 5 );
    FNameSizer->Add( new wxButton( 	panel, wxButtonFName, _T("Set FName"), 
									wxDefaultPosition, wxDefaultSize, 
									wxBU_EXACTFIT), 
					 0, wxALL, 0 );
    // -- Personnal Status
    PStatusSizer->Add( 	new wxStaticText(	panel, -1, _T("Personnal Status :"), 
											wxDefaultPosition, wxDefaultSize, 
											wxALIGN_CENTER_VERTICAL),
						0, wxALL, 3 );
    PStatusSizer->Add( m_pstatus, 1, wxEXPAND | wxLEFT | wxRIGHT, 5 );
    PStatusSizer->Add(	new wxButton( 	panel, wxButtonPStatus, _T("Set PStatus"), 
										wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 
						0, wxALL, 0 );
    
	// -- Buttons (Connect, Disconnect, Clear)
    ButtSizer->Add(	new wxButton( 	panel, wxButtonConnect, _T("Connect"), 
									wxDefaultPosition, wxDefaultSize, 
									wxBU_EXACTFIT), 
					0, wxALL, 1 );
    ButtSizer->Add( new wxButton( 	panel, wxButtonDisconnect, _T("Disconnect"), 
									wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 
					0, wxALL, 1 );
    ButtSizer->Add( new wxButton( 	panel, wxButtonClear, _T("Clear Console"), 
									wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 
					0, wxALL, 1 );
    ButtSizer->Add( m_gauge = new wxGauge(panel, -1, 6),
                    1, wxEXPAND | wxLEFT | wxRIGHT, 1);

	// Adding all Sizers to the Global Sizer
    TotalSizer->Add( MailSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3 );
    TotalSizer->Add( PassSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3 );
    TotalSizer->Add( FNameSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3 );
    TotalSizer->Add( PStatusSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3 );
    TotalSizer->Add( ButtSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3);
    TotalSizer->Add( m_console, 1, wxEXPAND | wxALL, 3 );
    
    panel->SetSizer( TotalSizer );
    
    //m_MSNCon = new wxMsnCon(&m_data, this);
    
    TotalSizer->SetSizeHints( panel );
    
    /*CContactFrame * CList = new CContactFrame(this, &m_contactdata);
    m_clist = CList->GetCList();
    CList->Show();
    CList->Raise();*/
}

void CLASS::OnClose(wxCloseEvent & event)
{
    wxExit();
}

void CLASS::Buttons(wxCommandEvent& event)
{
	//Retrieving Data when Buttons are Clicked
    /*m_data->SetMail(m_mail->GetLineText(0));
    m_data->SetPassword(m_password->GetLineText(0));*/
    
    switch(event.GetId())
    {
        case wxButtonConnect:
	{
            //m_MSNCon->Connect();
		CListEvent Event(wxListEventConnect);
		Event.SetMail(m_mail->GetLineText(0));
		Event.SetPass(m_password->GetLineText(0));
		Event.SetEventObject( this );
		m_pl->ProcessEvent( Event );
	} break;
        case wxButtonDisconnect:
            //m_MSNCon->Disconnect();
	{
		CListEvent Event(wxListEventDisconnect);
		//wxMsnEvent Event(_T("A marche la"));
        Event.SetEventObject( this );
        m_pl->ProcessEvent( Event );
    }    
	break;
		case wxButtonClear:
            m_console->Clear();
            break;
        case wxButtonPStatus:
        {
           /* *m_console << _T("Changes Personnal Status to : ") << m_pstatus->GetLineText(0) << _T("\n");
            m_MSNCon->SetPStatus(m_pstatus->GetLineText(0));*/
		CListEvent Event(wxListEventNewPStatus);
		Event.SetEventObject( this );
		Event.SetString(m_fname->GetLineText(0));
		m_pl->ProcessEvent( Event );
        } break;
        case wxButtonFName:
        {
            /*wxString encoded = UrlEncode(m_fname->GetLineText(0));
            *m_console << _T("Changes Friendly Name to : ") << encoded << _T("\n");
            m_MSNCon->SetFName(encoded);*/
		CListEvent Event(wxListEventNewFName);
		Event.SetEventObject( this );
		Event.SetString(UrlEncode(m_fname->GetLineText(0)));
		m_pl->ProcessEvent( Event );
        } break;
            
    };
}



#undef CLASS
