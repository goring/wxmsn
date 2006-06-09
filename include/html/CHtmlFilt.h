/////////////////////////////////////////////////////////////////////////////
// Name:        CHtmlFilt.h
// Purpose:     filters
// Author:      Vaclav Slavik
// RCS-ID:      $Id: CHtmlFilt.h,v 1.10 2004/09/27 19:06:39 ABX Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLFILT_H_
#define _WX_HTMLFILT_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CHtmlFilt.h"
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/filesys.h"


//--------------------------------------------------------------------------------
// CHtmlFilter
//                  This class is input filter. It can "translate" files
//                  in non-HTML format to HTML format
//                  interface to access certain
//                  kinds of files (HTPP, FTP, local, tar.gz etc..)
//--------------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlFilter : public wxObject
{
    DECLARE_ABSTRACT_CLASS(CHtmlFilter)

public:
    CHtmlFilter() : wxObject() {}
    virtual ~CHtmlFilter() {}

    // returns true if this filter is able to open&read given file
    virtual bool CanRead(const wxFSFile& file) const = 0;

    // Reads given file and returns HTML document.
    // Returns empty string if opening failed
    virtual wxString ReadFile(const wxFSFile& file) const = 0;
};



//--------------------------------------------------------------------------------
// CHtmlFilterPlainText
//                  This filter is used as default filter if no other can
//                  be used (= uknown type of file). It is used by
//                  CHtmlWindow itself.
//--------------------------------------------------------------------------------


class WXDLLIMPEXP_HTML CHtmlFilterPlainText : public CHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(CHtmlFilterPlainText)

public:
    virtual bool CanRead(const wxFSFile& file) const;
    virtual wxString ReadFile(const wxFSFile& file) const;
};

//--------------------------------------------------------------------------------
// CHtmlFilterHTML
//          filter for text/html
//--------------------------------------------------------------------------------

class CHtmlFilterHTML : public CHtmlFilter
{
    DECLARE_DYNAMIC_CLASS(CHtmlFilterHTML)

    public:
        virtual bool CanRead(const wxFSFile& file) const;
        virtual wxString ReadFile(const wxFSFile& file) const;
};



#endif // wxUSE_HTML

#endif // _WX_HTMLFILT_H_

