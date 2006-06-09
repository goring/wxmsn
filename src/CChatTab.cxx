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

/** 	\file 	CChatTab.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	29/11/2005
*/

// Includes

#include "CChatTab.h"
#include "CChatFrame.h"


// Namespaces

using namespace std;
using namespace nsChat;


// Define

#define CLASS CChatTab


// Events

BEGIN_EVENT_TABLE(CLASS, wxSplitterWindow)
	EVT_TEXT_ENTER(EntryTextControl, CLASS::OnSendText)
	EVT_MENU(TabChangeEvent, CLASS::OnTabShow)
    EVT_BUTTON(SendButton, CLASS::OnSendText)
END_EVENT_TABLE()
    
	
// Implementation

CLASS::CLASS(wxWindow * parent, CChatFrame * chatframe, const wxString & topname)
    : wxSplitterWindow(parent, -1), m_chatframe(chatframe), m_topname(topname)
{
	// Top panel
	wxPanel * toppanel = new wxPanel(this, -1);	
	wxBoxSizer * toppanelsizer = new wxBoxSizer(wxHORIZONTAL);
	m_text = new CChatArea(toppanel);
	toppanelsizer->Add(m_text, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 3);
	toppanel->SetSizer(toppanelsizer);
	toppanelsizer->SetSizeHints(toppanel);
	
	// Bottom panel
	wxPanel * bottompanel = new wxPanel(this, -1);
	m_send = new wxButton(bottompanel, SendButton, _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_entry = new CEntryText(bottompanel, EntryTextControl); 
	wxBoxSizer * bottompanelsizer1 = new wxBoxSizer(wxHORIZONTAL);
    bottompanelsizer1->Add(m_entry, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 3);
    bottompanelsizer1->Add(m_send, 0, wxCENTER | wxRIGHT | wxBOTTOM, 3);
	
	m_statusbar = new wxStatusBar(bottompanel, -1, 0);
	m_statusbar->SetFieldsCount(1);

	wxBoxSizer * bottompanelsizer = new wxBoxSizer(wxVERTICAL);
    bottompanelsizer->Add(bottompanelsizer1, 1, wxEXPAND);
    bottompanelsizer->Add(m_statusbar);
	
	bottompanel->SetSizer(bottompanelsizer);
    bottompanelsizer->SetSizeHints(bottompanel);
	
	m_statusbar->SetStatusText(_("Put status here"));
	
	// Configuring splitter window
	SplitHorizontally(toppanel, bottompanel, -10);
	SetMinimumPaneSize(10);
	SetSashGravity(1.0);
}

wxString & CLASS::GetTopName()
{
    return m_topname;
}

void CLASS::SetTopName(wxString str)
{
    m_topname = str;
}

void CLASS::OnCloseTab(wxCommandEvent & evt)
{
    m_chatframe->DoTabClose();
}
 

void CLASS::OnSendText(wxCommandEvent & event)
{
	if(m_entry->GetValue().IsEmpty() == false)
	{
		AddText(wxT("you"), m_entry->GetValue());
		m_entry->SetValue(wxEmptyString);
	}
}


void CLASS::OnTabShow(wxCommandEvent & event)
{
	m_entry->SetFocus();
}



void CLASS::AddText(const wxString & address, const wxString & txt)
{
    m_text->AddText(txt);
}


void CLASS::AddMessage(const wxString & txt)
{
    m_statusbar->SetStatusText(txt);
}


#undef CLASS
