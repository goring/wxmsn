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

#include "CChatArea.h"


// Defines

#define CLASS CChatArea


// Implementation

CLASS::CLASS(wxWindow * parent) : CHtmlWindow(parent, -1, wxDefaultPosition, wxDefaultSize)
{
	int Size[] = {6,8,10,12,14,16,18};
	SetFonts(wxT(""), wxT(""), Size);
	SetBorders(10);
	SetParseEmoticons(true);
	SetParseMSNPlusCodes(true);
	SetParseURLs(true);
	m_mustrebuildhtml = false;
}


CLASS::~CLASS()
{

}


// Sets formatting pattern
void CLASS::SetPattern(const wxString & pattern)
{
	m_pattern = pattern;
}


void CLASS::AddText(const wxString & text)
{
	// TODO : use CContact to have complete informations
	if(m_source.GetCount() == 0)
	{
		m_htmlbuffer += nsUtils::BuildHtmlFromText(text, m_parseemoticons, m_parsemsnpluscodes, m_parseurls);
	}
	else
	{
		m_htmlbuffer += wxT("<br>") + nsUtils::BuildHtmlFromText(text, m_parseemoticons, m_parsemsnpluscodes, m_parseurls);
	}
	
	m_source.Add(text);
	Redraw();
}


void CLASS::Redraw()
{
	// TODO : process all tags //
	if(m_mustrebuildhtml == true)
	{
		m_htmlbuffer.Empty();
		for(unsigned int i=0; i<m_source.GetCount(); i++)
		{
			m_htmlbuffer += nsUtils::BuildHtmlFromText(m_source[i], m_parseemoticons, m_parsemsnpluscodes, m_parseurls);
			if(i != m_source.GetCount()-1)
			{
				m_htmlbuffer += wxT("<br>");
			}
		}
	}

	SetPage(wxT("<html><body") + m_background + wxT(">") + m_htmlbuffer + wxT("</body></html>"));
	
	int x, y;
	GetVirtualSize(&x, &y);
	Scroll(-1, y);
}


void CLASS::SetParseMSNPlusCodes(bool parse)
{
	m_parsemsnpluscodes = parse;
	m_mustrebuildhtml = true;
}


void CLASS::SetParseEmoticons(bool parse)
{
	m_parseemoticons = parse;
	m_mustrebuildhtml = true;
}


void CLASS::SetParseURLs(bool parse)
{
	m_parseurls = parse;
	m_mustrebuildhtml = true;
}


void CLASS::SetBackground(unsigned char r, unsigned char g, unsigned char b)
{
	m_background.Printf(wxT(" bgcolor='#%.2X%.2X%.2X'"), r, g, b);
}


void CLASS::SetBackground(const wxString & image)
{
	m_background = wxT(" background='") + image + wxT("'");
}


int CLASS::GetHistoryNumber()
{
	return m_source.GetCount();
}


const wxString& CLASS::GetHistory(int index)
{
	if(index > -1 && index < static_cast<int>(m_source.GetCount()))
	{
		return m_source[index];
	}
	else
	{
		return wxString(wxEmptyString);
	}
}
