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
using namespace nsConsole;
using namespace nsNetwork;
using namespace nsEvents;
using namespace nsUtils;

// Class
#define CLASS wxMsnCon

/*
        public:
            wxMsnCon(wxWindow * parent = NULL);
        
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
    //SendTLog(_("Input on the Socket"));
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
        //SendTLog(_("Input on the Socket"));
        break;
    default:
        SendTLog(_("Other Socket Event"));
        //cout << "SockEvent ID :" << Event.GetSocketEvent() << endl;
        wxLogMessage(_("SockEvent ID : %i"), Event.GetSocketEvent());
        break;
    }

    //Tests What to do
    m_parse.Parse();
    //cout << "Entering Loop for " << m_parse.Lines() << " lines" << endl;
    for(size_t i = 0 ; i < m_parse.Lines() ; ++i)
    {
        wxArrayString vecp = m_parse.GetWords(i);
        wxString   First = vecp[0];//m_parse.GetString(i, 0),
        //                   P1 = m_parse.GetString(i, 1),
        //                        P2 = m_parse.GetString(i, 2);

        //cout << m_parse[i].mb_str(wxConvUTF8)  << endl;
        wxLogMessage(_("%s"), m_parse[i].c_str());
        if(First.StartsWith(wxT("VER"))) //Send CVR Command
            MsnCVR();
        if(First.StartsWith(wxT("CVR"))) //Send USR Command
            MsnUSR();
        if(First.StartsWith(wxT("XFR"))) //Server Info (NS or SB)
            if(!MsnXFR(m_parse[i]))
                return;
        if(First.StartsWith(wxT("QNG")))
            SendEvent(wxMsnEvent());
        if(First.StartsWith(wxT("CHL")))
            MsnChallenge(vecp[2]);
        if(First.StartsWith(wxT("MSG")))
            SendEvent(wxMsnEvent(wxMsnEventConnectionP3));
        if(First.StartsWith(wxT("SYN")))
            SendEvent(wxMsnEvent(wxMsnEventConnectionP4));
        if(First.StartsWith(wxT("CHG")))
            MyStatusChanged(m_parse[i]);
        /*{
            SendEvent(wxMsnEvent(wxMsnEventConnected));
            MsnConnected();
        }*/
        if(First.StartsWith(wxT("LST")))
            BuddyList(m_parse[i]);
        if(First.StartsWith(wxT("LSG")))
            GroupList(m_parse[i]);
        if(First.StartsWith(wxT("ILN")))
            BuddyOnline(m_parse[i]);
        if(First.StartsWith(wxT("UBX")))
            ParseUBX(m_parse[i]);
        if(First.StartsWith(wxT("UUX")))
            ParseUUX(m_parse[i]);
        if(First.StartsWith(wxT("NLN")))
            StatusChanged(m_parse[i]);
        if(First.StartsWith(wxT("FLN")))
            BuddyDisconnected(vecp[1]);
        if(First.StartsWith(wxT("PRP")))
            PersoInfo(m_parse[i]);
        if(First.StartsWith(wxT("RNG")))
            ParseRNG(m_parse[i]);
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
            //cout << "Error Occured : " << m_socket.LastError() << endl;
            wxLogError(_("Socket Error : %i"), m_socket.LastError());
            //m_socket.Close();
            return;
        }
    }
}

void CLASS::MsnVER()
{
    SendTLog(_("Sending VER"));
    ++m_connum;
    m_command = wxString::Format(PROTO_VER, m_connum );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnCVR()
{
    SendTLog(_("Sending CVR"));
    ++m_connum;
    m_command = wxString::Format(PROTO_CVR, m_connum, m_data->GetMail().c_str() );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

void CLASS::MsnUSR()
{
    SendTLog(_("Sending USR"));
    ++m_connum;
    m_command = wxString::Format(PROTO_USR, m_connum, m_data->GetMail().c_str() );
    //cout << "Sending :" << m_command.mb_str(wxConvUTF8) << endl;
    wxLogMessage(_("Sending : %s"), m_command.c_str());
    m_socket.Write(m_command.mb_str(wxConvUTF8), m_command.size());
}

bool CLASS::MsnXFR(const wxString & data)
{
    // Separated ft for SB sessions requests
    wxStringTokenizer StrTok(data);
    StrTok.GetNextToken(); //XFR
    StrTok.GetNextToken(); //n
    wxString type = StrTok.GetNextToken(); //NS or SB
    if(type.IsSameAs(_T("NS")))
        return ChangeServer(StrTok.GetNextToken()); //IP:PORT
    else if(type.IsSameAs(_T("SB")))
    {
        //New SB session
        wxMsnEvent Event(wxMsnEventNewSbSession);
        Event.Add(StrTok.GetNextToken()); //IP
        Event.Add(StrTok.GetNextToken()); //CKI
        Event.Add(StrTok.GetNextToken()); //n.n.n
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

void CLASS::MsnChallenge(const wxString & challenge)
{
    m_command = wxString::Format(wxT("QRY %i %s 32\r\n%s"),
                                 m_connum, MSN_CHALLENGE_PROD, MSN_Challenge(challenge).c_str() );
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

void CLASS::BuddyList(const wxString & Params)
{
    wxStringTokenizer StrTok(Params, wxT(" "));
    StrTok.GetNextToken(); //LST
    wxString Mail = StrTok.GetNextToken(); //N=mail
    wxString FName = StrTok.GetNextToken(); //F=friendly name
    StrTok.GetNextToken(); //C=Guid
    wxString ListBit = StrTok.GetNextToken(); //List Position

    wxMsnEvent Event(wxMsnEventNewContact);
    Event.Add(Mail.Mid(2));
    Event.Add(ListBit);
    Event.Add(FName.Mid(2));
    //Event.SetString(Mail.Mid(2) + _(";") + ListBit + _(";") + FName.Mid(2));
    SendEvent(Event);

    if(StrTok.HasMoreTokens())
    {
        wxString Groups = StrTok.GetNextToken(); //Group Hashs
        Groups.Replace(wxT(","), wxT(";"));
        Event.SetId(wxMsnEventAssignGroup);
        Event.Add(Mail.Mid(2));
        Event.SetString(Groups);
        SendEvent(Event);
    }

}

void CLASS::GroupList(const wxString & Params)
{
    wxStringTokenizer StrTok(Params);
    StrTok.GetNextToken(); //LSG
    wxString Name = StrTok.GetNextToken(); //Name
    wxString Hash = StrTok.GetNextToken(); //Hash
    wxMsnEvent Event(wxMsnEventNewGroup);
    Event.Add(Name);
    Event.Add(Hash);
    //Event.SetString(Name + _(";") + Hash);
    SendEvent(Event);
}

void CLASS::BuddyOnline(const wxString & Params)
{
    wxStringTokenizer StrTok(Params);
    StrTok.GetNextToken(); //ILN
    StrTok.GetNextToken(); //num

    wxString Status = StrTok.GetNextToken(); //status
    wxString ID = StrTok.GetNextToken(); //mail
    wxString FriendlyName = StrTok.GetNextToken(); //mail

    wxMsnEvent Event(wxMsnEventContactConnected);
    //Event.SetString(ID + _(";") + Status + _(";") + FriendlyName);
    Event.Add(ID);
    Event.Add(Status);
    Event.Add(FriendlyName);
    SendEvent(Event);
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
        wxString mail;
        wxStringTokenizer StrTok(Params);
        StrTok.GetNextToken(); //UBX
        last_ubx = StrTok.GetNextToken(); //mail
    }
}

void CLASS::ParseUUX(const wxString & Params)
{
    // TODO Check Response
    wxMsnEvent Event(wxMsnEventPersoPStatus);
    Event.SetString(last_uux);
    SendEvent(Event);
}

void CLASS::StatusChanged(const wxString & Params)
{
    //wxMsnEventContactStatus
    wxStringTokenizer StrTok(Params);
    StrTok.GetNextToken(); //NLN
    wxString Status = StrTok.GetNextToken(); //Status
    wxString Mail = StrTok.GetNextToken(); //Mail
    wxString FName = StrTok.GetNextToken(); //Friendly Name
    wxMsnEvent Event(wxMsnEventContactStatus);
    Event.Add(Mail);
    Event.Add(Status);
    Event.Add(FName);
    //Event.SetString(Mail + _(";") + Status + _(";") + FName);
    SendEvent(Event);
}

void CLASS::MyStatusChanged(const wxString & Params)
{
    wxStringTokenizer StrTok(Params);
    StrTok.GetNextToken();
    StrTok.GetNextToken();
    wxString Status = StrTok.GetNextToken(); //Status
    //wxLogMessage(_("Status changed %s"), Status.c_str());
    wxMsnEvent Event(wxMsnEventConnected);
    Event.SetString(Status);
    SendEvent(Event);
    m_data->SetConnected(true);
}

void CLASS::ParseRNG(const wxString & Params)
{
    wxStringTokenizer StrTok(Params);
    wxMsnEvent Event(wxMsnEventChatInvitation);
    StrTok.GetNextToken(); //RNG
    Event.Add(StrTok.GetNextToken()); //id
    Event.Add(StrTok.GetNextToken()); //address
    StrTok.GetNextToken();
    Event.Add(StrTok.GetNextToken()); //id
    Event.Add(StrTok.GetNextToken()); //email
    Event.Add(StrTok.GetNextToken()); //name
    SendEvent(Event);    
}

void CLASS::PersoInfo(const wxString & Params)
{
    //wxMsnEventPersoInfo
    wxStringTokenizer StrTok(Params);
    StrTok.GetNextToken(); //PRP
    wxString Info = StrTok.GetNextToken(); //MFN/PHW/HSB/WWE
    if(Info == wxT("MFN") || StrTok.GetNextToken() == wxT("MFN"))
    {
        wxString FName = StrTok.GetNextToken(); //Friendly Name
        wxMsnEvent Event(wxMsnEventPersoInfo);
        Event.SetString(FName);
        SendEvent(Event);
    }
}

void CLASS::BuddyDisconnected(const wxString & Params)
{
    wxMsnEvent Event(wxMsnEventContactDisconnected);
    Event.SetString(Params);
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
