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

/** 	\file 	CGui.cxx
	\author 	Neil "Superna" ARMSTRONG
	\date 	21/11/2005
*/

// Includes

#include "CGui.h"
#include "Utils.h"
#include "wxmsn.h"
#include "SCPrefs.h"

// Namespaces

using namespace std;
using namespace nsCore;
using namespace nsConsole;
using namespace nsUtils;

// Class CGui
#define CLASS CGui

IMPLEMENT_APP(CLASS)

bool CLASS::OnInit()
{
	//LogWindow
    new nsUtils::CLogWindow(NULL, _("Log Window"));
    wxInitAllImageHandlers();
    CPrefs::Init();
    SCPrefs::Init();

    /*frame = new CFrame((wxFrame *) NULL, 
                                        _("Console"),
                                        50, 50, 
                                        800, 600 );
	frame->Show(TRUE);
	SetTopWindow(frame);*/
    new nsChat::CChatFrame(NULL);
    
    
    #ifdef __WXMAC__

    wxApp::s_macAboutMenuItemId = nsContact::AboutMenu;
    wxApp::s_macPreferencesMenuItemId = nsContact::PrefMenu;
    wxApp::s_macHelpMenuTitleName = HelpMenuName;
    
    #endif 
    
    SetUseBestVisual(true);
    SetAppName(APP_NAME);
    SetVendorName(APP_NAME);
    SetClassName(APP_NAME);
    
	m_locale.Init();
	wxLocale::AddCatalogLookupPathPrefix(RC_PATH + wxT("."));
	wxLocale::AddCatalogLookupPathPrefix(RC_PATH + wxT(".."));
	wxLocale::AddCatalogLookupPathPrefix(RC_PATH + wxT("../po/test"));
	
	switch(m_locale.GetLanguage())
	{
		case wxLANGUAGE_FRENCH:
			m_locale.AddCatalog(wxT("fr"));
			break;
	}
	
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(RC_PATH + wxT("resources/xrc/ConnectPanel.xrc"));
	
    m_core = new CCoreHandler();
    SetTopWindow(m_core->getContactList());
    m_core->getContactList()->Show();
	return true;
}

#undef CLASS

// Class CFrame
#define CLASS CFrame
/*
BEGIN_EVENT_TABLE(CLASS, wxFrame)
    EVT_CLOSE(CLASS::OnClose)
END_EVENT_TABLE()

CLASS::CLASS(wxFrame *frame, const wxChar *title, int x, int y, int w, int h)
    : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h) )
{
	SetMinSize(wxSize(w-100,h-100));
    m_panel = new CConsole( this, 10, 10, 300, 100 );
    SetIcon(wxIcon(wxMSN_xpm));
}

void CLASS::OnClose(wxCloseEvent & event)
{
    wxExit();
}
*/
#undef CLASS
