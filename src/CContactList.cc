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


// Events

BEGIN_EVENT_TABLE(CLASS, CHtmlWindow)
	EVT_LEFT_DOWN(CLASS::OnLeftClick)
	EVT_RIGHT_UP(CLASS::OnRightClick)
    EVT_RIGHT_DCLICK(CLASS::OnDoubleClick) 
	EVT_MOTION(CLASS::OnMouseMove)
	EVT_TIMER(HOVER_TIMER, CLASS::OnContactHover)
	EVT_SIZE(CLASS::OnSize)
	EVT_SET_FOCUS(CLASS::OnFocusChange)
	EVT_KILL_FOCUS(CLASS::OnFocusChange)
	EVT_MENU(-1, CLASS::OnMenu)
END_EVENT_TABLE()


// Implementation


CLASS::CLASS(wxWindow* Parent, CData* Data) : CHtmlWindow(Parent, -1, wxDefaultPosition, wxDefaultSize, wxHW_NO_SELECTION)
{
	int Size[] = {6,8,10,12,14,16,18};
	SetFonts(wxT(""), wxT(""), Size);
	SetBorders(20);
	
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
	ArrowCursor = wxCursor(wxCURSOR_ARROW);
	HandCursor = wxCursor(wxCURSOR_HAND);
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
		wxString HtmlBuffer;
		
		// ---------- Starts html building
						
		// Sets the background
		HtmlBuffer = wxT("<html><body") + Background + wxT(">");
		
		// Render SelfContact (if one is setted)
		if(LinkedData->SelfContact != NULL)
		{
			HtmlBuffer	+= wxT("<a name='StartSelf'><font color='")
						+ NormalContactColor + wxT("'>")
						+ LinkedData->SelfContact->GetHtmlCode(SelfHtmlPattern, ParseEmoticons, ParseMSNPlusCodes, ParseURLs, MustReparse)
						+ wxT("<a name='EndSelf'>");
		}
	
		// Harcoded pictures pathes
		wxString FoldedArrowTag = wxT("<img src='") + wxPathOnly(wxTheApp->argv[0]) + wxT("/resources/folded.png'>");
		wxString UnfoldedArrowTag = wxT("<img src='") + wxPathOnly(wxTheApp->argv[0]) + wxT("/resources/unfolded.png'>");

		// OK, now draws group with respect to the contact sorting style
		// Lot of duplicated code here, but it's much easier to read and debug that way
		GroupArray DrawedGroup;
		
		// Empties the displayed items array
		DisplayedGroups.Empty();
		DisplayedContacts.Empty();
		int DisplayedGroupsIndex = 0;
		int DisplayedContactsIndex = 0;
		
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
					HtmlBuffer	+= wxString::Format(wxT("<a name='StartGroup-%d'>"), DisplayedGroupsIndex) 
								+ wxT("<font color='") + NormalContactColor + wxT("'>")
								+ wxT("<table><tr><td valign=center width='15'>")
								+ (DrawedGroup[i]->GetContactCount()>0 ? (DrawedGroup[i]->GetFolded() ? FoldedArrowTag : UnfoldedArrowTag) : wxT(""))
								+ wxT("<td>") 
								+ DrawedGroup[i]->GetHtmlCode(GroupHtmlPattern, ParseEmoticons, ParseMSNPlusCodes, ParseURLs, MustReparse)
								+ wxString::Format(wxT("</table></font><a name='EndGroup-%d'><br>"), DisplayedGroupsIndex);			
						
					DisplayedGroups.Add(DrawedGroup[i]);
					DisplayedGroupsIndex++;
						
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are on FL (Forward List)
							if(DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								HtmlBuffer 	+= wxString::Format(wxT("<a name='StartContact-%d'>"), DisplayedContactsIndex)
											+ 
											+ DrawContact(DrawedGroup[i]->Contact[j])
											+ wxString::Format(wxT("<a name='EndContact-%d'>"), DisplayedContactsIndex);
	
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
								DisplayedContactsIndex++;
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
					HtmlBuffer	+= wxString::Format(wxT("<a name='StartGroup-%d'>"), DisplayedGroupsIndex) 
								+ wxT("<font color='") + NormalContactColor + wxT("'>")
								+ wxT("<table><tr><td valign=center width='15'>")
								+ (DrawedGroup[i]->GetContactCount()>0 ? (DrawedGroup[i]->GetFolded() ? FoldedArrowTag : UnfoldedArrowTag) : wxT(""))
								+ wxT("<td>") 
								+ DrawedGroup[i]->GetHtmlCode(GroupHtmlPattern, ParseEmoticons, ParseMSNPlusCodes, ParseURLs, MustReparse)
								+ wxString::Format(wxT("</table></font><a name='EndGroup-%d'><br>"), DisplayedGroupsIndex);			
						
					DisplayedGroups.Add(DrawedGroup[i]);
					DisplayedGroupsIndex++;
						
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are on FL (Forward List)
							if(DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								HtmlBuffer 	+= wxString::Format(wxT("<a name='StartContact-%d'>"), DisplayedContactsIndex)
											+ DrawContact(DrawedGroup[i]->Contact[j])
											+ wxString::Format(wxT("<a name='EndContact-%d'>"), DisplayedContactsIndex);
	
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
								DisplayedContactsIndex++;
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
					HtmlBuffer	+= wxString::Format(wxT("<a name='StartGroup-%d'>"), DisplayedGroupsIndex) 
								+ wxT("<font color='") + NormalContactColor + wxT("'>")
								+ wxT("<table><tr><td valign=center width='15'>")
								+ (((DrawedGroup[i]==LinkedData->StatusGroup[1]&&DrawedGroup[i]->GetContactCount()>0)||(DrawedGroup[i]!=LinkedData->StatusGroup[1]&&DrawedGroup[i]->GetOnlineContactCount()>0)) ? (DrawedGroup[i]->GetFolded() ? FoldedArrowTag : UnfoldedArrowTag) : wxT(""))
								+ wxT("<td>") 
								+ DrawedGroup[i]->GetHtmlCode(GroupHtmlPattern, ParseEmoticons, ParseMSNPlusCodes, ParseURLs, MustReparse)
								+ wxString::Format(wxT("</table></font><a name='EndGroup-%d'><br>"), DisplayedGroupsIndex);			
						
					DisplayedGroups.Add(DrawedGroup[i]);
					DisplayedGroupsIndex++;
						
					// Draws contact if necessary (group is unfolded)
					if(DrawedGroup[i]->GetFolded() == false) 
					{
						DrawedGroup[i]->Contact.Sort(CompareContact);
						for(unsigned int j=0; j<DrawedGroup[i]->Contact.GetCount(); j++)
						{		
							// Draws only contacts which are Online and on FL (Forward List)
							if((DrawedGroup[i]->Contact[j]->GetStatus() != OFFLINE || DrawedGroup[i] == LinkedData->StatusGroup[1]) && DrawedGroup[i]->Contact[j]->IsOnFL() == true)
							{
								HtmlBuffer 	+= wxString::Format(wxT("<a name='StartContact-%d'>"), DisplayedContactsIndex)
											+ DrawContact(DrawedGroup[i]->Contact[j])
											+ wxString::Format(wxT("</font><a name='EndContact-%d'>"), DisplayedContactsIndex);
	
								DisplayedContacts.Add(DrawedGroup[i]->Contact[j]);
								DisplayedContactsIndex++;
							}
						}
					}
				}
				
				break;
			}
		}
				
		HtmlBuffer += wxT("</body></html>");
		SetPage(HtmlBuffer);
		MustReparse = false;
		
		// Assigns correct IDs to all the html cells
		// For int data : 1 = SelfContact ; 2 = Group ; 3 = Contact
		
		wxString StartAnchor, EndAnchor;
		const CHtmlCell* StartCell, *EndCell;
	
		if(LinkedData->SelfContact != NULL)
		{
			StartAnchor = wxT("StartSelf");
			EndAnchor = wxT("EndSelf");
			StartCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &StartAnchor);
			EndCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &EndAnchor);
			AssignIntDataToCells(1, StartCell, EndCell);
			AssignPtrDataToCells((void*)LinkedData->SelfContact, StartCell, EndCell);
		}
		
		for(int i=0; i<DisplayedGroupsIndex; i++)
		{
			StartAnchor = wxString::Format(wxT("StartGroup-%d"), i);
			EndAnchor = wxString::Format(wxT("EndGroup-%d"), i);
			StartCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &StartAnchor);
			EndCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &EndAnchor);
			AssignIntDataToCells(2, StartCell, EndCell);
			AssignPtrDataToCells((void*)DisplayedGroups[i], StartCell, EndCell);
		}
		
		for(int i=0; i<DisplayedContactsIndex; i++)
		{
			StartAnchor = wxString::Format(wxT("StartContact-%d"), i);
			EndAnchor = wxString::Format(wxT("EndContact-%d"), i);
			StartCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &StartAnchor);
			EndCell = GetInternalRepresentation()->Find(wxHTML_COND_ISANCHOR, &EndAnchor);
			AssignIntDataToCells(3, StartCell, EndCell);
			AssignPtrDataToCells((void*)DisplayedContacts[i], StartCell, EndCell);
		}
	}
}


wxString CContactList::DrawContact(CContact* DrawedContact)
{
	// Set the correct color
	wxString FontColor;
	if(DrawedContact->IsSelected() == true)
	{
		FontColor = SelectedContactColor;
	}
	else if(DrawedContact->GetStatus() != ONLINE)
	{
		FontColor = OfflineContactColor;
	}
	else
	{
		FontColor = NormalContactColor;
	}
									
	bool MustParseMSNPlusCodes = (ParseMSNPlusCodes == true) && (DrawedContact->IsSelected() == false) && (DrawedContact->GetStatus() == ONLINE);
	wxString HtmlBuffer	= wxT("<font color='") + FontColor + wxT("'>")
						+ DrawedContact->GetHtmlCode(ContactHtmlPattern, ParseEmoticons, MustParseMSNPlusCodes, ParseURLs, MustReparse)
						+ wxT("</font>");
	return HtmlBuffer;
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


void CLASS::AssignIntDataToCells(const int data, const CHtmlCell* StartCell, const CHtmlCell* EndCell)
{
	CHtmlCell* CurrentCell = wxConstCast(StartCell, CHtmlCell);
	while(CurrentCell != NULL && CurrentCell != EndCell) 
	{
		AssignIntDataToCells(data, CurrentCell->GetFirstChild(), EndCell);
		CurrentCell->SetDataInt(data);
		CurrentCell = CurrentCell->GetNext();
	}
}


void CLASS::AssignPtrDataToCells(const void* data, const CHtmlCell* StartCell, const CHtmlCell* EndCell)
{
	CHtmlCell* CurrentCell = wxConstCast(StartCell, CHtmlCell);
	while(CurrentCell != NULL && CurrentCell != EndCell) 
	{
		AssignPtrDataToCells(data, CurrentCell->GetFirstChild(), EndCell);
		CurrentCell->SetDataPtr(wxConstCast(data, void));
		CurrentCell = CurrentCell->GetNext();
	}
}


void CLASS::OnLeftClick(wxMouseEvent& MouseEvent)
{	
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}
		
	if(SelectedContact != NULL)
	{
		SelectedContact->SetSelected(false);
		SelectedContact = NULL;
	}

//	HoverTimer.Stop();
	
	CHtmlCell* ParentCell = wx_static_cast(CHtmlCell*, GetInternalRepresentation());
	int x, y;
	CalcUnscrolledPosition(MouseEvent.GetX(), MouseEvent.GetY(), &x, &y);
	CHtmlCell* ClickedCell = ParentCell->FindCellByPos(x, y);

	if(ClickedCell != NULL)
	{
		// 1 = self ; 2 = group ; 3 = contact
		switch(ClickedCell->GetDataInt())
		{
			case 2:
			{
				CGroup* ClickedGroup = (CGroup*)ClickedCell->GetDataPtr();
				ClickedGroup->SetFolded(!ClickedGroup->GetFolded());
				break;
			}
			
			case 3:
			{	
				CContact* ClickedContact = (CContact*)ClickedCell->GetDataPtr();
				ClickedContact->SetSelected(true);
				SelectedContact = ClickedContact;
				HoverTimer.Start(1000, true);
				break;
			}
		}
	}
	
	Redraw();
}


void CLASS::OnRightClick(wxMouseEvent& MouseEvent)
{
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}
		
	if(SelectedContact != NULL)
	{
		SelectedContact->SetSelected(false);
		SelectedContact = NULL;
	}

	HoverTimer.Stop();
	
	CHtmlCell* ParentCell = wx_static_cast(CHtmlCell*, GetInternalRepresentation());
	int x, y;
	CalcUnscrolledPosition(MouseEvent.GetX(), MouseEvent.GetY(), &x, &y);
	CHtmlCell* ClickedCell = ParentCell->FindCellByPos(x, y);

	if(ClickedCell != NULL)
	{
		// 1 = self ; 2 = group ; 3 = contact
		switch(ClickedCell->GetDataInt())
		{
			case 1:
			{
				Redraw();
				wxMenu Menu;
				Menu.Append(-1, LinkedData->GetMail());
				Menu.AppendSeparator();
				Menu.Append(OnlineStatusMenu, _("On line"));
				Menu.Append(BusyStatusMenu, _("Busy"));
				Menu.Append(IdleStatusMenu, _("Idle"));
				Menu.Append(BrbStatusMenu, _("Be right back"));
				Menu.Append(AwayStatusMenu, _("Away"));
				Menu.Append(PhoneStatusMenu, _("On phone"));
				Menu.Append(LunchStatusMenu, _("Out to lunch"));
				Menu.Append(AppearOfflineStatusMenu, _("Appear offline"));
				Menu.Append(CustomStatusMenu, _("Personal away message"));
				Menu.AppendSeparator();
				Menu.Append(ChangeFNameMenu, _("Change friendly name"));
				Menu.Append(ChangePMessageMenu, _("Change personal message"));
				PopupMenu(&Menu, MouseEvent.GetX(), MouseEvent.GetY());
				break;
			}
			
			case 2:
			{
				CGroup* ClickedGroup = (CGroup*)ClickedCell->GetDataPtr();
				Redraw();
				wxMenu Menu;
				Menu.Append(-1, ClickedGroup->GetName());
				Menu.AppendSeparator();
				Menu.Append(GroupBlockMenu, _("Block All Group"));
				Menu.Append(GroupChatMenu, _("Chat with all Group"));
				Menu.Append(GroupRemoveMenu, _("Remove Group"));
				Menu.Append(GroupRenameMenu, _("Rename Group"));
				PopupMenu(&Menu, MouseEvent.GetX(), MouseEvent.GetY());
				break;
			}
			
			case 3:
			{
				CContact* ClickedContact = (CContact*)ClickedCell->GetDataPtr();
				ClickedContact->SetSelected(true);
				SelectedContact = ClickedContact;
				Redraw();
				wxMenu Menu;
				Menu.Append(-1, SelectedContact->GetAddress());
				Menu.AppendSeparator();
				Menu.Append(ContactChatMenu, _("Open Chat Window"));
				Menu.Append(ContactInfoMenu, _("View Contact Infos"));
				Menu.Append(ContactGroupRemoveMenu, _("Remove from Group"));
				Menu.Append(ContactGroupCopyMenu, _("Copy to another Group"));
				Menu.Append(ContactGroupMoveMenu, _("Move to another Group"));
				Menu.Append(ContactRemoveMenu, _("Remove Contact"));
				PopupMenu(&Menu, MouseEvent.GetX(), MouseEvent.GetY());
				break;
			}
		}
	} 
	else
	{
		Redraw();
	}
}

void CLASS::OnDoubleClick(wxMouseEvent& MouseEvent)
{
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}
		
	if(SelectedContact != NULL)
	{
		SelectedContact->SetSelected(false);
		SelectedContact = NULL;
	}

	HoverTimer.Stop();
	
	CHtmlCell* ParentCell = wx_static_cast(CHtmlCell*, GetInternalRepresentation());
	int x, y;
	CalcUnscrolledPosition(MouseEvent.GetX(), MouseEvent.GetY(), &x, &y);
	CHtmlCell* ClickedCell = ParentCell->FindCellByPos(x, y);

	// 1 = self ; 2 = group ; 3 = contact
	if(ClickedCell != NULL && ClickedCell->GetDataInt() == 3)
	{
		CContact* ClickedContact = (CContact*)ClickedCell->GetDataPtr();
        ClickedContact->SetSelected(true);
		SelectedContact = ClickedContact;
		Redraw();
        //DBL Click on a contact
        CListEvent Event(wxListEventDblClick);
        Event.SetEventObject(this);
		Event.SetString(SelectedContact->GetAddress());
		GetParent()->ProcessEvent(Event);
    }
}

void CLASS::OnMouseMove(wxMouseEvent& MouseEvent)
{
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}

	if(SelectedContact != NULL && MouseEvent.Leaving() == false && ContactInfoPopup == NULL)
	{
		HoverTimer.Start(1000, true);
	}
	else
	{
		HoverTimer.Stop();
	}

	int x, y;
	CalcUnscrolledPosition(MouseEvent.GetX(), MouseEvent.GetY(), &x, &y);
	CHtmlCell* ParentCell = wx_static_cast(CHtmlCell*, GetInternalRepresentation());
	if(ParentCell->GetLink(x, y) != NULL)
	{
		if(GetCursor() != HandCursor)
		{
			SetCursor(HandCursor);
		}
	}
	else
	{
		if(GetCursor() != ArrowCursor)
		{
			SetCursor(ArrowCursor);
		}
	}
}


void CLASS::OnLinkClicked(const CHtmlLinkInfo& ClickedLink)
{
	wxExecute(BrowserCommand + wxT(" ") + ClickedLink.GetHref());
}


void CLASS::OnContactHover(wxTimerEvent& TimerEvent)
{
	wxPoint ScreenMousePosition = wxGetMousePosition();
	wxPoint ClientMousePosition = ScreenToClient(ScreenMousePosition);

	CHtmlCell* ParentCell = wx_static_cast(CHtmlCell*, GetInternalRepresentation());
	int x, y;
	CalcUnscrolledPosition(ClientMousePosition.x, ClientMousePosition.y, &x, &y);
	CHtmlCell* HoveredCell = ParentCell->FindCellByPos(x, y);
	
	// For int data : 1 = SelfContact ; 2 = Group ; 3 = Contact
	if(HoveredCell != NULL && HoveredCell->GetDataInt() == 3)
	{
		CContact* HoveredContact = (CContact*)HoveredCell->GetDataPtr();
		if(HoveredContact == SelectedContact)
		{
			ScreenMousePosition.x++;
			ScreenMousePosition.y++;
			HoveredContact = HoveredContact;
			
			ContactInfoPopup = 	new CPopup(NULL, _("Informations"), ScreenMousePosition, wxSize(100, 100), HoveredContact->GetAvatar(), wxSize(96,96), 
								_("Name : ") + HoveredContact->GetName()
								+ _("\nMessage : ") + HoveredContact->GetPersonalMessage()
								+ _("\nAddress : ") + HoveredContact->GetAddress()
								+ ((HoveredContact->IsOnRL() == true) ? _("\nYou're not in this contact's list") : _("\nYou're in this contact's list")), 
								true, 0);
		}
		else
		{
			if(ContactInfoPopup != NULL)
			{
				ContactInfoPopup->Destroy();
				ContactInfoPopup = NULL;
			}
			HoveredContact = NULL;
		}	
	}
}


void CLASS::OnSize(wxSizeEvent& SizeEvent)
{	
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}
	
	SizeEvent.Skip();
}


void CLASS::OnFocusChange(wxFocusEvent& FocusEvent)
{
	if(ContactInfoPopup != NULL)
	{
		ContactInfoPopup->Destroy();
		ContactInfoPopup = NULL;
	}
}


void CLASS::OnMenu(wxCommandEvent& MenuEvent)
{ 
	switch(MenuEvent.GetId())
	{
		case ChangeFNameMenu:
		{
			wxTextEntryDialog EntryDialog(this, _("New friendly name : "), _("Change friendly name"), LinkedData->SelfContact->GetName());
			if(EntryDialog.ShowModal() == wxID_OK && EntryDialog.GetValue().IsEmpty() == false)
			{
				CListEvent Event(wxListEventNewFName);
				Event.SetEventObject(this);
				Event.SetString(UrlEncode(EntryDialog.GetValue()));
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
			
		case ChangePMessageMenu:
		{
			wxTextEntryDialog EntryDialog(this, _("New personal message : "), _("Change personal message"), LinkedData->SelfContact->GetPersonalMessage());
			if(EntryDialog.ShowModal() == wxID_OK)
			{
				CListEvent Event(wxListEventNewPStatus);
				Event.SetEventObject(this);
				Event.SetString(EntryDialog.GetValue());
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case OnlineStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != ONLINE)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(ONLINE);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case BusyStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != BUSY)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(BUSY);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case IdleStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != IDLE)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(IDLE);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case BrbStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != BE_RIGHT_BACK)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(BE_RIGHT_BACK);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case AwayStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != AWAY)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(AWAY);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case PhoneStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != PHONE)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(PHONE);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case LunchStatusMenu:
	    {
			if(LinkedData->SelfContact->GetStatus() != OUT_TO_LUNCH)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(OUT_TO_LUNCH);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case AppearOfflineStatusMenu:
		{
			if(LinkedData->SelfContact->GetStatus() != APPEAR_OFFLINE)
			{
				CListEvent Event(wxListEventNewStatus);
				Event.SetStatus(APPEAR_OFFLINE);
				Event.SetEventObject(this);
				GetParent()->ProcessEvent(Event);
			}
		}
		break;
		
		case CustomStatusMenu:
		{
			wxTextEntryDialog EntryDialog(this, _("Personal away message"), _("Change personal away message"), LinkedData->SelfContact->GetCustomStatus());
			if(EntryDialog.ShowModal() == wxID_OK && EntryDialog.GetValue().IsEmpty() == false)
			{
				wxString CurrentName = LinkedData->SelfContact->GetName();
				LinkedData->SelfContact->SetCustomStatusState(false);
				LinkedData->SelfContact->SetCustomStatusState(true);
				LinkedData->SelfContact->SetCustomStatus(EntryDialog.GetValue());
				wxString NewName = LinkedData->SelfContact->GetName() + wxT(" {") + LinkedData->SelfContact->GetCustomStatus() + wxT("}");
				
				if(NewName != CurrentName)
				{
					CListEvent NameEvent(wxListEventNewFName);
					NameEvent.SetEventObject(this);
					NameEvent.SetString(UrlEncode(NewName));
					GetParent()->ProcessEvent(NameEvent);
				}
				
				if(LinkedData->SelfContact->GetStatus() != AWAY)
				{
					CListEvent StatusEvent(wxListEventNewStatus);
					StatusEvent.SetStatus(AWAY);
					StatusEvent.SetEventObject(this);
					GetParent()->ProcessEvent(StatusEvent);
				}
			}
		}
		break;
	}
	
	if(MenuEvent.GetId() >= OnlineStatusMenu && MenuEvent.GetId() <= AppearOfflineStatusMenu)
	{
		if(LinkedData->SelfContact->HasCustomStatus() == true)
		{
			LinkedData->SelfContact->SetCustomStatusState(false);
			CListEvent NameEvent(wxListEventNewFName);
			NameEvent.SetEventObject(this);
			NameEvent.SetString(UrlEncode(LinkedData->SelfContact->GetName()));
			GetParent()->ProcessEvent(NameEvent);
		}
	}
}


// Others stuff

// Corrects a wxHtmlWindow's bug only if Wx is not staticly built
#ifdef _WXQSTATIC_
void CHtmlWindow::CreateLayout()
{	
    if(m_Cell != NULL)
	{
		int ScrollX, ScrollY;
		CalcUnscrolledPosition(0, 0, &ScrollX, &ScrollY);
		
		int ClientWidth, ClientHeight;
		GetClientSize(&ClientWidth, &ClientHeight);
		m_Cell->Layout(ClientWidth);
		
		if(m_Style & wxHW_SCROLLBAR_NEVER)
		{
			SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0, ScrollX / wxHTML_SCROLL_STEP, 0);
		}
		else
		{
			if(ClientHeight < m_Cell->GetHeight() + GetCharHeight())
			{
				SetScrollbars(wxHTML_SCROLL_STEP, wxHTML_SCROLL_STEP, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, (m_Cell->GetHeight() + GetCharHeight()) / wxHTML_SCROLL_STEP, ScrollX / wxHTML_SCROLL_STEP, ScrollY / wxHTML_SCROLL_STEP);
			}
			else
			{
				SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0, ScrollX / wxHTML_SCROLL_STEP, 0);
			}
		}
	}
}
#endif
