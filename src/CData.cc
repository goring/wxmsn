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


// Includes

#include "CData.h"


// Defines

#define CLASS CData


// Implementation

CLASS::CLASS()
{
	// StatusGroup is a table with 3 groups
	// 1. Online contacts
	// 2. Offline contacts
	// 3. "no parent" group : gathers all contacts wich are not assigned to any group
	CGroup* OnlineGroup = new CGroup(wxT(""), _("Online"), false);
	CGroup* OfflineGroup = new CGroup(wxT(""), _("Offline"), false);
	CGroup* NoParentGroup = new CGroup(wxT(""), _("No group"), false);
	StatusGroup.Add(OnlineGroup);
	StatusGroup.Add(OfflineGroup);
	StatusGroup.Add(NoParentGroup);
	m_finalcon = false;
	
	// Puts some pointers to NULL
	SelfContact = NULL;
}


CLASS::~CLASS()
{
	Clear();
	if(LinkedContactList != NULL)
	{
		LinkedContactList->LinkedData = NULL;
	}
}

void CLASS::SetMail(const wxString & Mail) 
{ 
	m_mail = Mail; 
}

void CLASS::SetPassword(const wxString & Password) 
{ 
	m_password = Password; 
}

void CLASS::SetPassportID(const wxString & passportid)
{ 
	m_passportid = passportid; 
}

wxString & CLASS::GetMail()
{ 
	return  m_mail; 
}

wxString & CLASS::GetPassword()
{ 
	return m_password;
}

wxString & CLASS::GetPassportID() 
{ 
	return m_passportid; 
}

void CLASS::SetConnected(bool val) 
{ 
	m_con = val;
}

bool CLASS::IsConnected() 
{ 
	return m_con; 
}

// Adds an empty group to the list
bool CLASS::AddGroup(const wxString& ID, const wxString& Name, bool Folded)
{
	// Don't add if the group already exists
	if(FindGroup(ID) == NULL)
	{
		CGroup* AddedGroup = new CGroup(ID, Name, Folded);
		if(AddedGroup != NULL)
		{
			Group.Add(AddedGroup);
			return true;
		}
	}
	
	return false;
}


// Removes group from the list
// Doesn't remove the contact in it
void CLASS::RemoveGroup(const wxString& ID)
{
	CGroup* RemovedGroup = FindGroup(ID);
	
	if(RemovedGroup != NULL)
	{
		Group.Remove(RemovedGroup);
		delete RemovedGroup;
		
		for(unsigned int i=0; i<Contact.GetCount(); i++)
		{
			// Adds contacts to "no parent" group if they become orphan
			if(Contact[i]->HasParent() == false)
			{
				StatusGroup[2]->AddContact(Contact[i]);
			}
		}
	}
}


// Adds a contact to the list (added by default in the "no parent" group)
bool CLASS::AddContact(const wxString& Address, const wxString& Name, const wxString& PersonalMessage, const ONLINE_STATUS Status, const wxString& Avatar, int ListBit)
{
	// if contact is self
	if(Address == wxT("self"))
	{
		if(SelfContact != NULL)
		{
			return false;
		}
		
		SelfContact = new CContact(Address, Name, PersonalMessage, Status, Avatar, 0);
		if(SelfContact == NULL)
		{
			return false;
		}
		
		return true;
	}
	
	CContact* AddedContact = new CContact(Address, Name, PersonalMessage, Status, Avatar, ListBit);
	if(AddedContact != NULL)
	{
		Contact.Add(AddedContact);
		
		// Adds contact in the correct status group
		if(Status == OFFLINE)
		{
			StatusGroup[1]->AddContact(AddedContact);
		}
		else
		{
			StatusGroup[0]->AddContact(AddedContact);
		}
		
		// By default, adds contact in the "no parent" group
		StatusGroup[2]->AddContact(AddedContact);
		return true;
	}
	
	return false;
}


// Removes contact from the list
void CLASS::RemoveContact(const wxString& Address)
{
	CContact* RemovedContact = FindContact(Address);

	if(RemovedContact != NULL)
	{
		Contact.Remove(RemovedContact);
		delete RemovedContact;
	}
}


// Assigns contact to a group (and remove it from the "no parent" group)
bool CLASS::PutContactInGroup(const wxString& Address, const wxString& GroupID)
{
	CContact* AddedContact = FindContact(Address);
	if(AddedContact == NULL)
	{
		return false;
	}
	
	CGroup* DestinationGroup = FindGroup(GroupID);
	if(DestinationGroup == NULL)
	{
		return false;
	}
	
	StatusGroup[2]->RemoveContact(AddedContact);
	DestinationGroup->AddContact(AddedContact);
	return true;
}


// Removes contact from a group (this function doesn't delete the contact)
bool CLASS::RemoveContactFromGroup(const wxString& Address, const wxString& GroupID)
{
	CContact* RemovedContact = FindContact(Address);
	if(RemovedContact == NULL)
	{
		return false;
	}
	
	CGroup* DestinationGroup = FindGroup(GroupID);
	if(DestinationGroup == NULL)
	{
		return false;
	}
	
	DestinationGroup->RemoveContact(RemovedContact);
	
	// if the contact becomes orphan, adds it to the "no parent group"
	if(RemovedContact->HasParent() == false)
	{
		StatusGroup[2]->AddContact(RemovedContact);
	}
	
	return true;
}


// Clears the datas
void CLASS::Clear()
{
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		delete Contact[i];
	}
		
	for(unsigned int i=0; i<Group.GetCount(); i++)
	{
		delete Group[i];
	}
	
	Group.Clear();
	Contact.Clear();
	StatusGroup[0]->Contact.Clear();
	StatusGroup[1]->Contact.Clear();
	StatusGroup[2]->Contact.Clear();
	
	if(SelfContact != NULL)
	{
		delete SelfContact;
		SelfContact = NULL;
	}
}


// No comment
CGroup* CLASS::FindGroup(const wxString& ID)
{
	for(unsigned int i=0; i<Group.GetCount(); i++)
	{
		if(Group[i]->GetID().StartsWith(ID))
		{
			return Group[i];
		}
	}
	
	return NULL;
}


// No comment
CContact* CLASS::FindContact(const wxString& Address)
{
	if(Address == wxT("self"))
	{
		return SelfContact;
	}
	
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		if(Address == Contact[i]->GetAddress())
		{
			return Contact[i];
		}
	}
		
	return NULL;
}


bool CLASS::SetContactName(const wxString& Address, const wxString& NewName)
{
	CContact* CurrentContact = (Address == wxT("self")) ? SelfContact : FindContact(Address);
	if(CurrentContact != NULL)
	{
		CurrentContact->SetName(NewName);
		return true;
	}
		
	return false;
}


bool CLASS::SetContactStatus(const wxString& Address, const ONLINE_STATUS NewStatus)
{
	CContact* CurrentContact = (Address == wxT("self")) ? SelfContact : FindContact(Address);
	if(CurrentContact != NULL)
	{
		if(CurrentContact->GetStatus() != OFFLINE && NewStatus == OFFLINE)
		{
			StatusGroup[0]->RemoveContact(CurrentContact);
			StatusGroup[1]->AddContact(CurrentContact);	
		}
		else if(CurrentContact->GetStatus() == OFFLINE && NewStatus != OFFLINE)
		{
			StatusGroup[1]->RemoveContact(CurrentContact);
			StatusGroup[0]->AddContact(CurrentContact);	
		}
		
		CurrentContact->SetStatus(NewStatus);
		return true;
	}
	
	return false;
}


bool CLASS::SetContactCustomStatus(const wxString& Address, const wxString& NewCustomStatus)
{
	CContact* CurrentContact = (Address == wxT("self")) ? SelfContact : FindContact(Address);
	if(CurrentContact != NULL)
	{
		CurrentContact->SetCustomStatus(NewCustomStatus);
		return true;
	}
	
	return false;
}


bool CLASS::SetContactAvatar(const wxString& Address, const wxString& NewAvatar)
{
	CContact* CurrentContact = (Address == wxT("self")) ? SelfContact : FindContact(Address);
	if(CurrentContact != NULL)
	{
		CurrentContact->SetAvatar(NewAvatar);
		return true;
	}
	
	return false;
}


bool CLASS::SetContactPersonalMessage(const wxString& Address, const wxString& NewPersonalMessage)
{
	CContact* CurrentContact = (Address == wxT("self")) ? SelfContact : FindContact(Address);
	if(CurrentContact != NULL)
	{
		CurrentContact->SetPersonalMessage(NewPersonalMessage);
		return true;
	}
	
	return false;
}


bool CLASS::SetGroupName(const wxString& ID, const wxString& NewName)
{
	CGroup* CurrentGroup = FindGroup(ID);
	if(CurrentGroup != NULL)
	{
		CurrentGroup->SetName(NewName);
		return true;
	}
	
	return false;
}
