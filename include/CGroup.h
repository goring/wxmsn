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

#ifndef _CGROUP_H_
#define _CGROUP_H_


// Includes

#include "wx/wx.h"
#include "arraydeclaration.h"
#include "CContact.h"
#include "Utils.h"

// Classes

class CGroup
{
	public:
		CGroup(const wxString& ID, const wxString& Name, bool Folded);
		~CGroup();
		wxString&		GetID();
		void 			SetID(const wxString& NewID);
		wxString&		GetName();
		void 			SetName(const wxString& Name);
		bool 			GetFolded();
		void			SetFolded(bool Folded);
		wxString		GetHtmlCode(const wxString& HtmlPattern, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURLs, bool MustReparse);
		void 			AddContact(CContact* Contact);
		void			RemoveContact(CContact* RemovedContact);
		int				GetContactCount();
		int				GetOnlineContactCount();
		int				GetOfflineContactCount();
		ContactArray	Contact;

	private:	
		wxString	ID;
		wxString 	Name;
		wxString	HtmlCache;
		bool		Folded;
		bool 		Modified;
};

#endif	//_CGROUP_H_
