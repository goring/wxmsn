/*  Copyright (c) 2005 Neil "Superna" ARMSTRONG
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

/** 	\file 	SCPrefs.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	19/03/2006
*/

// Includes

#include "SCPrefs.h"
#include <wx/textfile.h>

// Namespaces

using namespace std;
using namespace nsCore;

// Class 
#define CLASS SCPrefs

wxString nsCore::CLASS::m_artist, nsCore::CLASS::m_title;
wxStandardPaths nsCore::CLASS::m_paths;
wxFileName nsCore::CLASS::m_songfile;
wxDateTime nsCore::CLASS::m_songmodif;

void CLASS::Init()
{
    m_artist = wxString();
    m_title = wxString();
    if(!wxFileName::Mkdir(m_paths.GetUserDataDir(), 0700))
        wxLogMessage(_("Unable to create User Config directory !"));
    m_songfile.Assign(m_paths.GetUserDataDir(), SONGFILE);
    if(!m_songfile.FileExists())
        wxFile().Create(m_songfile.GetFullPath());
    m_songfile.Touch();
    m_songfile.GetTimes(NULL, &m_songmodif, NULL);
}

//static wxString m_artist, m_title;
SongStatus CLASS::CheckPlayedSong()
{
    if(m_songfile.FileExists())
    {
        wxDateTime modiftime;
        m_songfile.GetTimes(NULL, &modiftime, NULL);
        if(m_songmodif.IsEarlierThan(modiftime))
        {
            m_songmodif = modiftime;
            m_title.Empty();
            m_artist.Empty();
            //File modified
            wxTextFile file(m_songfile.GetFullPath());
            if(file.Open() && file.GetLineCount() >= 1)
            {
                m_title = file.GetFirstLine();
                if(file.GetLineCount() > 1)
                    m_artist = file.GetNextLine();
                else
                    m_artist = wxString();
            }

            if(m_title.IsEmpty() && m_artist.IsEmpty())
                return PrefsNoSong;
            
            return PrefsNewSong;
        }
        else
            return PrefsNoNewSong;
    }
    else
        return PrefsSongError;
}

wxString & CLASS::GetPlayedTitle()
{
    return m_title;
}

wxString & CLASS::GetPlayedArtist()
{
    return m_artist;
}

#undef CLASS
