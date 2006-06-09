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

/** 	\file 	CChatframe.h
	\author Neil "Superna" ARMSTRONG
	\date 	29/11/2005
    $Id$
*/

// Include Protection

#ifndef _CCHATFRAME_H_
#define _CCHATFRAME_H_

// Includes
#include <wx/wx.h>
#include <wx/imaglist.h>
#include "CContact.h"

namespace nsChat
{
    class CChatTab;
    WX_DEFINE_ARRAY(CChatTab *, TabArray);
    
    class CChatFrame : public wxFrame
    {
        public:
		    CChatFrame(wxWindow * parent);
	    
            void DoTabClose();
            size_t AddNewTab(CChatTab * tab);
        
            void SetTabStatus(size_t page, const TabStatus status);
        
            wxWindow * GetTabParent();
        
        private:
            void OnTabChange(wxNotebookEvent & event);
		
            wxNotebook  * m_notebook;
            //TabArray      m_tabs;
            size_t        m_currentpage;
		
			DECLARE_EVENT_TABLE()
    };
	
} // nsChat
		
#endif
