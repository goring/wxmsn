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

/** 	\file 	CSsl.h
	\author 	Neil "Superna" ARMSTRONG
	\date 	14/11/2005
    $Id$
*/

// Include Protection

#ifndef __CSSL_H__
#define __CSSL_H__

// Includes

#include <wx/wx.h>
#include "config.h"
#ifdef USE_OPENSSL
	#include <openssl/bio.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
#elif USE_GNUTLS
	#ifdef __WXMSW__
        #include <winsock2.h>
	#else
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
	#endif
	#include <gnutls/gnutls.h>
#endif

namespace nsNetwork
{
	class CSsl
	{
		public:
			CSsl(wxString Address);
		
			bool Reconnect(wxString Address);
			bool Reconnect();
		
			bool Connect();
		
			bool Write(wxString & Data);
		
			bool Read(wxString & Data);
		
			bool Close();
		
		private:
			#ifdef USE_OPENSSL
            //{
                BIO 		*	m_sbio;
                SSL_CTX 	*	m_ctx;
                SSL 		*	m_ssl;
            //}
			#elif USE_GNUTLS
            //{
                #ifdef __WXMSW__
                    SOCKET        m_sd;
                #else
                    int 				m_sd;
                #endif
        
                struct sockaddr_in 	m_sa;
                gnutls_session	 	m_session;
                gnutls_certificate_client_credentials 
                                m_cred;
            //}
			#endif
			wxString 		m_address;

			bool Init();
		
	}; //class CSsl
    
    wxString MSN_Challenge(const wxString & Phrase);
	
}//Namespace nsSsl

#endif
