/////////////////////////////////////////////////////////////////////////////
// Name:        m_list.cpp
// Purpose:     CHtml module for lists
// Author:      Vaclav Slavik
// RCS-ID:      $Id: m_list.cpp,v 1.30 2005/09/05 17:37:06 ABX Exp $
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
    #include "wx/dc.h"
#endif

#include "forcelnk.h"
#include "m_templ.h"

#include "CHtmlCell.h"

FORCE_LINK_ME(m_list)


//-----------------------------------------------------------------------------
// CHtmlListmarkCell
//-----------------------------------------------------------------------------

class CHtmlListmarkCell : public CHtmlCell
{
    private:
        wxBrush m_Brush;
    public:
        CHtmlListmarkCell(wxDC *dc, const wxColour& clr);
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                  CHtmlRenderingInfo& info);

    DECLARE_NO_COPY_CLASS(CHtmlListmarkCell)
};

CHtmlListmarkCell::CHtmlListmarkCell(wxDC* dc, const wxColour& clr) : CHtmlCell(), m_Brush(clr, wxSOLID)
{
    m_Width =  dc->GetCharHeight();
    m_Height = dc->GetCharHeight();
    // bottom of list mark is lined with bottom of letters in next cell
    m_Descent = m_Height / 3;
}



void CHtmlListmarkCell::Draw(wxDC& dc, int x, int y,
                              int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                              CHtmlRenderingInfo& WXUNUSED(info))
{
    dc.SetBrush(m_Brush);
    dc.DrawEllipse(x + m_PosX + m_Width / 3, y + m_PosY + m_Height / 3,
                   (m_Width / 3), (m_Width / 3));
}

//-----------------------------------------------------------------------------
// CHtmlListCell
//-----------------------------------------------------------------------------

struct CHtmlListItemStruct
{
    CHtmlContainerCell *mark;
    CHtmlContainerCell *cont;
    int minWidth;
    int maxWidth;
};

class CHtmlListCell : public CHtmlContainerCell
{
    private:
        wxBrush m_Brush;

        int m_NumRows;
        CHtmlListItemStruct *m_RowInfo;
        void ReallocRows(int rows);
        void ComputeMinMaxWidths();
        int ComputeMaxBase(CHtmlCell *cell);
        int m_ListmarkWidth;

    public:
        CHtmlListCell(CHtmlContainerCell *parent);
        virtual ~CHtmlListCell();
        void AddRow(CHtmlContainerCell *mark, CHtmlContainerCell *cont);
        virtual void Layout(int w, int vw);

    DECLARE_NO_COPY_CLASS(CHtmlListCell)
};

CHtmlListCell::CHtmlListCell(CHtmlContainerCell *parent) : CHtmlContainerCell(parent)
{
    m_NumRows = 0;
    m_RowInfo = 0;
    m_ListmarkWidth = 0;
}

CHtmlListCell::~CHtmlListCell()
{
    if (m_RowInfo) free(m_RowInfo);
}

int CHtmlListCell::ComputeMaxBase(CHtmlCell *cell)
{
    if(!cell)
        return 0;

    CHtmlCell *child = cell->GetFirstChild();

    while(child)
    {
        int base = ComputeMaxBase( child );
        if ( base > 0 ) return base + child->GetPosY();
        child = child->GetNext();
    }

    return cell->GetHeight() - cell->GetDescent();
}

void CHtmlListCell::Layout(int w, int vw)
{
    CHtmlCell::Layout(w, vw);

    ComputeMinMaxWidths();
    m_Width = wxMax(m_Width, wxMin(w, GetMaxTotalWidth()));

    int s_width = m_Width - m_IndentLeft;

    int vpos = 0;
    for (int r = 0; r < m_NumRows; r++)
    {
        // do layout first time to layout contents and adjust pos
        m_RowInfo[r].mark->Layout(m_ListmarkWidth, 0);
        m_RowInfo[r].cont->Layout(s_width - m_ListmarkWidth, 0);

        const int base_mark = ComputeMaxBase( m_RowInfo[r].mark );
        const int base_cont = ComputeMaxBase( m_RowInfo[r].cont );
        const int adjust_mark = vpos + wxMax(base_cont-base_mark,0);
        const int adjust_cont = vpos + wxMax(base_mark-base_cont,0);

        m_RowInfo[r].mark->SetPos(m_IndentLeft, adjust_mark);
        m_RowInfo[r].cont->SetPos(m_IndentLeft + m_ListmarkWidth, adjust_cont);

        vpos = wxMax(adjust_mark + m_RowInfo[r].mark->GetHeight(),
                     adjust_cont + m_RowInfo[r].cont->GetHeight());
    }
    m_Height = vpos;
}

void CHtmlListCell::AddRow(CHtmlContainerCell *mark, CHtmlContainerCell *cont)
{
    ReallocRows(++m_NumRows);
    m_RowInfo[m_NumRows - 1].mark = mark;
    m_RowInfo[m_NumRows - 1].cont = cont;
}

void CHtmlListCell::ReallocRows(int rows)
{
    m_RowInfo = (CHtmlListItemStruct*) realloc(m_RowInfo, sizeof(CHtmlListItemStruct) * rows);
    m_RowInfo[rows - 1].mark = NULL;
    m_RowInfo[rows - 1].cont = NULL;
    m_RowInfo[rows - 1].minWidth = 0;
    m_RowInfo[rows - 1].maxWidth = 0;

    m_NumRows = rows;
}

void CHtmlListCell::ComputeMinMaxWidths()
{
    if (m_NumRows == 0) return;

    m_MaxTotalWidth = 0;
    m_Width = 0;

    for (int r = 0; r < m_NumRows; r++)
    {
        CHtmlListItemStruct& row = m_RowInfo[r];
        row.mark->Layout(1, 0);
        row.cont->Layout(1, 0);
        int maxWidth = row.cont->GetMaxTotalWidth();
        int width = row.cont->GetWidth();
        if (row.mark->GetWidth() > m_ListmarkWidth)
            m_ListmarkWidth = row.mark->GetWidth();
        if (maxWidth > m_MaxTotalWidth)
            m_MaxTotalWidth = maxWidth;
        if (width > m_Width)
            m_Width = width;
    }
    m_Width += m_ListmarkWidth + m_IndentLeft;
    m_MaxTotalWidth += m_ListmarkWidth + m_IndentLeft;
}

//-----------------------------------------------------------------------------
// CHtmlListcontentCell
//-----------------------------------------------------------------------------

class CHtmlListcontentCell : public CHtmlContainerCell
{
public:
    CHtmlListcontentCell(CHtmlContainerCell *p) : CHtmlContainerCell(p) {}
    virtual void Layout(int w, int vw) {
        // Reset top indentation, fixes <li><p>
        SetIndent(0, wxHTML_INDENT_TOP);
        CHtmlContainerCell::Layout(w, vw);
    }
};

//-----------------------------------------------------------------------------
// The list handler:
//-----------------------------------------------------------------------------


TAG_HANDLER_BEGIN(OLULLI, "OL,UL,LI")

    TAG_HANDLER_VARS
        CHtmlListCell *m_List;
        int m_Numbering;
                // this is number of actual item of list or 0 for dots

    TAG_HANDLER_CONSTR(OLULLI)
    {
        m_List = NULL;
        m_Numbering = 0;
    }

    TAG_HANDLER_PROC(tag)
    {
        CHtmlContainerCell *c;

        // List Item:
        if (m_List && tag.GetName() == wxT("LI"))
        {
            c = m_WParser->SetContainer(new CHtmlContainerCell(m_List));
            c->SetAlignVer(wxHTML_ALIGN_TOP);

            CHtmlContainerCell *mark = c;
            c->SetWidthFloat(2 * m_WParser->GetCharWidth(), wxHTML_UNITS_PIXELS);
            if (m_Numbering == 0)
            {
                // Centering gives more space after the bullet
                c->SetAlignHor(wxHTML_ALIGN_CENTER);
                c->InsertCell(new CHtmlListmarkCell(m_WParser->GetDC(), m_WParser->GetActualColor()));
            }
            else
            {
                c->SetAlignHor(wxHTML_ALIGN_RIGHT);
                wxString markStr;
                markStr.Printf(wxT("%i."), m_Numbering);
                c->InsertCell(new CHtmlWordCell(markStr, *(m_WParser->GetDC())));
            }
            m_WParser->CloseContainer();

            c = m_WParser->OpenContainer();

            m_List->AddRow(mark, c);
            c = m_WParser->OpenContainer();
            m_WParser->SetContainer(new CHtmlListcontentCell(c));

            if (m_Numbering != 0) m_Numbering++;
        }

        // Begin of List (not-numbered): "UL", "OL"
        else if (tag.GetName() == wxT("UL") || tag.GetName() == wxT("OL"))
        {
            int oldnum = m_Numbering;

            if (tag.GetName() == wxT("UL")) m_Numbering = 0;
            else m_Numbering = 1;

            CHtmlContainerCell *oldcont;
            oldcont = c = m_WParser->OpenContainer();

            CHtmlListCell *oldList = m_List;
            m_List = new CHtmlListCell(c);
            m_List->SetIndent(2 * m_WParser->GetCharWidth(), wxHTML_INDENT_LEFT);

            ParseInner(tag);

            m_WParser->SetContainer(oldcont);
            m_WParser->CloseContainer();

            m_Numbering = oldnum;
            m_List = oldList;
            return true;
        }
        return false;

    }

TAG_HANDLER_END(OLULLI)


TAGS_MODULE_BEGIN(List)

    TAGS_MODULE_ADD(OLULLI)

TAGS_MODULE_END(List)

#endif
