/*  Copyright (c) 2005 Julien "Trapamoosch" ENCHE
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

/*
    $Id$
*/

#ifndef _CPOPUP_H_
#define _CPOPUP_H_


// Includes

#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/bitmap.h>
#include <wx/frame.h>
#include <wx/filename.h>
#include "defines.h"

// Classes

class CPopup : public wxFrame
{
	public:
		CPopup(wxWindow* Parent, const wxString& Title, const wxPoint& Position, const wxSize& Size, const wxString& Avatar, const wxSize& AvatarSize, const wxString& Text, bool Fit, const long LifeTime);
		~CPopup();
	
	protected:
		void OnDismissTimer(wxTimerEvent& TimerEvent);
		void OnMouseClick(wxMouseEvent& MouseEvent);
		void OnMouseMove(wxMouseEvent& MouseEvent);
		void OnPaint(wxPaintEvent& PaintEvent);
	
	private:
		wxWindow* 		AvatarDisplay;
		wxStaticText*	TextDisplay;
		wxTimer			DismissTimer;
		wxBitmap		AvatarBitmap;
	
	DECLARE_EVENT_TABLE()
};


#endif	//_CHTMLPOPUP_H_
