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

/** 	\file 	CChatframe.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	29/12/2005
*/

// Includes

#ifdef __WXMAC__
    #define __WXMAC_CARBON__
#endif

#include "CChatFrame.h"
#include "CChatTab.h"
#include "wxMSN.xpm"
#include "tabimages.xpm"


// Namespaces

using namespace std;
using namespace nsChat;


// Define 

#define CLASS CChatFrame
#define FRAMENAME _("Chat")


// Events

BEGIN_EVENT_TABLE(CLASS, wxFrame)
	EVT_NOTEBOOK_PAGE_CHANGED(-1, CLASS::OnTabChange)
END_EVENT_TABLE()


// Implementation

CLASS::CLASS(wxWindow * parent)
    : wxFrame(parent, -1, FRAMENAME, wxDefaultPosition,
              wxSize(600, 400), wxDEFAULT_FRAME_STYLE&~wxCLOSE_BOX), m_currentpage(0)
{    
    wxPanel * m_panel = new wxPanel(this, -1);
	m_notebook = new wxNotebook(m_panel, -1, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);

    wxBoxSizer * panelsizer = new wxBoxSizer(wxHORIZONTAL);
    panelsizer->Add(m_notebook, 1, wxEXPAND | wxALL, 0);
    m_panel->SetSizer(panelsizer); 
    
    SetMinSize(wxSize(500,300));
    SetIcon(wxIcon(wxMSN_xpm));
    
    //Image List
    wxImageList * IList = new wxImageList(16,16,TRUE,1);
	IList->Add(wxIcon(wxmsn_grey));
	IList->Add(wxIcon(wxmsn_red));
	IList->Add(wxIcon(wxmsn_normal));
    IList->Add(wxIcon(wxmsn_green));
	IList->Add(wxIcon(wxmsn_blue));
	m_notebook->AssignImageList(IList);
    
	SetTabStatus(AddNewTab(new CChatTab(m_notebook, this, _("Idle"))), TabStIdle);
    SetTabStatus(AddNewTab(new CChatTab(m_notebook, this, _("Error"))), TabStError);
    SetTabStatus(AddNewTab(new CChatTab(m_notebook, this, _("Connected"))), TabStConnected);
	SetTabStatus(AddNewTab(new CChatTab(m_notebook, this, _("New Message"))), TabStNewMessage);
	SetTabStatus(AddNewTab(new CChatTab(m_notebook, this, _("Writing"))), TabStWriting);
    
    Show();
}

size_t CLASS::AddNewTab(CChatTab * tab)
{
    m_notebook->AddPage(tab, tab->GetTopName(), true);
    //m_tabs.Add(tab);
    
    return m_currentpage++;
}

void CLASS::SetTabStatus(size_t page, const TabStatus status)
{
    if(status <= TabStWriting && status >= TabStIdle)
    {
        m_notebook->SetPageImage(page, status);
    }
}

wxWindow * CLASS::GetTabParent()
{
    return m_notebook;
}

void CLASS::DoTabClose()
{
   int cur = m_notebook->GetSelection();
   if(cur == -1)
   {
       wxLogMessage(_("No Tab Selected !"));
       return;
   }   
   m_notebook->DeletePage(cur);
   //m_tabs.RemoveAt(cur);
   if(m_notebook->GetPageCount()==0)  //m_tabs.IsEmpty())
       Hide();
}

void CLASS::OnTabChange(wxNotebookEvent & event)
{
	wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, TabChangeEvent);
	m_notebook->GetCurrentPage()->AddPendingEvent(evt);
	event.Skip();
}

#undef CLASS
