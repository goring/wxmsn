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

/** 	\file 	CConnec.cxx
	\author 	Neil "Superna" ARMSTRONG
	\date 	09/10/2005
    $Id$
*/

// Includes

#include <iostream>
#include <wx/tokenzr.h>

#include "CConnec.h"
#include "wxmsn.h"
#include "CSsl.h"
#include "Utils.h"

// Namespaces

using namespace std;
using namespace nsCore;
using namespace nsNetwork;
using namespace nsEvents;
using namespace nsUtils;

// Class
#define CLASS CConnec

/*
        public:
            CConnec(wxWindow * parent = NULL);
        
            void SetMail(wxString & mail);
            void SetPass(wxString & pass);
        
            void Connect();
            void Disconnect();
        
            void InputEvent(wxSocketEvent & Event);
        
        protected:
        
            wxTextCtrl  *   m_log;
            wxString        m_mail,
                            m_password;
            wxWindow    *   m_parent;
            wxSocketClient  m_socket;
        
        DECLARE_EVENT_TABLE()
*/

BEGIN_EVENT_TABLE(CLASS, wxEvtHandler)
EVT_SOCKET(-1, CLASS::InputEvent)
END_EVENT_TABLE()

CLASS::CLASS(wxEvtHandler * parent, CData * Data)
        : m_parse(&m_socket), m_data(Data), m_parent(parent)
{
    m_connum = 0;

    //Socket Options
    m_socket.SetEventHandler(*this);
    m_socket.SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    m_socket.Notify(true);
    m_socket.SetFlags(wxSOCKET_NOWAIT);

    m_data->SetConnected(false);
}

void CLASS::Connect()
{
    if(m_data->IsConnected())
    {
        SendLog(_("Already Connected !"));
        return;
    }

    SendTLog(_("Connect Asked"));
    m_data->SetConnected(false);

    SendTLog(_("Initializing Address"));
    //Adress Options, setting to first server
    m_address.Hostname(MSN_DS);
    m_address.Service(MSN_DS_PORT);

    SendTLog(_("Sending Connection"));
    m_socket.Connect(m_address);
    if( ! m_socket.WaitOnConnect() )
    {
        SendTLog(_("Connection Failed"));
        SendEvent(wxMsnEvent(wxMsnEventConnectionFailed));
        return;
    }

    SendLog(_("Connection Ok"));

    //Sending First Command
    MsnVER();
}

void CLASS::InputEvent(wxSocketEvent & Event)
{
    switch(Event.GetSocketEvent())
    {
    case wxSOCKET_LOST:
        {
            SendTLog(_("Lost Connection"));
            SendEvent(wxMsnEvent(wxMsnEventConnectionFailed));
            SendEvent(wxMsnEvent(wxMsnEventDisconnected));
            m_data->SetConnected(false);
        }
        break;
    case wxSOCKET_INPUT :
        break;
    default:
        SendTLog(_("Other Socket Event"));
        wxLogMessage(_("SockEvent ID : %i"), Event.GetSocketEvent());
        break;
    }

    m_parse.Parse();
    for(size_t i = 0 ; i < m_parse.Lines() ; ++i)
    {
        wxArrayString vecp = m_parse.GetWords(i);
        wxString   First = vecp[0];

        wxLogMessage(_("%s"), m_parse[i].c_str());
        if(First.StartsWith(wxT("VER"))) //Send CVR Command
            MsnCVR(vecp);
        if(First.StartsWith(wxT("CVR"))) //Send USR Command
            MsnUSR(vecp);
        if(First.StartsWith(wxT("XFR"))) //Server Info (NS or SB)
            if(!MsnXFR(vecp))
                return;
        if(First.StartsWith(wxT("QNG")))
            SendEvent(wxMsnEvent());
        if(First.StartsWith(wxT("CHL")))
            MsnChallenge(vecp);
        if(First.StartsWith(wxT("MSG")))
            SendEvent(wxMsnEvent(wxMsnEventConnectionP3));
        if(First.StartsWith(wxT("SYN")))
            SendEvent(wxMsnEvent(wxMsnEventConnectionP4));
        if(First.StartsWith(wxT("CHG")))
            MyStatusChanged(vecp);
        if(First.StartsWith(wxT("LST")))
            BuddyList(vecp);
        if(First.StartsWith(wxT("LSG")))
            GroupList(vecp);
        if(First.StartsWith(wxT("ILN")))
            BuddyOnline(vecp);
        if(First.StartsWith(wxT("UBX")))
            ParseUBX(m_parse[i]); //Exception, needs full string for server response
        if(First.StartsWith(wxT("UUX")))
            ParseUUX(vecp);
        if(First.StartsWith(wxT("NLN")))
            StatusChanged(vecp);
        if(First.StartsWith(wxT("FLN")))
            BuddyDisconnected(vecp);
        if(First.StartsWith(wxT("PRP")))
            PersoInfo(vecp);
        if(First.StartsWith(wxT("RNG")))
            ParseRNG(vecp);
        if(First.StartsWith(wxT("<")))
        {
            wxString temp = m_parse[i].Mid( 0, m_parse[i].rfind(wxT(">")) );
            m_parse[i].Remove(0, temp.size()+1 );
            ParseUBX(temp);
            --i;
        }
        if(First.StartsWith(wxT("USR")))
        {
            if(vecp[2] == wxT("OK"))
                SendEvent(wxMsnEvent(wxMsnEventConnectionP2));
            else if(!PassportIdent(vecp[4]))
                return;
        }

        if(m_socket.Error() && m_socket.LastError() != wxSOCKET_WOULDBLOCK)
        {
            SendTLog(_("A Socket Error Occured, aborting"));
            wxLogError(_("Socket Error : %i"), m_socket.LastError());
            //m_socket.Close();
            return;
        }
    }
}

void CLASS::MsnVER() //Exception, first call for connection
{
    SendTLog(_("Sending VER"));
    ++m_connum;
    m_command = wxString::Format(PROTO_VER, m_connum );
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnCVR(const wxArrayString & array)
{
    SendTLog(_("Sending CVR"));
    ++m_connum;
    m_command = wxString::Format(PROTO_CVR, m_connum, m_data->GetMail().c_str() );
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnUSR(const wxArrayString & array)
{
    SendTLog(_("Sending USR"));
    ++m_connum;
    m_command = wxString::Format(PROTO_USR, m_connum, m_data->GetMail().c_str() );
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

bool CLASS::MsnXFR(const wxArrayString & array)
{
    // Separated ft for SB sessions requests
    //wxStringTokenizer StrTok(data);
    //array[0] StrTok.GetNextToken(); //XFR
    //wxString rqNb = array[1];//StrTok.GetNextToken(); //n

    if(array.size() < 4)
    { 
        SendTLog(_("MsnXFR Protocol Error"));
        return false;
    }

    wxString type = array[2]; //StrTok.GetNextToken(); //NS or SB
    if(type.IsSameAs(_T("NS")))
        return ChangeServer(array[3]); //StrTok.GetNextToken()); //IP:PORT
    else if(type.IsSameAs(_T("SB")) && array.size() > 6)
    {
        //New SB session
        wxMsnEvent Event(wxMsnEventNewSbSession);
        Event.Add(array[1]); //Request Number
        Event.Add(array[4]);//StrTok.GetNextToken()); //IP
        Event.Add(array[5]);//StrTok.GetNextToken()); //CKI
        Event.Add(array[6]);//StrTok.GetNextToken()); //n.n.n
        SendEvent(Event);
        return true;
    }
}

bool CLASS::ChangeServer(const wxString & Server)
{
    SendEvent(wxMsnEvent(wxMsnEventConnectionP1));

    SendTLog(_("Switching to Server : ") + Server);
    int i = Server.Find(wxT(":"));
    if(!i)
    {
        SendLog(_("ServerString is Incorrect, Aborting"));
        return false;
    }
    m_address.Hostname(Server.Mid(0, i));
    m_address.Service(Server.Mid(i+1));

    SendLog(_("Server : ") + Server.Mid(0, i) + Server.Mid(i+1));

    SendTLog(_("Sending Connection"));
    m_socket.Connect(m_address);
    if( ! m_socket.WaitOnConnect() )
    {
        SendTLog(_("Connection Failed"));
        SendEvent(wxMsnEvent(wxMsnEventConnectionFailed));
        return false;
    }

    SendLog(_("Connection Ok"));

    //Sending First Thing
    MsnVER();

    return true;
}

// New threaded implementation with caller + callback
/*
void CLASS::CallPassportIdent(const wxString & lcString)
{
    AuthThread * t = new AuthThread(this, m_data->GetMail(), m_data->GetPassword(), lcString);
    t->Run();
    // Can set timer for timeout
}

void CLASS::CallbackPassportIdent(bool success, const wxString & AuthID)
{
    SendEvent(wxMsnEvent(wxMsnEventConnectionP2));

    if(success)
    {
        //Auth was successful
        m_data->SetPassportID(AuthID);

        SendTLog(_("Successful Passport Authentification"));

        MsnFUSR();

        InputEvent();
    }
    else
    {
        SendLog(m_auth.GetError());
        SendEvent(wxMsnEvent(wxMsnEventConnectionFailed));
        SendEvent(wxMsnEvent(wxMsnEventDisconnected));

        Disconnect();
    }
}
*/
bool CLASS::PassportIdent(const wxString & lcString)
{
    SendEvent(wxMsnEvent(wxMsnEventConnectionP2));

    if(m_auth.Auth(m_data->GetMail(), m_data->GetPassword(), lcString))
    {
        //Auth was successful
        m_data->SetPassportID(m_auth.GetAuthID());

        SendTLog(_("Successful Passport Authentification"));

        MsnFUSR();

        return true;
    }

    SendLog(m_auth.GetError());
    SendEvent(wxMsnEvent(wxMsnEventConnectionFailed));
    SendEvent(wxMsnEvent(wxMsnEventDisconnected));

    Disconnect();

    return false;
}

void CLASS::AskSwitchboardSession()
{
    SendTLog(_("Requesting new Switchboard Session"));
    ++m_connum;
    m_command = wxString::Format(PROTO_SBXFR, m_connum );
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnFUSR()
{
    SendTLog(_("Sending Final USR Auth"));
    ++m_connum;
    m_command = wxString::Format(PROTO_FUSR, m_connum, m_data->GetPassportID().c_str() );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnChallenge(const wxArrayString & array)
{
    m_command = wxString::Format(wxT("QRY %i %s 32\r\n%s"),
                                 m_connum, MSN_CHALLENGE_PROD, MSN_Challenge(array[2]).c_str() );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

/*
void CLASS::SetPresence(int type)
{
    SendTLog(_("Set Presence"));
    ++m_connum;
    m_command = wxString::Format(_("CHG %i NLN 0\r\nPNG\r\n"), m_connum );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
	wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}
*/

void CLASS::Sync()
{
    SendTLog(_("Sync"));
    ++m_connum;
    m_command = wxString::Format(wxT("SYN %i 2005-04-23T18:57:44.8130000-07:00 2005-04-23T18:57:54.2070000-07:00\r\n"), m_connum );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

/*
void CLASS::MsnConnected()
{
    m_data->SetConnected(true);
}
*/

void CLASS::SendEvent(wxMsnEvent Event)
{
    Event.SetEventObject( this );
    m_parent->ProcessEvent( Event );
}

void CLASS::Disconnect()
{
    if(m_socket.IsDisconnected())
        SendLog(_("Not Connected"));
    else
    {
        m_command = wxT("OUT\r\n");
        m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
        SendEvent(wxMsnEvent(wxMsnEventDisconnected));
        m_socket.Close();
        m_data->SetConnected(false);
    }
}

void CLASS::SetFName(wxString & Name)
{
    //Cuts name at 300 cars
    if(Name.size() > 300)
        Name = Name.Mid(0,300);

    SendTLog(_("Changing Friendly Name"));
    ++m_connum;
    m_command = wxString::Format(PROTO_PRP_MFN, m_connum, Name.c_str());
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::BuddyList(const wxArrayString & array) //const wxString & Params)
{
    // array[0]; LST
    // array[1]; N=mail
    // array[2]; F=friendly name
    // array[3]; C=Guid
    // array[4]; List Position

    if(array.size() < 5)
    {
        SendTLog(_("BuddyList Protocol Error"));
        return;
    }

    wxMsnEvent Event(wxMsnEventNewContact);
    Event.Add(array[1].Mid(2));
    Event.Add(array[4]);
    Event.Add(array[2].Mid(2));
    SendEvent(Event);

    if(array.size() > 5)
    {
        wxString Groups = array[5]; //Group Hashs
        Groups.Replace(wxT(","), wxT(";"));
        Event.SetId(wxMsnEventAssignGroup);
        Event.Add(array[1].Mid(2));
        Event.SetString(Groups);
        SendEvent(Event);
    }

}

void CLASS::GroupList(const wxArrayString & array) //const wxString & Params)
{
    //array[0]; LSG
    //array[1]; Name
    //array[2]; Hash
    if(array.size() >= 3)
    {
        wxMsnEvent Event(wxMsnEventNewGroup);
        Event.Add(array[1]);
        Event.Add(array[2]);
        SendEvent(Event);
    }
    else SendTLog(_("GroupList Protocol Error"));
}

void CLASS::BuddyOnline(const wxArrayString & array) //const wxString & Params)
{
    //array[0]; ILN
    //array[1]; num
    //array[2]; Status
    //array[3]; ID
    //array[4]; FriendlyName
    
    if(array.size() >= 5)
    {   
        wxMsnEvent Event(wxMsnEventContactConnected);
        Event.Add(array[3]);
        Event.Add(array[2]);
        Event.Add(array[4]);
        SendEvent(Event);
    }
    else SendTLog(_("BuddyOnline Protocol Error"));
}

void CLASS::SetStatus(ONLINE_STATUS status)
{
    SendTLog(_("Set Status ")+GetTxtStatus(status));
    ++m_connum;
    m_command = wxString::Format(wxT("CHG %i %s 0\r\nPNG\r\n"), m_connum,  GetTxtStatus(status).c_str());
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::SetPStatus(wxString Msg)
{
    Msg.Replace(wxT("<"), wxT("&#60"));
    Msg.Replace(wxT(">"), wxT("&#62"));

    last_uux = Msg;

    Msg = wxT("<Data><PSM>") + Msg + wxT("</PSM><CurrentMedia></CurrentMedia></Data>");

    SendTLog(_("Changing Personnal Status"));
    ++m_connum;
    m_command = wxString::Format(PROTO_UUX, m_connum, Msg.size(), Msg.c_str());
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::SetCurrentMedia(wxString psm, wxString Title, wxString Artist)
{
    Title.Replace(wxT("<"), wxT("&#60"));
    Title.Replace(wxT(">"), wxT("&#62"));
    Artist.Replace(wxT("<"), wxT("&#60"));
    Artist.Replace(wxT(">"), wxT("&#62"));
    psm.Replace(wxT("<"), wxT("&#60"));
    psm.Replace(wxT(">"), wxT("&#62"));
    
    last_uux = _T("<Data>") + psm + _T("<PSM></PSM><CurrentMedia>\\0Music\\01\\0{0}");
    if(!Artist.IsEmpty())
        last_uux += _T(" - {1}");
    last_uux += _T("\\0") + Title + _T("\\0");
    if(!Artist.IsEmpty())
        last_uux += Artist;
    last_uux += _T("\\0\\0\\0</CurrentMedia></Data>");
    SendTLog(_("Changing Personnal Status"));
    ++m_connum;
    m_command = wxString::Format(PROTO_UUX, m_connum, last_uux.size(), last_uux.c_str());
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
    
    last_uux = _T("(8) ") + Title;
    if(!Artist.IsEmpty())
        last_uux += _T(" - ") + Artist;
}

void CLASS::ParseUBX(const wxString & Params)
{
    //<Data><PSM>...</PSM><CurrentMedia>\0Music\01\0{0} - {1}\018 Piste 18\0\0\0\0</CurrentMedia></Data>
    if(Params.StartsWith(wxT("<Data>")))
    {
        wxStringTokenizer StrTok(Params, wxT(">"));
        StrTok.GetNextToken(); //<Data
        StrTok.GetNextToken(); //<PSM
        wxString PSM = StrTok.GetNextToken(); //Message</PSM
        StrTok.GetNextToken(); //<CurrentMedia
        wxString Media = StrTok.GetNextToken(); //Message</CurrentMedia
        PSM.Remove(PSM.size()-5);
        Media.Remove(Media.size()-14);
        if(Media.size())
        {
            Media.Replace(wxT("\\0"), wxT("\\"));
            StrTok.SetString(Media, wxT("\\"));
            StrTok.GetNextToken(); //rien
            wxString What = StrTok.GetNextToken(); //Music/Games/Office/...
            wxString Bool = StrTok.GetNextToken(); // 1 ou 0
            wxString Formater = StrTok.GetNextToken(); // {0} - {1]
            wxString String1 = StrTok.GetNextToken(); // {0} =
            wxString String2 = StrTok.GetNextToken(); // {1} =
            Formater.Replace(wxT("{0}"), String1);
            Formater.Replace(wxT("{1}"), String2);
            if(Bool == wxT("1"))
                PSM = wxT("(8) ") + Formater;
        }

        wxMsnEvent Event(wxMsnEventContactPStatus);
        Event.Add(last_ubx);
        Event.Add(PSM);
        //Event.SetString(last_ubx + _(" ") + PSM);
        SendEvent(Event);
    }
    else
    {
        wxStringTokenizer StrTok(Params);
        StrTok.GetNextToken(); //UBX
        last_ubx = StrTok.GetNextToken(); //mail
    }
}

void CLASS::ParseUUX(const wxArrayString & array) //const wxString & Params)
{
    // TODO Check Response
    wxMsnEvent Event(wxMsnEventPersoPStatus);
    Event.SetString(last_uux);
    SendEvent(Event);
}

void CLASS::StatusChanged(const wxArrayString & array) //const wxString & Params)
{
    //array[0]; NLN
    //array[1]; Status
    //array[2]; Mail
    //array[3]; Friendly Name
    if(array.size() >= 4)
    {
        wxMsnEvent Event(wxMsnEventContactStatus);
        Event.Add(array[2]);
        Event.Add(array[1]);
        Event.Add(array[3]);
        SendEvent(Event);
    }
    else SendTLog(_("StatusChanged Protocol Error"));
}

void CLASS::MyStatusChanged(const wxArrayString & array) //const wxString & Params)
{
    if(array.size() >= 3)
    {
        wxMsnEvent Event(wxMsnEventConnected);
        Event.SetString(array[2]); //Status);
        SendEvent(Event);
        m_data->SetConnected(true);
    }
    else SendTLog(_("MyStatusChanged Protocol Error"));
}

void CLASS::ParseRNG(const wxArrayString & array) //const wxString & Params)
{
    if(array.size() >= 7)
    {
        wxMsnEvent Event(wxMsnEventChatInvitation);
        //array[0] RNG
        Event.Add(array[1]); //id
        Event.Add(array[2]); //address
        //array[3] CKI
        Event.Add(array[4]); //id
        Event.Add(array[5]); //email
        Event.Add(array[6]); //name
        SendEvent(Event);
    }
    else SendTLog(_("ParseRNG Protocol Error")); 
}

void CLASS::PersoInfo(const wxArrayString & array) //const wxString & Params)
{
    //array[0]; PRP
    //array[1]; num or MFN/PHW/HSB/WWE
    //array[2]; MFN or Name
    //array[3]; Name or none

    wxMsnEvent Event(wxMsnEventPersoInfo);
    if(array[1] == wxT("MFN") && array.size()>2)
        Event.SetString(array[2]);
    else if(array[2] == wxT("MFN") && array.size()>3)
        Event.SetString(array[3]);
    else
    {
        SendTLog(_("PersoInfo Protocol Warning"));
        return;
    }
    SendEvent(Event);
}

void CLASS::BuddyDisconnected(const wxArrayString & array)
{
    wxMsnEvent Event(wxMsnEventContactDisconnected);
    Event.SetString(array[1]);
    SendEvent(Event);
}

void CLASS::Refresh()
{
    if(m_data->IsConnected())
    {
        m_command = wxT("PNG\r\n");
        m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
    }
}

#undef CLASS
