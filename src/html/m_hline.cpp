/////////////////////////////////////////////////////////////////////////////
// Name:        m_hline.cpp
// Purpose:     CHtml module for horizontal line (HR tag)
// Author:      Vaclav Slavik
// RCS-ID:      $Id: m_hline.cpp,v 1.25 2004/11/25 08:32:49 VS Exp $
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
    #include "wx/brush.h"
    #include "wx/pen.h"
    #include "wx/dc.h"
#endif

#include "forcelnk.h"
#include "m_templ.h"

#include "CHtmlCell.h"

FORCE_LINK_ME(m_hline)


//-----------------------------------------------------------------------------
// CHtmlLineCell
//-----------------------------------------------------------------------------

class CHtmlLineCell : public CHtmlCell
{
    public:
        CHtmlLineCell(int size, bool shading) : CHtmlCell() {m_Height = size; m_HasShading = shading;}
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                  CHtmlRenderingInfo& info);
        void Layout(int w, int vw)
            { m_Width = w; m_VirtualWidth = vw; CHtmlCell::Layout(w, vw); }

    private:
        // Should we draw 3-D shading or not
      bool m_HasShading;

      DECLARE_NO_COPY_CLASS(CHtmlLineCell)
};


void CHtmlLineCell::Draw(wxDC& dc, int x, int y,
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          CHtmlRenderingInfo& WXUNUSED(info))
{
    wxBrush mybrush(wxT("GREY"), (m_HasShading) ? wxTRANSPARENT : wxSOLID);
    wxPen mypen(wxT("GREY"), 1, wxSOLID);
    dc.SetBrush(mybrush);
    dc.SetPen(mypen);
    dc.DrawRectangle(x + m_PosX, y + m_PosY, m_VirtualWidth, m_Height);
}




//-----------------------------------------------------------------------------
// The list handler:
//-----------------------------------------------------------------------------


TAG_HANDLER_BEGIN(HR, "HR")
    TAG_HANDLER_CONSTR(HR) { }

    TAG_HANDLER_PROC(tag)
    {
        CHtmlContainerCell *c;
        int sz;
        bool HasShading;

        m_WParser->CloseContainer();
        c = m_WParser->OpenContainer();

        c->SetIndent(m_WParser->GetCharHeight(), wxHTML_INDENT_VERTICAL);
        c->SetAlignHor(wxHTML_ALIGN_CENTER);
        c->SetAlign(tag);
        c->SetWidthFloat(tag);
        sz = 1;
        tag.GetParamAsInt(wxT("SIZE"), &sz);
        HasShading = !(tag.HasParam(wxT("NOSHADE")));
        c->InsertCell(new CHtmlLineCell((int)((double)sz * m_WParser->GetPixelScale()), HasShading));

        m_WParser->CloseContainer();
        m_WParser->OpenContainer();

        return false;
    }

TAG_HANDLER_END(HR)





TAGS_MODULE_BEGIN(HLine)

    TAGS_MODULE_ADD(HR)

TAGS_MODULE_END(HLine)

#endif
