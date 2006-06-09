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

#ifndef _CDATA_H_
#define _CDATA_H_

// Includes

#include "wx/wx.h"
#include "wx/dynarray.h"
#include "CContactList.h"
#include "CGroup.h"
#include "CContact.h"


// Classes

class CContactList;
	
class CData
{
	friend class CContactList;
	
	public:
		CData();
		~CData();
	
		void SetMail(const wxString & Mail) ;
		void SetPassword(const wxString & Password);
		void SetPassportID(const wxString & passportid);
		
		wxString & GetMail();
		wxString & GetPassword();
		wxString & GetPassportID();
		
		void SetConnected(bool val);
		bool IsConnected();
		bool IsFinalConnected() { return m_finalcon; }
		void SetFinalConnected(bool val) { m_finalcon = val; }
	
		bool AddContact(const wxString& Address,const wxString& Name, const wxString& PersonalMessage, ONLINE_STATUS Status, const wxString& Avatar, int ListBit);
		void RemoveContact(const wxString& Address);
		bool AddGroup(const wxString& ID, const wxString& Name, bool Folded);
		void RemoveGroup(const wxString& ID);
		bool PutContactInGroup(const wxString& Address, const wxString& GroupID);
		bool RemoveContactFromGroup(const wxString& Address, const wxString& GroupID);
		void Clear();
		bool SetContactName(const wxString& Address, const wxString& NewName);
		bool SetContactStatus(const wxString& Address, const ONLINE_STATUS NewStatusStatus);
		bool SetContactCustomStatus(const wxString& Address, const wxString& NewCustomStatus);
		bool SetContactAvatar(const wxString& Address, const wxString& NewAvatar);
		bool SetContactPersonalMessage(const wxString& Address, const wxString& NewPersonalMessage);
		bool SetGroupName(const wxString& ID, const wxString& NewName);
		CGroup* FindGroup(const wxString& ID);
		CContact* FindContact(const wxString& Address);
	
	private:
		CContact*		SelfContact;
		GroupArray		Group;
		GroupArray		StatusGroup;
		ContactArray	Contact;
		CContactList*	LinkedContactList;
		wxString    m_mail,
                        m_password,
                        m_passportid;
		bool m_con;
		bool m_finalcon;
};


#endif	//_CDATA_H_
