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

/** 	\file 	CSsl.cxx
	\author 	Neil "Superna" ARMSTRONG
	\date 	24/11/2005
*/

// Includes

#include "CSsl.h"
#include <wx/socket.h>
#include <errno.h>

// Namespaces

using namespace std;
using namespace nsNetwork;

// Class 
#define CLASS CSsl

/*		
	BIO 		*	m_sbio;
	CLASS_CTX 	*	m_ctx;
	CLASS 		*	m_ssl;
	wxString 		m_address;
*/

#ifdef USE_GNUTLS
int cert_type_priority[2] = { GNUTLS_CRT_X509, 0 };
#endif

CLASS::CLASS(wxString Address)
	: m_address(Address)
{
	#ifdef USE_OPENSSL
	ERR_load_crypto_strings();
	ERR_load_SSL_strings();
	OpenSSL_add_all_algorithms();
	m_ctx = SSL_CTX_new(SSLv23_client_method());
	#elif USE_GNUTLS
	gnutls_global_init ();
	gnutls_certificate_allocate_credentials(&m_cred);
	gnutls_certificate_set_x509_trust_file(m_cred, "ca.pem", GNUTLS_X509_FMT_PEM);
    #ifdef __WXMSW__
    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    #endif
	#endif
	
	Init();
}

bool CLASS::Reconnect(wxString Address)
{
	m_address = Address;
	Close();
	Init();
	return Connect();
} //CLASS::Reconnect(wxString & Address)

bool CLASS::Reconnect()
{
	Close();
	Init();
	return Connect();
} //CLASS::Reconnect
		
bool CLASS::Connect()
{
	#ifdef USE_OPENSSL
	if( BIO_do_connect(m_sbio) <= 0 ) 
	{
		printf("Error connecting to SSL server '%s'\n", (const char *)m_address.mb_str(wxConvUTF8));
		ERR_print_errors_fp(stderr);
		return 1;
	}
	
	if( BIO_do_handshake(m_sbio) <= 0 ) 
	{
		printf("Error establishing SSL connection\n");
		ERR_print_errors_fp(stderr);
		return 1;
	}
	#elif USE_GNUTLS
	int err;
	
	err = connect(m_sd, (struct sockaddr *) & m_sa, sizeof(m_sa));
	
	if (err < 0) 
	{
		printf( "Connect error %s\n", strerror (errno));
		return false;
	}
	
	gnutls_transport_set_ptr(m_session, (gnutls_transport_ptr) m_sd);
	
	err = gnutls_handshake(m_session);

	if (err < 0) {
		printf("*** Handshake failed\n");
		gnutls_perror(err);
		return false;
	}
	
	#endif
	
	return 0;
} //CLASS::Connect
		
bool CLASS::Write(wxString & Data)
{
	#ifdef USE_OPENSSL
	return BIO_puts(m_sbio, (const char *)Data.mb_str(wxConvUTF8));
	#elif USE_GNUTLS
	return gnutls_write(m_session, (const char *)Data.mb_str(wxConvUTF8), Data.size());
	#endif
	
} //CLASS::Write
		
bool CLASS::Read(wxString & Data)
{
	char TMP[3001];
	int len = 1;
	
	Data.clear();
	
	while(len > 0) 
	{
		#ifdef USE_OPENSSL
		len = BIO_read(m_sbio, (char *)TMP, 3000);
		#elif USE_GNUTLS
		len = gnutls_read(m_session, (char *)TMP, 3000);
		#endif
		
        	TMP[len] = '\0';
        	Data.Append(wxString(wxConvUTF8.cMB2WC(TMP)));
	}

	return Data.size();
} //CLASS::Read
		
bool CLASS::Close()
{
	#ifdef USE_OPENSSL
	BIO_free_all(m_sbio);
	#elif USE_GNUTLS
	gnutls_bye(m_session, GNUTLS_SHUT_RDWR);
	#ifdef __WXMSW__
        shutdown(m_sd, SD_BOTH);
	#else	
	shutdown(m_sd, SHUT_RDWR);
	#endif
	close(m_sd);
	gnutls_deinit(m_session);
	gnutls_certificate_free_credentials(m_cred);
	#endif
	return true;
} //CLASS::Close

bool CLASS::Init()
{
	#ifdef USE_OPENSSL
	m_sbio = BIO_new_ssl_connect(m_ctx);
	
	BIO_get_ssl(m_sbio, &m_ssl);
	
	SSL_set_mode(m_ssl, SSL_MODE_AUTO_RETRY);
	
	BIO_set_conn_hostname(m_sbio, (const char *)m_address.mb_str(wxConvUTF8));
	#elif USE_GNUTLS
	int i;
	
	wxIPV4address address;
	i = m_address.Find(wxT(":"));
	address.Hostname(m_address.Mid(0, i));
	
	gnutls_init(&m_session, GNUTLS_CLIENT);
	gnutls_set_default_priority (m_session);

	gnutls_certificate_type_set_priority(m_session, cert_type_priority);

	gnutls_credentials_set(m_session, GNUTLS_CRD_CERTIFICATE, m_cred);

	//Socket connect
    #ifdef __WXMSW__
    m_sd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    #else
	m_sd = socket(AF_INET, SOCK_STREAM, 0);
    #endif
	
	memset(&m_sa, '\0', sizeof(m_sa));
	m_sa.sin_family = AF_INET;
	
	if(m_address.Mid(i+1) == wxT("https"))
		m_sa.sin_port = htons(443);
	else if(m_address.Mid(i+1) == wxT("http"))
		m_sa.sin_port = htons(80);
	else
		m_sa.sin_port = htons(atoi(m_address.Mid(i).mb_str(wxConvUTF8)));
    
	#ifdef __WXMSW__
	m_sa.sin_addr.s_addr = inet_addr(address.IPAddress().mb_str(wxConvUTF8));
	#else
	inet_pton(AF_INET, address.IPAddress().mb_str(wxConvUTF8), &m_sa.sin_addr);
	#endif
	
	#endif
	return true;	
} //CLASS::Init

//Old MSNP8 Challenge
/*wxString ssl::wxMD5(wxString & Phrase)
{
    unsigned char hash[16];
    wxString result;
    MD5((unsigned char *)(const char *)Phrase.mb_str(wxConvUTF8), Phrase.size(), hash);
    for(int i = 0 ; i < 16; ++i)
        result.Append(wxString::Format(_("%02x"), (int)hash[i]));
    cout << "Hash of :" << Phrase.mb_str(wxConvUTF8) << "(" << Phrase.size() << ")=>" << result.mb_str(wxConvUTF8) << endl;
    return result;
}*/

extern "C" void MSN_handle_chl(char *input, char *output);

wxString nsNetwork::MSN_Challenge(const wxString & Phrase)
{
	char hash[33];
	wxString result;
    	MSN_handle_chl((char *)(const char *)Phrase.mb_str(wxConvUTF8), hash);
	hash[32]='\0';
	return wxString(wxConvUTF8.cMB2WC(hash));
}	

#undef CLASS
