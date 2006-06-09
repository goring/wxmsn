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

//Includes

#include "CEntryText.h"


// Defines

#define CLASS CEntryText


// Events

BEGIN_EVENT_TABLE(CLASS, wxTextCtrl)
	EVT_KEY_DOWN(CLASS::OnKeyDown)
	EVT_SET_FOCUS(CLASS::OnFocus)
END_EVENT_TABLE()


// Implementation

CLASS::CLASS(wxWindow * parent, int id) : wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition,
			wxDefaultSize, wxTE_RICH2 | wxTE_PROCESS_TAB | wxTE_PROCESS_ENTER | wxTE_MULTILINE)
{
	m_historyindex = -1;
}


CLASS::~CLASS()
{
	
}


void CLASS::OnKeyDown(wxKeyEvent & event)
{
	if(event.HasModifiers() == false)
	{
		switch(event.GetKeyCode())
		{
			case WXK_RETURN:
			case WXK_NUMPAD_ENTER:
				if(event.ShiftDown() == true)
				{
					AppendText(wxT("\n"));
				}
				else if(GetValue().IsEmpty() == false)
				{
					m_entryhistory.Add(GetValue());
					m_historyindex = -1;
					event.Skip();
				}
				break;
			
			default:
				m_historyindex = -1;
				event.Skip();
		}
	}
	else
	{	
		switch(event.GetKeyCode())
		{
			case WXK_UP:
				if(event.ControlDown() == true)
				{
					if(m_historyindex == -1 && m_entryhistory.GetCount() != 0)
					{
						m_historyindex = m_entryhistory.GetCount()-1;
						if(GetValue().IsEmpty() == false)
						{
							m_entryhistory.Add(GetValue());
						}
					}
					else if(m_historyindex > 0)
					{
						m_historyindex--;
					}
					
					if(m_historyindex > -1 && m_historyindex < wx_static_cast(int, m_entryhistory.GetCount()))
					{
						SetValue(m_entryhistory[m_historyindex]);
						SetInsertionPointEnd();
					}
				}
				else
				{
					event.Skip();
				}
				break;
		
			case WXK_DOWN:
				if(event.ControlDown() == true)
				{
					if(m_historyindex < wx_static_cast(int, m_entryhistory.GetCount()) && m_historyindex != -1)
					{
						m_historyindex++;
					}
						
					if(m_historyindex > -1 && m_historyindex < wx_static_cast(int, m_entryhistory.GetCount()))
					{
						SetValue(m_entryhistory[m_historyindex]);
						SetInsertionPointEnd();
					}
					else if(m_historyindex != -1)
					{
						SetValue(wxEmptyString);
					}
				}
				else
				{
					event.Skip();
				}
				break;
				
			default:
				event.Skip();
		}
	}
}


void CLASS::OnFocus(wxFocusEvent & event)
{
//	event.Skip();
}
