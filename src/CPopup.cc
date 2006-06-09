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


//Includes

#include "CPopup.h"
#include "Utils.h"

// Defines

#define CLASS CPopup


// Events

BEGIN_EVENT_TABLE(CLASS, wxFrame)
	EVT_TIMER(DISMISS_TIMER, CLASS::OnDismissTimer)
	EVT_PAINT(CLASS::OnPaint)
END_EVENT_TABLE()


// Implementation

CLASS::CLASS(wxWindow* Parent, const wxString& Title, const wxPoint& Position, const wxSize& Size, const wxString& Avatar, const wxSize& AvatarSize, const wxString& Text, bool Fit, const long LifeTime) : wxFrame(Parent, -1, Title, Position, Size, wxFRAME_TOOL_WINDOW|wxTHICK_FRAME|wxCLOSE_BOX)
{
	SetBackgroundColour(wxColor(243, 243, 255));
	AvatarDisplay = new wxWindow(this, -1, wxDefaultPosition, AvatarSize);
	AvatarDisplay->SetBackgroundColour(wxColor(243, 243, 255));
	TextDisplay = new wxStaticText(this, -1, Text);
	
	wxBoxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
	Sizer->Add(AvatarDisplay, 0, wxALIGN_CENTER|wxLEFT|wxTOP|wxBOTTOM, 10);
	Sizer->Add(TextDisplay, 1, wxALIGN_CENTER|wxALL, 10);
	SetSizer(Sizer);
	SetAutoLayout(true);
	
	// Compute maximum width of the popup before it gets clipped by the screen
	wxRect ClientDisplayRect = wxGetClientDisplayRect();
	
	// Fit or not to the content (with respect to the max width)
	if(Fit == true)
	{
		Sizer->SetSizeHints(this);
	}
	else
	{
		SetSize(Size);
		Sizer->SetSizeHints(this);
	}
	
	// Watch if the popup isn't clipped by the bottom or the right of the screen
	int PopupWidth, PopupHeight;
	GetSize(&PopupWidth, &PopupHeight);
	if(Position.x + PopupWidth > ClientDisplayRect.GetWidth())
	{
		Move(ClientDisplayRect.GetWidth() - PopupWidth, -1);
	}
	if(Position.y + PopupHeight > ClientDisplayRect.GetHeight())
	{
		Move(-1, ClientDisplayRect.GetHeight() - PopupHeight);
	}
	
	// Try to load specified picture, else loads a default one
	if(wxFileExists(Avatar) == true)
	{
        wxFileName filename(Avatar);
        wxLogMessage(_("Popup filename : %s"), filename.GetFullPath().c_str());
		AvatarBitmap.LoadFile(filename.GetFullPath(), wxBITMAP_TYPE_PNG);
	}
	else
	{
		wxFileName filename(wxT("resources/avatar.png"));
        wxLogMessage(_("Popup filename : %s"), filename.GetFullPath().c_str());
		AvatarBitmap.LoadFile(filename.GetFullPath(), wxBITMAP_TYPE_PNG);
	}
	
	// Set killer timer if one is desired
	DismissTimer.SetOwner(this, DISMISS_TIMER);
	if(LifeTime > 0)
	{
		DismissTimer.Start(1000, true);
	}

	Show();
}


CLASS::~CLASS()
{
	
}


void CLASS::OnPaint(wxPaintEvent& OnPaint)
{
	wxPaintDC PaintDC(this);
	wxPaintDC WndDC(AvatarDisplay);

	if(AvatarBitmap.Ok() == true && WndDC.Ok() == true)
	{
		double UsW, UsH, AvatarSW, AvatarSH;
		int AvatarDisplayW, AvatarDisplayH;
		AvatarDisplay->GetSize(&AvatarDisplayW, &AvatarDisplayH);
		AvatarSW = wx_static_cast(double, AvatarDisplayW) / wx_static_cast(double, AvatarBitmap.GetWidth());
		AvatarSH = wx_static_cast(double, AvatarDisplayH) / wx_static_cast(double, AvatarBitmap.GetHeight());
        WndDC.GetUserScale(&UsW, &UsH);
		WndDC.SetUserScale(AvatarSW * UsW, AvatarSH * UsH);
		WndDC.DrawBitmap(AvatarBitmap, 0, 0, true);
	}
	
	OnPaint.Skip();
}
		

void CLASS::OnDismissTimer(wxTimerEvent& TimerEvent)
{
	Destroy();
}


void CLASS::OnMouseClick(wxMouseEvent& MouseEvent)
{
	
}


void CLASS::OnMouseMove(wxMouseEvent& MouseEvent)
{
	
}
