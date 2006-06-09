/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.h
// Purpose:     CHtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// RCS-ID:      $Id: htmltag.h,v 1.20 2005/01/05 19:24:43 ABX Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLTAG_H_
#define _WX_HTMLTAG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "htmltag.h"
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/object.h"
#include "wx/arrstr.h"

class WXDLLIMPEXP_CORE wxColour;
class WXDLLIMPEXP_HTML CHtmlEntitiesParser;

//-----------------------------------------------------------------------------
// CHtmlTagsCache
//          - internal wxHTML class, do not use!
//-----------------------------------------------------------------------------

struct CHtmlCacheItem;

class WXDLLIMPEXP_HTML CHtmlTagsCache : public wxObject
{
    DECLARE_DYNAMIC_CLASS(CHtmlTagsCache)

private:
    CHtmlCacheItem *m_Cache;
    int m_CacheSize;
    int m_CachePos;

public:
    CHtmlTagsCache() : wxObject() {m_CacheSize = 0; m_Cache = NULL;}
    CHtmlTagsCache(const wxString& source);
    ~CHtmlTagsCache() {free(m_Cache);}

    // Finds parameters for tag starting at at and fills the variables
    void QueryTag(int at, int* end1, int* end2);

    DECLARE_NO_COPY_CLASS(CHtmlTagsCache)
};


//--------------------------------------------------------------------------------
// CHtmlTag
//                  This represents single tag. It is used as internal structure
//                  by CHtmlParser.
//--------------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlTag : public wxObject
{
    DECLARE_CLASS(CHtmlTag)

protected:
    // constructs CHtmlTag object based on HTML tag.
    // The tag begins (with '<' character) at position pos in source
    // end_pos is position where parsing ends (usually end of document)
    CHtmlTag(CHtmlTag *parent,
              const wxString& source, int pos, int end_pos,
              CHtmlTagsCache *cache,
              CHtmlEntitiesParser *entParser);
    friend class CHtmlParser;
public:
    ~CHtmlTag();

    CHtmlTag *GetParent() const {return m_Parent;}
    CHtmlTag *GetFirstSibling() const;
    CHtmlTag *GetLastSibling() const;
    CHtmlTag *GetChildren() const { return m_FirstChild; }
    CHtmlTag *GetPreviousSibling() const { return m_Prev; }
    CHtmlTag *GetNextSibling() const {return m_Next; }
    // Return next tag, as if tree had been flattened
    CHtmlTag *GetNextTag() const;

    // Returns tag's name in uppercase.
    inline wxString GetName() const {return m_Name;}

    // Returns true if the tag has given parameter. Parameter
    // should always be in uppercase.
    // Example : <IMG SRC="test.jpg"> HasParam("SRC") returns true
    bool HasParam(const wxString& par) const;

    // Returns value of the param. Value is in uppercase unless it is
    // enclosed with "
    // Example : <P align=right> GetParam("ALIGN") returns (RIGHT)
    //           <P IMG SRC="WhaT.jpg"> GetParam("SRC") returns (WhaT.jpg)
    //                           (or ("WhaT.jpg") if with_commas == true)
    wxString GetParam(const wxString& par, bool with_commas = false) const;

    // Convenience functions:
    bool GetParamAsColour(const wxString& par, wxColour *clr) const;
    bool GetParamAsInt(const wxString& par, int *clr) const;

    // Scans param like scanf() functions family does.
    // Example : ScanParam("COLOR", "\"#%X\"", &clr);
    // This is always with with_commas=false
    // Returns number of scanned values
    // (like sscanf() does)
    // NOTE: unlike scanf family, this function only accepts
    //       *one* parameter !
    int ScanParam(const wxString& par, const wxChar *format, void *param) const;

    // Returns string containing all params.
    wxString GetAllParams() const;

#if WXWIN_COMPATIBILITY_2_2
    // return true if this is ending tag (</something>) or false
    // if it isn't (<something>)
    wxDEPRECATED( bool IsEnding() const );
#endif

    // return true if this there is matching ending tag
    inline bool HasEnding() const {return m_End1 >= 0;}

    // returns beginning position of _internal_ block of text
    // See explanation (returned value is marked with *):
    // bla bla bla <MYTAG>* bla bla intenal text</MYTAG> bla bla
    inline int GetBeginPos() const {return m_Begin;}
    // returns ending position of _internal_ block of text.
    // bla bla bla <MYTAG> bla bla intenal text*</MYTAG> bla bla
    inline int GetEndPos1() const {return m_End1;}
    // returns end position 2 :
    // bla bla bla <MYTAG> bla bla internal text</MYTAG>* bla bla
    inline int GetEndPos2() const {return m_End2;}

private:
    wxString m_Name;
    int m_Begin, m_End1, m_End2;
    wxArrayString m_ParamNames, m_ParamValues;

    // DOM tree relations:
    CHtmlTag *m_Next;
    CHtmlTag *m_Prev;
    CHtmlTag *m_FirstChild, *m_LastChild;
    CHtmlTag *m_Parent;

    DECLARE_NO_COPY_CLASS(CHtmlTag)
};





#endif

#endif // _WX_HTMLTAG_H_

