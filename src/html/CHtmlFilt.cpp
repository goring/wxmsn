/////////////////////////////////////////////////////////////////////////////
// Name:        htmlfilt.cpp
// Purpose:     CHtmlFilter - input filter for translating into HTML format
// Author:      Vaclav Slavik
// RCS-ID:      $Id: htmlfilt.cpp,v 1.21 2004/09/27 19:15:04 ABX Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CHtmlFilt.h"
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include "wx/strconv.h"
#include "CHtmlFilt.h"
#include "CHtmlWin.h"

// utility function: read a wxString from a wxInputStream
static void ReadString(wxString& str, wxInputStream* s, wxMBConv& conv)
{
    size_t streamSize = s->GetSize();

    if (streamSize == ~(size_t)0)
    {
        const size_t bufSize = 4095;
        char buffer[bufSize+1];
        size_t lastRead;

        do
        {
            s->Read(buffer, bufSize);
            lastRead = s->LastRead();
            buffer[lastRead] = 0;
            str.Append(wxString(buffer, conv));
        }
        while (lastRead == bufSize);
    }
    else
    {
        char* src = new char[streamSize+1];
        s->Read(src, streamSize);
        src[streamSize] = 0;
        str = wxString(src, conv);
        delete[] src;
    }
}

/*

There is code for several default filters:

*/

IMPLEMENT_ABSTRACT_CLASS(CHtmlFilter, wxObject)

//--------------------------------------------------------------------------------
// CHtmlFilterPlainText
//          filter for text/plain or uknown
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(CHtmlFilterPlainText, CHtmlFilter)

bool CHtmlFilterPlainText::CanRead(const wxFSFile& WXUNUSED(file)) const
{
    return true;
}



wxString CHtmlFilterPlainText::ReadFile(const wxFSFile& file) const
{
    wxInputStream *s = file.GetStream();
    wxString doc, doc2;

    if (s == NULL) return wxEmptyString;
    ReadString(doc, s, wxConvISO8859_1);

    doc.Replace(wxT("&"), wxT("&amp;"), true);
    doc.Replace(wxT("<"), wxT("&lt;"), true);
    doc.Replace(wxT(">"), wxT("&gt;"), true);
    doc2 = wxT("<HTML><BODY><PRE>\n") + doc + wxT("\n</PRE></BODY></HTML>");
    return doc2;
}





//--------------------------------------------------------------------------------
// CHtmlFilterImage
//          filter for image/*
//--------------------------------------------------------------------------------

class CHtmlFilterImage : public CHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(CHtmlFilterImage)

    public:
        virtual bool CanRead(const wxFSFile& file) const;
        virtual wxString ReadFile(const wxFSFile& file) const;
};

IMPLEMENT_DYNAMIC_CLASS(CHtmlFilterImage, CHtmlFilter)



bool CHtmlFilterImage::CanRead(const wxFSFile& file) const
{
    return (file.GetMimeType().Left(6) == wxT("image/"));
}



wxString CHtmlFilterImage::ReadFile(const wxFSFile& file) const
{
    wxString res = wxT("<HTML><BODY><IMG SRC=\"") + file.GetLocation() + wxT("\"></BODY></HTML>");
    return res;
}




//--------------------------------------------------------------------------------
// CHtmlFilterHTML
//          filter for text/html
//--------------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(CHtmlFilterHTML, CHtmlFilter)

bool CHtmlFilterHTML::CanRead(const wxFSFile& file) const
{
//    return (file.GetMimeType() == "text/html");
// This is true in most case but some page can return:
// "text/html; char-encoding=...."
// So we use Find instead
  return (file.GetMimeType().Find(wxT("text/html")) == 0);
}



wxString CHtmlFilterHTML::ReadFile(const wxFSFile& file) const
{
    wxInputStream *s = file.GetStream();
    wxString doc;

    if (s == NULL)
    {
        wxLogError(_("Cannot open HTML document: %s"), file.GetLocation().c_str());
        return wxEmptyString;
    }

    // NB: We convert input file to wchar_t here in Unicode mode, based on
    //     either Content-Type header or <meta> tags. In ANSI mode, we don't
    //     do it as it is done by CHtmlParser (for this reason, we add <meta>
    //     tag if we used Content-Type header).
#if wxUSE_UNICODE
    int charsetPos;
    if ((charsetPos = file.GetMimeType().Find(_T("; charset="))) != wxNOT_FOUND)
    {
        wxString charset = file.GetMimeType().Mid(charsetPos + 10);
        wxCSConv conv(charset);
        ReadString(doc, s, conv);
    }
    else
    {
        wxString tmpdoc;
        ReadString(tmpdoc, s, wxConvISO8859_1);
        wxString charset = CHtmlParser::ExtractCharsetInformation(tmpdoc);
        if (charset.empty())
            doc = tmpdoc;
        else
        {
            wxCSConv conv(charset);
            doc = wxString(tmpdoc.mb_str(wxConvISO8859_1), conv);
        }
    }
#else // !wxUSE_UNICODE
    ReadString(doc, s, wxConvLibc);
    // add meta tag if we obtained this through http:
    if (!file.GetMimeType().empty())
    {
        wxString hdr;
        wxString mime = file.GetMimeType();
        hdr.Printf(_T("<meta http-equiv=\"Content-Type\" content=\"%s\">"), mime.c_str());
        return hdr+doc;
    }
#endif

    return doc;
}




///// Module:

class CHtmlFilterModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(CHtmlFilterModule)

    public:
        virtual bool OnInit()
        {
            CHtmlWindow::AddFilter(new CHtmlFilterHTML);
            CHtmlWindow::AddFilter(new CHtmlFilterImage);
            return true;
        }
        virtual void OnExit() {}
};

IMPLEMENT_DYNAMIC_CLASS(CHtmlFilterModule, wxModule)

#endif
