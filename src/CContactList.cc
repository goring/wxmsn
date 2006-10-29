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

#include "CContactList.h"
using namespace std;
using namespace nsContact;
using namespace nsEvents;
using namespace nsUtils;


// Defines

#define CLASS CContactList

// Implementation


CLASS::CLASS(wxWindow* Parent, CData* Data) : wxHtmlListBox(Parent)
{
	// Set some defaults options
	SetSelfHtmlPattern(wxT("<table><tr><td>%avatar(50,50)%<td nowrap><font size='4'>%name% (%status%)</font><br><font size='2'>%pm%</font></table><hr>"));
	SetContactHtmlPattern(wxT("<table><tr><td>%avatar(25,25)%<td nowrap><font size='3'>%name% (%status%)</font><br><font size='2'>%pm%</font></table>"));
	SetGroupHtmlPattern(wxT("<table><tr><td nowrap><font size='3'><b>%name% (%online%/%offline%)</b></font></table>"));
	SetContactSortingStyle(BY_GROUP);
	SetNormalContactColor(0, 0, 0);
	SetSelectedContactColor(38, 38, 255);
	SetOfflineContactColor(128, 128, 128);
	SetParseEmoticons(true);
	SetParseMSNPlusCodes(true);
	SetParseURLs(true);
	SetDisplayEmptyGroups(true);
	
	// Puts some pointer to NULL
	LinkedData = NULL;
	SelectedContact = NULL;
	HoveredContact = NULL;
	ContactInfoPopup = NULL;
	
	// Sets linked datas
	LinkedData = Data;
	if(LinkedData != NULL)
	{
		LinkedData->LinkedContactList = this;
	}
	
	// Creates the timer whick handles the info popup appearition
	HoverTimer.SetOwner(this, HOVER_TIMER);
	
	// Sets some other variables
	BrowserCommand = wxT("firefox");
	MustReparse = true;
}


CLASS::~CLASS()
{
	// Destroys popup if it's opened
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
	}
	
	// Unlinks the list
	if(LinkedData != NULL)
	{
		LinkedData->LinkedContactList = NULL;
	}
}


// Redraw the list
void CLASS::Redraw()
{	
	if(LinkedData != NULL && IsShown() == true)
	{
		// Empties the displayed items array
		DisplayedGroups.Empty();
		DisplayedContacts.Empty();
		
		// Render SelfContact (if one is setted)
		if(LinkedData->SelfContact != NULL)
		{
			DisplayedContacts.Add(LinkedData->SelfContact);
		}
	
		// Harcoded pictures pathes
		wxString FoldedArrowTag = wxT("<img src='") + wxPathOnly(wxTheApp->argv[0]) + wxT("/resources/folded.png'>");
		wxString UnfoldedArrowTag = wxT("<img src='") + wxPathOnly(wxTheApp->argv[0]) + wxT("/resources/unfolded.png'>");

		// OK, now draws group with respect to the contact sorting style
		// Lot of duplicated code here, but it's much easier to read and debug that way
		GroupArray DrawedGroup;
		
		switch(SortingStyle)
		{
			// Draws all the groups but not the Online and Offline StatusGroup
			case BY_GROUP:
			{
				for(unsigned int i=0; i<LinkedData->Group.GetCount(); i++)
				{
					if(LinkedData->Group[i]->GetContactCount() > 0 || DisplayEmptyGroups == true)
					{
						DrawedGroup.Add(LinkedData->Group[i]);
					}
				}
				DrawedGroup.Sort(CompareGroup);
				
				if(LinkedData->StatusGroup[2]->GetContactCount() > 0 || DisplayEmptyGroups == true)
				{
					DrawedGroup.Add(LinkedData->StatusGroup[2]);
				}
				
				for(unsigned int i=0; i<DrawedGroup.GetCount(); i++)
				{			
					DisplayedGroups.Add(DrawedGroup[i]);
					
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are on FL (Forward List)
							if(DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
							}
						}
					}
				}
				break;
			}
			
			// Draws only Online and Offline groups
			case BY_STATUS:
			{
				if(LinkedData->StatusGroup[0]->GetContactCount() > 0 || DisplayEmptyGroups == true)
				{
					DrawedGroup.Add(LinkedData->StatusGroup[0]);
				}
				if(LinkedData->StatusGroup[1]->GetContactCount() > 0 || DisplayEmptyGroups == true)
				{
					DrawedGroup.Add(LinkedData->StatusGroup[1]);
				}
				
				for(unsigned int i=0; i<DrawedGroup.GetCount(); i++)
				{
					DisplayedGroups.Add(DrawedGroup[i]);
						
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are on FL (Forward List)
							if(DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
							}
						}
					}
				}
				
				break;
			}
			
			// Draws all the groups except Online one
			case MIXED:
			{
				DrawedGroup.Alloc(LinkedData->Group.GetCount()+2);
				for(unsigned int i=0; i<LinkedData->Group.GetCount(); i++)
				{
					if(LinkedData->Group[i]->GetOnlineContactCount() > 0 || DisplayEmptyGroups == true)
					{
						DrawedGroup.Add(LinkedData->Group[i]);
					}
				}
				DrawedGroup.Sort(CompareGroup);
				if(LinkedData->StatusGroup[2]->GetOnlineContactCount() > 0 || DisplayEmptyGroups == true)
				{
					DrawedGroup.Add(LinkedData->StatusGroup[2]);
				}
				
				if(LinkedData->StatusGroup[1]->GetContactCount() || DisplayEmptyGroups == true)
				{
					DrawedGroup.Add(LinkedData->StatusGroup[1]);
				}
				
				for(unsigned int i=0; i<DrawedGroup.GetCount(); i++)
				{
					DisplayedGroups.Add(DrawedGroup[i]);
						
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are Online and on FL (Forward List)
							if((DrawedGroup[i]->Contact[j]->GetStatus() != OFFLINE || DrawedGroup[i] == LinkedData->StatusGroup[1]) && DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
							}
						}
					}
				}
				
				break;
			}
		}
				
		MustReparse = false;
		SetItemCount(DisplayedContacts.Count());
		RefreshAll();
	}
}


void CLASS::SetContactSortingStyle(const CONTACT_SORTING_STYLE NewSortingStyle)
{
	SortingStyle = NewSortingStyle;
}


void CLASS::SetGroupHtmlPattern(const wxString& Pattern)
{
	GroupHtmlPattern = Pattern;
	MustReparse = true;
}


void CLASS::SetSelfHtmlPattern(const wxString& Pattern)
{
	SelfHtmlPattern = Pattern;
	MustReparse = true;
}
	

void CLASS::SetContactHtmlPattern(const wxString& Pattern)
{
	ContactHtmlPattern = Pattern;
	MustReparse = true;
}


void CLASS::SetNormalContactColor(unsigned char R, unsigned char G, unsigned char B)
{
	NormalContactColor.Printf(wxT("#%.2X%.2X%.2X"), R, G, B);
}


void CLASS::SetSelectedContactColor(unsigned char R, unsigned char G, unsigned char B)
{
	SelectedContactColor.Printf(wxT("#%.2X%.2X%.2X"), R, G, B);
}


void CLASS::SetOfflineContactColor(unsigned char R, unsigned char G, unsigned char B)
{
	OfflineContactColor.Printf(wxT("#%.2X%.2X%.2X"), R, G, B);
}


void CLASS::SetDisplayEmptyGroups(bool Display)
{
	DisplayEmptyGroups = Display;
}


void CLASS::SetBackground(unsigned char R, unsigned char G, unsigned char B)
{
	Background.Printf(wxT(" bgcolor='#%.2X%.2X%.2X'"), R, G, B);
}


void CLASS::SetBackground(const wxString& Image)
{
	Background = wxT(" background='") + Image + wxT("'");
}


void CLASS::SetParseMSNPlusCodes(bool Parse)
{
	ParseMSNPlusCodes = Parse;
	MustReparse = true;
}


void CLASS::SetParseEmoticons(bool Parse)
{
	ParseEmoticons = Parse;
	MustReparse = true;
}


void CLASS::SetParseURLs(bool Parse)
{
	ParseURLs = Parse;
	MustReparse = true;
}


wxString CLASS::OnGetItem(size_t n) const
{
	// Set the correct color
	wxString FontColor;
	if(DisplayedContacts[n]->IsSelected() == true)
	{
		FontColor = SelectedContactColor;
	}
	else if(DisplayedContacts[n]->GetStatus() != ONLINE)
	{
		FontColor = OfflineContactColor;
	}
	else
	{
		FontColor = NormalContactColor;
	}
									
	bool MustParseMSNPlusCodes = (ParseMSNPlusCodes == true) && (DisplayedContacts[n]->IsSelected() == false) && (DisplayedContacts[n]->GetStatus() == ONLINE);
	wxString Result = wxString::Format(wxT("<font color='#FF0000'>Test</font>"));

	return Result;
}
