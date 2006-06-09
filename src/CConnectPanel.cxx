/*  Copyright (c) 2006 Julien "Trapamoosch" ENCHE
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


// Includes

#ifdef __WXMAC__
    #define __WXMAC_CARBON__
#endif

#include "CConnectPanel.h"
#include "Utils.h"


// Namespaces

using namespace std;
using namespace nsContact;
using namespace nsEvents;
using namespace nsUtils;

// Define 

#define CLASS CConnectPanel
IMPLEMENT_DYNAMIC_CLASS(CConnectPanel, wxPanel)

// Events

BEGIN_EVENT_TABLE(CLASS, wxPanel)
	EVT_TIMER(wxID_ANY, CLASS::AvatarAnimNextFrame)
	EVT_BUTTON(XRCID("NewProfileButtonId"), CLASS::Buttons)
	EVT_BUTTON(XRCID("ModifyProfileButtonId"), CLASS::Buttons)
	EVT_BUTTON(XRCID("DeleteProfileButtonId"), CLASS::Buttons)
	EVT_BUTTON(XRCID("ConnectButtonId"), CLASS::Buttons)
	EVT_TEXT_ENTER(XRCID("PasswordTextCtrlId"), CLASS::Buttons)
END_EVENT_TABLE()


// Implementation

CLASS::CLASS()
{
}

CLASS::CLASS(wxWindow * parent) : wxPanel(parent, wxID_ANY)
{    
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("ConnectPanel"));
    m_avatardisplay = XRCCTRL(*this, "ConnectButtonId", wxBitmapButton);
    m_profilelist = XRCCTRL(*this, "ProfileSelectionComboId", wxComboBox);
    m_password = XRCCTRL(*this, "PasswordTextCtrlId", wxTextCtrl);
    m_keeppassword = XRCCTRL(*this, "PasswordKeepCheckboxId", wxCheckBox);
    m_statuslist = XRCCTRL(*this, "InitialStatusChoiceId", wxChoice);
    m_newprofile = XRCCTRL(*this, "NewProfileButtonId", wxButton);
    m_modifyprofile = XRCCTRL(*this, "ModifyProfileButtonId", wxButton);
    m_deleteprofile = XRCCTRL(*this, "DeleteProfileButtonId", wxButton);
	
	// Loading a default avatar
	wxFileName filename(RC_LOCATE("avatar.png"));
	m_avatar.LoadFile(filename.GetFullPath(), wxBITMAP_TYPE_PNG);
	m_avatardisplay->SetLabel(wxBitmap(m_avatar));
	
	GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
	
	// Assigns the timer
	m_animtimer.SetOwner(this, wxID_ANY);
	
	// Default account for testing purpose
	m_profilelist->SetValue(wxT("wxmsn@na-prod.com"));
	m_password->SetValue(wxT("superna"));
}


CLASS::~CLASS()
{    
   
}


void CLASS::AvatarAnimNextFrame(wxTimerEvent & event)
{
	m_avataranimstep += 0.3f;
	if(m_avataranimstep > 6.2831853f)
	{
		m_avataranimstep -= 6.2831853f;
	}
	wxImage frame(m_avatar.Copy());
	
	// Weird : wxWidget doesn't copy the alpha data...
	if(m_avatar.HasAlpha() == true)
	{
		// We assume m_avatar lives always longer than frame.
		frame.SetAlpha(m_avatar.GetAlpha(), true);
	}
	
	// We must convert m_avatar mask to Alpha value for frame
	// (because by adding a value to each pixels, they can become all white and it
	// would be impossible to find a good color mask value)
	if(m_avatar.HasMask() == true)
	{
		// Just let wxWidget converts mask to alpha value automagically
		if(m_avatar.HasAlpha() == false)
		{
			frame.InitAlpha();
		}
		// else we must do it
		else
		{	
			unsigned char * alphadata = frame.GetAlpha();
			for(int y=0; y<frame.GetHeight(); y++)
			{
				for(int x=0; x<frame.GetWidth(); x++)
				{
					if(m_avatar.IsTransparent(x, y, 0) == true)
					{
						alphadata[y*frame.GetWidth()+x] = 0;
					}
				}
			}
		}
		frame.SetMask(false);
	}
	
	unsigned char * framedata = frame.GetData();
	unsigned char * avatardata = m_avatar.GetData();
	unsigned long offset = frame.GetWidth() * frame.GetHeight() * 3;
	int add = wx_static_cast(int, (sin(m_avataranimstep)+1) * 64.0f);
	int tmp;
	
	while(offset-- > 0)
	{
		tmp = avatardata[offset] + add;
		framedata[offset] = (tmp > 255) ? 255 : tmp;
	}
	
	m_avatardisplay->SetLabel(wxBitmap(frame));
}


void CLASS::SetAnim(bool status)
{
	if(status && !m_animtimer.IsRunning())
	{
		m_animtimer.Start(100);
	}
	else if(m_animtimer.IsRunning())
	{
		m_animtimer.Stop();
		m_avatardisplay->SetLabel(m_avatar);
	}
}


void CLASS::Buttons(wxCommandEvent & event)
{
	if(event.GetId() == XRCID("ConnectButtonId") || 
	   event.GetId() == XRCID("PasswordTextCtrlId"))
	{
		CListEvent Event(wxListEventConnect);
		Event.SetPass(m_password->GetLineText(0));
		Event.SetMail(m_profilelist->GetValue());
		Event.SetEventObject(this);
		GetParent()->ProcessEvent(Event);
		return;
	}
}
