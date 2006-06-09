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

/** 	\file 	CGui.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	21/11/2005
    $Id$
*/

// Include Protection

#ifndef _CGUI_H_
#define _CGUI_H_

// Includes

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include "CCoreHandler.h"

namespace nsCore
{
	/*class CFrame : public wxFrame
	{
	  public:
		CFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h);	
		
	  protected:
		wxPanel * m_panel;
        void OnClose(wxCloseEvent & event);
	  
		DECLARE_EVENT_TABLE()
	}; // wxMsnFrame*/
    
    class CGui : public wxApp
	{
	  public:
		virtual bool OnInit();
      		~CGui() { delete m_core; }
	  private:
          //CFrame * frame;
          CCoreHandler * m_core;
	  	  wxLocale m_locale;
	}; // wxMsnGui
	
} // nsGui
		
#endif
