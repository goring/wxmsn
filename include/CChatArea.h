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

#ifndef _CCHATAREA_H_
#define _CCHATAREA_H_


// Includes

#include "wx/wx.h"
#include "CHtmlWin.h"
#include "wx/arrstr.h"
#include "Utils.h"


// Defines


// Classes

class CChatArea : public CHtmlWindow
{
	public:
		CChatArea(wxWindow * parent);
		~CChatArea();
		void SetPattern(const wxString & pattern);
		void AddText(const wxString & text);
		void Redraw();
		void SetParseMSNPlusCodes(bool parse);
		void SetParseEmoticons(bool parse);
		void SetParseURLs(bool parse);
	 	void SetBackground(unsigned char r, unsigned char g, unsigned char b);
		void SetBackground(const wxString & image);
		int GetHistoryNumber();
		const wxString& GetHistory(int index);
	
	private:
		bool			m_parseemoticons;
		bool			m_parsemsnpluscodes;
		bool 			m_parseurls;
		bool			m_mustrebuildhtml;
		wxString		m_background;
		wxString		m_pattern;
		wxString		m_htmlbuffer;
		wxArrayString 	m_source;
};

#endif
