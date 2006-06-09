/////////////////////////////////////////////////////////////////////////////
// Name:        m_links.cpp
// Purpose:     CHtml module for links & anchors
// Author:      Vaclav Slavik
// RCS-ID:      $Id: m_links.cpp,v 1.16 2004/09/27 19:15:06 ABX Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#endif

#include "forcelnk.h"
#include "m_templ.h"


FORCE_LINK_ME(m_links)


class CHtmlAnchorCell : public CHtmlCell
{
private:
    wxString m_AnchorName;

public:
    CHtmlAnchorCell(const wxString& name) : CHtmlCell()
        { m_AnchorName = name; }
    void Draw(wxDC& WXUNUSED(dc),
              int WXUNUSED(x), int WXUNUSED(y),
              int WXUNUSED(view_y1), int WXUNUSED(view_y2),
              CHtmlRenderingInfo& WXUNUSED(info)) {}

    virtual const CHtmlCell* Find(int condition, const void* param) const
    {
        if ((condition == wxHTML_COND_ISANCHOR) &&
            (m_AnchorName == (*((const wxString*)param))))
        {
            return this;
        }
        else
        {
            return CHtmlCell::Find(condition, param);
        }
    }

    DECLARE_NO_COPY_CLASS(CHtmlAnchorCell)
};



TAG_HANDLER_BEGIN(A, "A")
    TAG_HANDLER_CONSTR(A) { }

    TAG_HANDLER_PROC(tag)
    {
        if (tag.HasParam( wxT("NAME") ))
        {
            m_WParser->GetContainer()->InsertCell(new CHtmlAnchorCell(tag.GetParam( wxT("NAME") )));
        }

        if (tag.HasParam( wxT("HREF") ))
        {
            CHtmlLinkInfo oldlnk = m_WParser->GetLink();
            wxColour oldclr = m_WParser->GetActualColor();
            int oldund = m_WParser->GetFontUnderlined();
            wxString name(tag.GetParam( wxT("HREF") )), target;

            if (tag.HasParam( wxT("TARGET") )) target = tag.GetParam( wxT("TARGET") );
            m_WParser->SetActualColor(m_WParser->GetLinkColor());
            m_WParser->GetContainer()->InsertCell(new CHtmlColourCell(m_WParser->GetLinkColor()));
            m_WParser->SetFontUnderlined(true);
            m_WParser->GetContainer()->InsertCell(new CHtmlFontCell(m_WParser->CreateCurrentFont()));
            m_WParser->SetLink(CHtmlLinkInfo(name, target));

            ParseInner(tag);

            m_WParser->SetLink(oldlnk);
            m_WParser->SetFontUnderlined(oldund);
            m_WParser->GetContainer()->InsertCell(new CHtmlFontCell(m_WParser->CreateCurrentFont()));
            m_WParser->SetActualColor(oldclr);
            m_WParser->GetContainer()->InsertCell(new CHtmlColourCell(oldclr));

            return true;
        }
        else return false;
    }

TAG_HANDLER_END(A)



TAGS_MODULE_BEGIN(Links)

    TAGS_MODULE_ADD(A)

TAGS_MODULE_END(Links)


#endif
