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

#include "CGroup.h"
#include "Utils.h"

// Defines

#define CLASS CGroup


// Implementation

CLASS::CLASS(const wxString& NewID, const wxString& NewName, bool NewFolded)
{
	ID = NewID;
	Name = NewName;
	Folded = NewFolded;
	Modified = true;
}


CLASS::~CLASS()
{
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		Contact[i]->RemoveParent(this);
	}
}


wxString& CLASS::GetID()
{
	return ID;
}


void CLASS::SetID(const wxString& NewID)
{
	ID = NewID;
	Modified = true;
}


wxString& CLASS::GetName()
{
	return Name;
}


void CLASS::SetName(const wxString& NewName)
{
	Name = NewName;
	Modified = true;
}


bool CLASS::GetFolded()
{
	return Folded;
}


void CLASS::SetFolded(bool NewFolded)
{
	Folded = NewFolded;
}


wxString CLASS::GetHtmlCode(const wxString& HtmlPattern, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURLs, bool MustReparse)
{
	if(Modified == true || MustReparse == true)
	{
		HtmlCache = HtmlPattern;
		
		if(HtmlCache.Find(wxT("%name%")) != -1)
		{
			HtmlCache.Replace(wxT("%name%"), nsUtils::BuildHtmlFromText(Name, ParseEmoticons, ParseMSNPlusCodes, ParseURLs));
		}
		
		HtmlCache.Replace(wxT("%hash%"), ID);
		HtmlCache.Replace(wxT("%online%"), wxString::Format(wxT("%d"), GetOnlineContactCount()));
		HtmlCache.Replace(wxT("%offline%"), wxString::Format(wxT("%d"), GetOfflineContactCount()));
	}	
	
	return HtmlCache;
}


void CLASS::AddContact(CContact* AddedContact)
{
	AddedContact->AddParent(this);
	Contact.Add(AddedContact);
	Modified = true;
}


void CLASS::RemoveContact(CContact* RemovedContact)
{
	Contact.Remove(RemovedContact);
	RemovedContact->RemoveParent(this);
	Modified = true;
}


int CLASS::GetOnlineContactCount()
{
	int OnlineContactNumber = Contact.GetCount();
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		if(Contact[i]->IsOnFL() == false || Contact[i]->GetStatus() == OFFLINE)
		{
			OnlineContactNumber--;
		}
	}
	
	return OnlineContactNumber;
}


int CLASS::GetOfflineContactCount()
{
	int OfflineContactNumber = 0;
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		if(Contact[i]->IsOnFL() == true && Contact[i]->GetStatus() == OFFLINE)
		{
			OfflineContactNumber++;
		}
	}
	
	return OfflineContactNumber;
}


int CLASS::GetContactCount()
{
	int ContactNumber = Contact.GetCount();
	for(unsigned int i=0; i<Contact.GetCount(); i++)
	{
		if(Contact[i]->IsOnFL() == false)
		{
			ContactNumber--;
		}
	}
	
	return ContactNumber;
}
