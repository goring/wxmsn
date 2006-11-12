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

/** 	\file 	CConnec.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	24/11/2005
    $Id$
*/

// Include Protection

#ifndef _CCONNEC_H_
#define _CCONNEC_H_

// Includes

#include <wx/wx.h>
#include <wx/socket.h>

#include "CMsnAuth.h"
#include "Events.h"
#include "CParseSocket.h"
#include "CData.h"

// Defines
#define _MCLASS_START_ \
    public:\
    CVER(CConnec * c) : m_c(c) {} \
    virtual void operator () (int i)
#define _MCLASS_END_ \
    protected:\
    CConnec * m_c;

namespace nsCore
{
    /*class MsnData
    {
        public:
            void SetMail(const wxString & Mail) { m_mail = Mail; }
            void SetPassword(const wxString & Password) { m_password = Password; }
            void SetPassportID(const wxString & passportid) { m_passportid = passportid; }
        
            wxString & GetMail() { return  m_mail; }
            wxString & GetPassword() { return m_password; }
            wxString & GetPassportID() { return m_passportid; }
            
            void SetConnected(bool val) { m_con = val; }
            bool IsConnected() { return m_con; }
        
        private:
            wxString    m_mail,
                        m_password,
                        m_passportid;
            bool m_con;
    };*/
    
    class CConnec : public wxEvtHandler
    {
        public:
            CConnec(wxEvtHandler * parent, CData * Data);
        
            void SetMail(const wxString & mail);
            void SetPass(const wxString & pass);
        
            void Connect();
            void Disconnect();
        
            void AskSwitchboardSession();
            void SetFName(wxString & Name);
            void SetPStatus(wxString Msg);
            void SetCurrentMedia(wxString psm, wxString Title, wxString Artist);
            void SetStatus(ONLINE_STATUS status = ONLINE);
            void Sync();
        
            void Refresh();
        
        protected:
            
            class CMethodClass
            {
                public:
                    virtual void operator () (int i) = 0;
                protected:
                    virtual ~CMethodClass() {}
            };

            class CVER : public CMethodClass
            {
                    _MCLASS_START_
                    {
                        m_c->MsnVER();
                    }
                    _MCLASS_END_
            };
            
            void InputEvent(wxSocketEvent & Event);
            void SendEvent(nsEvents::wxMsnEvent Event);
            
            void SendLog(const wxString & Str) { SendEvent(nsEvents::wxMsnEvent(Str)); }
            //void SendTLog(const wxString & Str) { SendEvent(wxMsnEvent(wxNow() + _(" : ") + Str)); }
            void SendTLog(const wxString & Str) { wxLogMessage(Str); }
            
            //MSN Commands
            void MsnVER();
            void MsnCVR();
            void MsnUSR();
            //bool MsnXFR(const wxString & data);
            bool MsnXFR(const wxArrayString & array);
            bool ChangeServer(const wxString & Server);
            bool PassportIdent(const wxString & lcString);
            void MsnFUSR();
            //void MsnConnected();
            void MsnChallenge(const wxArrayString & array);
            
            void BuddyList(const wxArrayString & array); //const wxString & Params);
            void GroupList(const wxArrayString & array); //const wxString & Params);
            void BuddyOnline(const wxArrayString & array); //const wxString & Params);
            void ParseUBX(const wxString & Params);
            void ParseUUX(const wxArrayString & array); //const wxString & Params);
            void StatusChanged(const wxArrayString & array); //const wxString & Params);
            void MyStatusChanged(const wxArrayString & array); //const wxString & Params);
            void BuddyDisconnected(const wxArrayString & array);
            void ParseRNG(const wxArrayString & array); //const wxString & Params);
            
            void PersoInfo(const wxArrayString & array); //const wxString & Params);
        
            wxString        m_command,
                            last_ubx,
                            last_uux;
            
            wxSocketClient  m_socket;
            wxIPV4address   m_address;
            
            nsNetwork::CMsnAuth         m_auth;
            nsNetwork::CParseSocket     m_parse;
            CData                   * m_data;
            wxEvtHandler    *   m_parent;
        
            int             m_connum;
        
        DECLARE_EVENT_TABLE()
    };
	
} // nsCore
		
#endif
