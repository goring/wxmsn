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

#ifndef _CCONTACTLIST_H_
#define _CCONTACTLIST_H_


// Includes

#include "wx/wx.h"
#include "CHtmlWin.h"
#include "defines.h"
#include "CData.h"
#include "CGroup.h"
#include "CContact.h"
#include "CPopup.h"
#include "config.h"
#include "Events.h"
#include "CListEvent.h"


// Classes

class CData;

//namespace nsContact
//{
	class CContactList : public CHtmlWindow
	{
		friend class CData;
			
		public:
			CContactList(wxWindow* Parent, CData* Data);
			~CContactList();
		
			void SetContactHtmlPattern(const wxString& Pattern);
			void SetSelfHtmlPattern(const wxString& Pattern);
			void SetGroupHtmlPattern(const wxString& Pattern);
			void SetBackground(unsigned char R, unsigned char G, unsigned char B);
			void SetBackground(const wxString& Image);
			void SetContactSortingStyle(const CONTACT_SORTING_STYLE NewSortingStyle);
			void SetSelectedContactColor(unsigned char R, unsigned char G, unsigned char B);
			void SetNormalContactColor(unsigned char R, unsigned char G, unsigned char B);
			void SetOfflineContactColor(unsigned char R, unsigned char G, unsigned char B);
			void SetParseMSNPlusCodes(bool Parse);
			void SetParseEmoticons(bool Parse);
			void SetParseURLs(bool Parse);
			void SetDisplayEmptyGroups(bool Display);
			void Redraw();
			void Clear();
					
		protected:
			void OnLeftClick(wxMouseEvent& MouseEvent);
			void OnRightClick(wxMouseEvent& MouseEvent);
            void OnDoubleClick(wxMouseEvent& MouseEvent);
			void OnMouseMove(wxMouseEvent& MouseEvent);
			void OnContactHover(wxTimerEvent& TimerEvent);
			void OnFocusChange(wxFocusEvent& FocusEvent);
			void OnSize(wxSizeEvent& SizeEvent);
			void OnLinkClicked(const CHtmlLinkInfo& ClickedLink);
			void OnMenu(wxCommandEvent& MenuEvent);
		
		private:
			CData*			LinkedData;

			wxString		SelfHtmlPattern;
			wxString		ContactHtmlPattern;
			wxString		GroupHtmlPattern;
			wxString		Background;
			wxString		NormalContactColor;
			wxString		SelectedContactColor;
			wxString		OfflineContactColor;
			CContact*		SelectedContact;
			CContact*		HoveredContact;
			wxTimer			HoverTimer;
			CPopup*			ContactInfoPopup;
			bool			ParseMSNPlusCodes;
			bool			ParseEmoticons;
			bool			ParseURLs;
			bool 			MustReparse;
			bool			DisplayEmptyGroups;
			wxString		BrowserCommand;
			GroupArray		DisplayedGroups;
			ContactArray	DisplayedContacts;
			CContact*		DisplayedSelf;
			wxString		DrawContact(CContact* DrawedContact);
			
			CONTACT_SORTING_STYLE SortingStyle;
		
			static int wxCMPFUNC_CONV CompareContact(CContact** C1, CContact** C2)
			{
				if((*C1)->GetStatus() > (*C2)->GetStatus())
				{
					return 1;
				}
				else if((*C1)->GetStatus() < (*C2)->GetStatus())
				{
					return -1;
				}
				
				return (*C1)->GetName().Cmp((*C2)->GetName());
			}
			
			static int wxCMPFUNC_CONV CompareGroup(CGroup** G1, CGroup** G2)
			{
				return (*G1)->GetName().Cmp((*G2)->GetName());
			}
			
			void CreateLayout();
			DECLARE_EVENT_TABLE()
	};
//}

#endif	//_CCONTACTLIST_H_
