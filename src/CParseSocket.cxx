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

/** 	\file 	CParseSocket.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	24/11/200
*/

// Includes

#include "CParseSocket.h"
#include <wx/tokenzr.h>

// Namespaces

using namespace std;
using namespace nsNetwork;

// Class 
#define CLASS CParseSocket

CLASS::CLASS(wxSocketClient * Socket)
    : m_sock(Socket), m_null(wxT(" ")), m_oldbuff(wxT(""))
{   }

void CLASS::Parse()
{
    //Extrait des lignes du Buffer d'entrée
    char Buffer[1025];
    wxString wxBuff;
    int i;
    
    //Empty the Line Vector
    m_vec.Empty();
    
    do //Loops while something is in the buffer and no error occurs
    {
        m_sock->Peek(Buffer, 1024);
        Buffer[m_sock->LastCount()] = '\0';
        wxBuff= wxString(wxConvUTF8.cMB2WC(Buffer));
        
        //Searchs a \n character in the string
        i = wxBuff.Find(wxT("\n"));
        
        //If no such character is found, put it in the buffer
        if(i == -1)
        {
            m_sock->Read(Buffer, 1024);
            Buffer[m_sock->LastCount()] = '\0';
            m_oldbuff += wxBuff;
        }
        else //Adds the buffer+the current string to the Vector
        {
            m_sock->Read(Buffer, i+1);
            Buffer[m_sock->LastCount()-1] = '\0';
            wxBuff= wxString(wxConvUTF8.cMB2WC(Buffer));
            
            //Removes the \r character
            wxBuff.Replace(wxT("\r"), wxT(""), true);
            
            m_vec.Add(m_oldbuff+wxBuff);
            m_oldbuff.Clear();
        }
        
    }while(m_sock->LastCount() > 0 && !m_sock->Error() );
}

size_t CLASS::Lines()
{
    return m_vec.GetCount();
}

size_t CLASS::LineSize(size_t line)
{
    if(line > m_vec.GetCount())
        return 0;
    
    wxStringTokenizer TokWord(m_vec[line], wxT(" "));
    return TokWord.CountTokens();
}
        
wxString CLASS::GetString(size_t line, size_t word)
{
    if(line > m_vec.GetCount())
        return m_null;
    
    wxStringTokenizer TokWord(m_vec[line], wxT(" "));
    if(word > TokWord.CountTokens())
        return m_null;
    
    for(size_t i = 0 ; i < word ; ++i)
        TokWord.GetNextToken();
    
    return TokWord.GetNextToken();
}

wxString & CLASS::operator [] (size_t line)
{
    if(line > m_vec.GetCount())
        return m_null;
    return m_vec[line];
}

#undef CLASS
