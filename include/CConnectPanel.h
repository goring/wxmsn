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


#ifndef _CCONNECTPANEL_H_
#define _CCONNECTPANEL_H_


// Includes

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/xrc/xmlres.h>
#include "defines.h"
#include "CListEvent.h"

namespace nsContact
{
    class CConnectPanel : public wxPanel
    {
		DECLARE_DYNAMIC_CLASS(CConnectPanel)
		DECLARE_EVENT_TABLE()
		
        public:
			CConnectPanel();
			CConnectPanel(wxWindow * parent);
	    	~CConnectPanel();
	
		void SetAnim(bool status);
		void Buttons(wxCommandEvent & event);
		
        private:
			void AvatarAnimNextFrame(wxTimerEvent & event);
		
			wxBitmapButton   * m_avatardisplay;
        	wxComboBox       * m_profilelist;
		    wxChoice         * m_statuslist;
			wxButton         * m_newprofile;
			wxButton         * m_deleteprofile;
			wxButton         * m_modifyprofile;
			wxTextCtrl       * m_password;
			wxCheckBox       * m_keeppassword;
			wxImage            m_avatar;
			float              m_avataranimstep;
			wxTimer            m_animtimer;
    };
	
} // nsContact
		
#endif
