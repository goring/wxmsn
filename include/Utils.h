/*  Copyright (c) 2005 Julien "Trapamoosch" ENCHE
                                    Neil "Superna" ARMSTRONG
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

#ifndef _CUTILS_H_
#define _CUTILS_H_


// Includes

#include "wx/wx.h"
#include "wx/image.h"
#include "wx/string.h"
#include "wx/tokenzr.h"
#include "wx/regex.h"
#include "wx/hashmap.h"
#include "defines.h"


// Classes

namespace nsUtils
{	
	wxImage BicubicScale(wxImage& Scaled, int Width, int Height);
    wxString BuildHtmlFromText(const wxString& Source, bool ParseEmoticons, bool ParseMSNPlusCodes, bool ParseURL, bool CleanHtml = true);

    wxString UrlEncode(const wxString & Str);
    wxString UrlDecode(const wxString & Str);
    
    void WizzFrame(wxFrame * frame, int nb = 40, int decal = 3);
    
    ONLINE_STATUS GetNumStatus(wxString & Str);
    wxString GetTxtStatus(ONLINE_STATUS status);

    class CLogWindow : public wxLogWindow
    {
        public:
            CLogWindow(wxWindow * parent, wxString str);

        protected:
            bool OnFrameClose(wxFrame *frame);
    };

    class CPrefs
    {
	public:
		static void Init();
		static wxString GetPath();
		static wxString LocateRSC(const wxString & str);
	
	private:
		static wxString m_path;
    };
}

WX_DECLARE_STRING_HASH_MAP(wxString, StringHash);
extern 	StringHash	IndexedColorHash;
extern 	StringHash	SmileyHash;
extern 	StringHash	HtmlHash;

#endif
