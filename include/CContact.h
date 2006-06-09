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

#ifndef _CCONTACT_H_
#define _CCONTACT_H_


// Includes

#include "wx/wx.h"
#include "wx/string.h"
#include "defines.h"
#include "arraydeclaration.h"
#include "CGroup.h"

// Classes

class CContact
{
	public:
		CContact(const wxString& NewAddress, const wxString& NewName, const wxString& NewPersonalMessage, const ONLINE_STATUS NewStatus, const wxString& NewAvatar, int NewListBit);

		~CContact();
		wxString&		GetName();
		void 			SetName(const wxString& NewName);
		wxString&		GetPersonalMessage();
		void 			SetPersonalMessage(const wxString& NewPersonalMessage);
		ONLINE_STATUS	GetStatus();
		void 			SetStatus(const ONLINE_STATUS NewStatus);
		wxString&		GetAvatar();
		void 			SetAvatar(const wxString& NewAvatar);
		wxString&		GetAddress();
		void 			SetAddress(const wxString& NewAddress);
		bool 			IsSelected();
		void			SetSelected(bool NewSelected);
		bool 			HasCustomStatus();
		void 			SetCustomStatus(const wxString& NewCustomStatus);
		void 			SetCustomStatusState(bool NewCustomStatusState);
		wxString&		GetCustomStatus();
		wxString		GetHtmlCode(const wxString& HtmlPattern, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURLs, bool MustReparse);
		void			AddParent(CGroup* AddedParent);
		void			RemoveParent(CGroup* RemovedParent);
		bool			HasParent();
		void			SetHomePhoneNumber(const wxString& PhoneNumber);
		wxString&		GetHomePhoneNumber();
		void			SetMobilePhoneNumber(const wxString& PhoneNumber);
		wxString&		GetMobilePhoneNumber();
		void			SetWorkPhoneNumber(const wxString& PhoneNumber);
		wxString&		GetWorkPhoneNumber();
		void 			SetListBit(int NewListBit);
		bool 			IsOnFL();
		bool 			IsOnAL();
		bool 			IsOnBL();
		bool 			IsOnRL();
		bool 			IsOnPL();
		
	private:
		wxString 		Name;
		wxString		SavedName;
		wxString 		PersonalMessage;
		ONLINE_STATUS 	Status;
		wxString	 	Avatar;
		wxString		Address;
		bool			Selected;
		bool 			CustomStatusState;
		wxString		CustomStatus;
		wxString		HomePhoneNumber;
		wxString		WorkPhoneNumber;
		wxString		MobilePhoneNumber;
		GroupArray		Parent;
		int				ListBit;
		wxString		HtmlCache;
		bool			Modified;
};

#endif	//_CCONTACT_H_
