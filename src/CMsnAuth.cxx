/*  Copyright (c) 2005 
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

/** 	\file 	CMsnAuth.cxx
	\author Neil "Superna" ARMSTRONG
	\date 	24/11/2005
*/

//#define _DEBUG_

// Includes

#include "CMsnAuth.h"
#include "wxmsn.h"
#include <wx/tokenzr.h>
#include "Utils.h"

// Namespaces

using namespace std;
using namespace nsNetwork;
using namespace nsUtils;

// Class 
#define CLASS CMsnAuth

CLASS::CLASS()
    : m_ssl(NEXUS)
{ }

bool CLASS::Auth(wxString mail, wxString password, wxString con_str)
{
    m_mail = mail;
    m_pass = password;
    m_con_str = con_str;
    m_authid = wxT("");
    
    if( ! FindServer() )
        return false;
    
    if( ! DoAuth() )
        return false;
    
    return true;
}
        
wxString & CLASS::GetAuthID()
{
    return m_authid;
}
        
wxString & CLASS::GetError()
{
    return m_errstr;
}
        
bool CLASS::FindServer()
{
    wxString Buffer(NEXUS_REQUEST);
        
    m_ssl = CSsl(NEXUS);
    
    if(m_ssl.Connect())
    {
	m_errstr = wxString(_("Unable to connect to SSL Server ")) + NEXUS;
	return false;
    }

    if(!m_ssl.Write(Buffer))
    {
        m_errstr = wxString(_("Unable to Write to SSL Server ")) + NEXUS;
        return false;
    }
    
    m_ssl.Read(Buffer);
    
    #ifdef _DEBUG_
        //cout << "SSL Received : {" << Buffer.mb_str(wxConvUTF8) << "}" << endl;
        wxLogMessage(_("SSL Received : {%s}"),  Buffer.c_str());
    #endif 
    
    //Parsing answer
    wxStringTokenizer NexusTok(Buffer, wxT("\r\n"));
    
    Buffer = NexusTok.GetNextToken(); 
    
    while( !Buffer.Mid(0,12).IsSameAs(wxT("PassportURLs")) && NexusTok.HasMoreTokens() )
        Buffer = NexusTok.GetNextToken();
    
    if(!NexusTok.HasMoreTokens())
    {
        m_errstr = _("Error, did not found Nexus PassportURLs String");
        return false;
    }
    
    NexusTok.SetString(Buffer, wxT(" ,"));
    Buffer = NexusTok.GetNextToken();
    
    while( !Buffer.Mid(0,7).IsSameAs(wxT("DALogin")) && NexusTok.HasMoreTokens() )
        Buffer = NexusTok.GetNextToken();
    
    if(!NexusTok.HasMoreTokens())
    {
        m_errstr = _("Error, did not found DALogin String");
        return false;
    }
    m_serverhost = Buffer.Mid(8);
    m_errstr = _("Found Passport Host : ") + m_serverhost;
    
    m_ssl.Close();
    return true;
}

bool CLASS::DoAuth()
{
    wxString Host, File, Buffer, SSLHost, Mail, Pass, Status;
    
    //Cutting host/file
    wxInt32 i = m_serverhost.Find(_T("/"));
    Host = m_serverhost.Mid(0, i);
    File = m_serverhost.Mid(i);
    
    SSLHost = Host + wxT(":https");
    
    #ifdef _DEBUG_
        //cout << "Request : " << SSLHost.mb_str(wxConvUTF8) << File.mb_str(wxConvUTF8) << endl;
         wxLogMessage(_("Request : %s %s"), SSLHost.c_str(), File.c_str());
    #endif
    
    Mail = UrlEncode(m_mail);
    Pass = UrlEncode(m_pass);
    //Mail.Replace(_T("@"), _T("%40"));

    //Creating Request
    Buffer = wxT("GET ") + File + wxT(" HTTP/1.1\r\n");
    Buffer += wxT("Authorization: Passport1.4 OrgVerb=GET,OrgURL=http%3A%2F%2Fmessenger%2Emsn%2Ecom,sign-in=");
    Buffer += Mail + wxT(",pwd=") + Pass  + wxT(",");
    Buffer += m_con_str;
    Buffer += wxT("\r\nHost: ") + Host + wxT("\r\n\r\n");
    
    #ifdef _DEBUG_
        //cout << "To send : {" << Buffer.mb_str(wxConvUTF8) << "}" << endl;
        wxLogMessage(_("To send : {%s}"), Buffer.c_str());
    #endif
    
    m_ssl = CSsl(SSLHost);
    
    if(m_ssl.Connect())
    {
	m_errstr = _("Unable to connect to SSL Server ") + SSLHost;
	return false;
    }
    
    #ifdef _DEBUG_
        //cout << "Connected" << endl;
        wxLogMessage(_("Connected"));
    #endif
    
    if(!m_ssl.Write(Buffer))
    {
        m_errstr = _("Unable to Write to SSL Server ") + SSLHost;
        return false;
    }
    
    m_ssl.Read(Buffer);
    
    #ifdef _DEBUG_
        //cout << "SSL Received : {" << Buffer.mb_str(wxConvUTF8) << "}" << endl;
        wxLogMessage(_("SSL Received : {%s}"), Buffer.c_str());
    #endif
    
    //Parsing Answer
    wxStringTokenizer NexusTok(Buffer, wxT("\r\n"));
    Buffer = NexusTok.GetNextToken();
    
    if(!Buffer.StartsWith(wxT("HTTP/1.1 200")))
    {
        if(Buffer.StartsWith(wxT("HTTP/1.1 401")))
            m_errstr = _("Incorrect E-Mail or Password, try again");
        else
            m_errstr = _("Unknown Auth Error (") + Buffer + wxT(")");
        return false;
    }
    
    while( !Buffer.StartsWith(wxT("Authentication-Info")) && NexusTok.HasMoreTokens() )
        Buffer = NexusTok.GetNextToken();
    
    if(!NexusTok.HasMoreTokens())
    {
        m_errstr = _("Error, did not found Authentication-Info");
        return false;
    }
    
    NexusTok.SetString(Buffer, wxT(" ,'")); // To cut ' ' ',' and '\''
    Buffer = NexusTok.GetNextToken();
    
    while( !Buffer.StartsWith(wxT("da-status")) && NexusTok.HasMoreTokens() )
        Buffer = NexusTok.GetNextToken();
    
    if(!NexusTok.HasMoreTokens())
    {
        m_errstr = _("Error, did not found Status");
        return false;
    }
    
    if(Buffer != wxT("da-status=success"))
    {
        m_errstr = _("Passport Failed : ") + Buffer;
        return false;
    }
    
    while( Buffer != wxT("from-PP=") && NexusTok.HasMoreTokens() )
        Buffer = NexusTok.GetNextToken();
    
    if(!NexusTok.HasMoreTokens())
    {
        m_errstr = _("Unable to find AuthID");
        return false;
    }
    
    m_authid = NexusTok.GetNextToken();
    
    return true;
}

#undef CLASS
