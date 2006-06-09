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

#include "CContact.h"
#include "Utils.h"


// Defines

#define CLASS CContact


// Implementation

CLASS::CLASS(const wxString& NewAddress, const wxString& NewName, const wxString& NewPersonalMessage, const ONLINE_STATUS NewStatus, const wxString& NewAvatar, int NewListBit)
{
	Address = NewAddress;
	Name = NewName;
	PersonalMessage = NewPersonalMessage;
	Status = NewStatus;
	Avatar = NewAvatar;
	Selected = false;
	CustomStatusState = false;
	ListBit = NewListBit;
	Modified = true;
}


CLASS::~CLASS()
{
	GroupArray ParentCopy = Parent;
	for(unsigned int i=0; i<ParentCopy.GetCount(); i++)
	{
		ParentCopy[i]->RemoveContact(this);
	}
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


wxString& CLASS::GetPersonalMessage()
{
	return PersonalMessage;
}


void CLASS::SetPersonalMessage(const wxString& NewPersonalMessage)
{
	PersonalMessage = NewPersonalMessage;
	Modified = true;
}


void CLASS::SetStatus(const ONLINE_STATUS NewStatus)
{
	Status = NewStatus;
	Modified = true;
}


ONLINE_STATUS CLASS::GetStatus()
{
	return Status;
}


wxString& CLASS::GetAvatar()
{
	return Avatar;
}


void CLASS::SetAvatar(const wxString& NewAvatar)
{
	Avatar = NewAvatar;
	Modified = true;
}


wxString& CLASS::GetAddress()
{
	return Address;
}


void CLASS::SetAddress(const wxString& NewAddress)
{
	Address = NewAddress;
	Modified = true;
}


bool CLASS::IsSelected()
{
	return Selected;
}


void CLASS::SetSelected(bool NewSelected)
{
	Selected = NewSelected;
	Modified = true;
}


bool CLASS::HasCustomStatus()
{
	return CustomStatusState;
}


void CLASS::SetCustomStatus(const wxString& NewCustomStatus)
{
	CustomStatus = NewCustomStatus;
	Modified = true;
}


void CLASS::SetCustomStatusState(bool NewCustomStatusState)
{
	if(CustomStatusState == false && NewCustomStatusState == true)
	{
		SavedName = Name;
	}
	else if(CustomStatusState == true && NewCustomStatusState == false)
	{
		Name = SavedName;
	}
	
	CustomStatusState = NewCustomStatusState;
	Modified = true;
}


wxString& CLASS::GetCustomStatus()
{
	return CustomStatus;
}


void CLASS::SetHomePhoneNumber(const wxString& PhoneNumber)
{
	HomePhoneNumber = PhoneNumber;
	Modified = true;
}


wxString& CLASS::GetHomePhoneNumber()
{
	return HomePhoneNumber;
}


void CLASS::SetMobilePhoneNumber(const wxString& PhoneNumber)
{
	MobilePhoneNumber = PhoneNumber;
	Modified = true;
}


wxString& CLASS::GetMobilePhoneNumber()
{
	return MobilePhoneNumber;
}

void CLASS::SetWorkPhoneNumber(const wxString& PhoneNumber)
{
	WorkPhoneNumber = PhoneNumber;
	Modified = true;
}


wxString& CLASS::GetWorkPhoneNumber()
{
	return WorkPhoneNumber;
}


void CLASS::SetListBit(int NewListBit)
{
	ListBit = NewListBit;
	Modified = true;
}


bool CLASS::IsOnFL()
{
	return (ListBit & 1) == 1;
}


bool CLASS::IsOnAL()
{
	return (ListBit & 2) == 1;
}


bool CLASS::IsOnBL()
{
	return (ListBit & 4) == 1;
}


bool CLASS::IsOnRL()
{
	return (ListBit & 8) == 1;
}


bool CLASS::IsOnPL()
{
	return (ListBit & 16) == 1;
}


wxString CLASS::GetHtmlCode(const wxString& HtmlPattern, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURLs, bool MustReparse)
{
	if(Modified == true || MustReparse == true)
	{
		HtmlCache = HtmlPattern;
		wxString StrippedName = nsUtils::BuildHtmlFromText(Name, false, false, false);
		bool HasCustomStatus = StrippedName.Matches(wxT("*{*}")) == true && Status == AWAY;
		
		if(HtmlCache.Find(wxT("%name%")) != -1)
		{
			if(HasCustomStatus == true)
			{
				wxString RealName = Name.BeforeLast(wxT('{'));
				RealName.Trim();
				HtmlCache.Replace(wxT("%name%"), nsUtils::BuildHtmlFromText(RealName, ParseEmoticons, ParseMSNPlusCodes, ParseURLs));
			}
			else
			{
				HtmlCache.Replace(wxT("%name%"), nsUtils::BuildHtmlFromText(Name, ParseEmoticons, ParseMSNPlusCodes, ParseURLs));
			}
		}
		
		if(HtmlCache.Find(wxT("%pm%")) != -1)
		{
			HtmlCache.Replace(wxT("%pm%"), nsUtils::BuildHtmlFromText(PersonalMessage, ParseEmoticons, ParseMSNPlusCodes, ParseURLs));
		}
	
		if(HtmlCache.Find(wxT("%status%")) != -1)
		{
			if(HasCustomStatus == true)
			{
				wxString RealStatus = (Name.AfterLast(wxT('{'))).BeforeLast(wxT('}'));
				RealStatus.Trim(false);
				RealStatus.Trim(true);
				HtmlCache.Replace(wxT("%status%"), nsUtils::BuildHtmlFromText(RealStatus, ParseEmoticons, false, ParseURLs));
			}
			else
			{
				wxString StatusList[] = 
				{
					_("Online"),
					_("Busy"),
					_("Idle"),
					_("Be right back"),
					_("Away"),
					_("On phone"),
					_("Out to lunch"),
					_("Appear offline"),
					_("Offline")
				};
				
				HtmlCache.Replace(wxT("%status%"), StatusList[Status]);
			}
		}
		
		HtmlCache.Replace(wxT("%address%"), Address);
		
		wxRegEx reAvatar(wxT("%avatar\\([0-9]+,[0-9]+\\)%"));
		while(reAvatar.Matches(HtmlCache) == true)
		{
			wxString AvatarTag = reAvatar.GetMatch(HtmlCache);
			wxString AvatarSize = AvatarTag.Mid(8);
			AvatarSize.Truncate(AvatarSize.Length()-2);
			wxStringTokenizer SizeToken(AvatarSize, wxT(","));
			wxString StrWidth = SizeToken.GetNextToken();
			wxString StrHeight = SizeToken.GetNextToken();
			long Width, Height;
			StrWidth.ToLong(&Width);
			StrHeight.ToLong(&Height);
			HtmlCache.Replace(AvatarTag, wxT("<img src='") + Avatar + wxString::Format(wxT("' width='%d' height='%d'>"), Width, Height));
		}
		
		Modified = false;
	}
	
	return HtmlCache;
}


void CLASS::AddParent(CGroup* AddedParent)
{
	Parent.Add(AddedParent);
}


void CLASS::RemoveParent(CGroup* RemovedParent)
{
	Parent.Remove(RemovedParent);
}


bool CLASS::HasParent()
{
	return (Parent.GetCount()-1>0);
}
