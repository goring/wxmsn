/////////////////////////////////////////////////////////////////////////////
// Name:        htmlcell.cpp
// Purpose:     CHtmlCell - basic element of HTML outputl
// Author:      Vaclav Slavik
// RCS-ID:      $Id: htmlcell.cpp,v 1.102 2005/09/11 12:09:35 VZ Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CHtmlCell.h"
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
    #include "wx/brush.h"
    #include "wx/colour.h"
    #include "wx/dc.h"
#endif

#include "CHtmlCell.h"
#include "CHtmlWin.h"
#include "wx/settings.h"
#include "wx/module.h"
#include "wx/dynarray.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static wxCursor *gs_cursorLink = NULL;
static wxCursor *gs_cursorText = NULL;


//-----------------------------------------------------------------------------
// Helper classes
//-----------------------------------------------------------------------------

void CHtmlSelection::Set(const wxPoint& fromPos, const CHtmlCell *fromCell,
                          const wxPoint& toPos, const CHtmlCell *toCell)
{
    m_fromCell = fromCell;
    m_toCell = toCell;
    m_fromPos = fromPos;
    m_toPos = toPos;
}

void CHtmlSelection::Set(const CHtmlCell *fromCell, const CHtmlCell *toCell)
{
    wxPoint p1 = fromCell ? fromCell->GetAbsPos() : wxDefaultPosition;
    wxPoint p2 = toCell ? toCell->GetAbsPos() : wxDefaultPosition;
    if ( toCell )
    {
        p2.x += toCell->GetWidth();
        p2.y += toCell->GetHeight();
    }
    Set(p1, fromCell, p2, toCell);
}

wxColour
CDefaultHtmlRenderingStyle::
GetSelectedTextColour(const wxColour& WXUNUSED(clr))
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}

wxColour
CDefaultHtmlRenderingStyle::
GetSelectedTextBgColour(const wxColour& WXUNUSED(clr))
{
    return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
}


//-----------------------------------------------------------------------------
// CHtmlCell
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlCell, wxObject)

CHtmlCell::CHtmlCell() : wxObject()
{
    m_Next = NULL;
    m_Parent = NULL;
    m_Width = m_Height = m_Descent = 0;
    m_CanLiveOnPagebreak = true;
    m_Link = NULL;
}

CHtmlCell::~CHtmlCell()
{
    delete m_Link;
}


void CHtmlCell::OnMouseClick(wxWindow *parent, int x, int y,
                              const wxMouseEvent& event)
{
    CHtmlLinkInfo *lnk = GetLink(x, y);
    if (lnk != NULL)
    {
        CHtmlLinkInfo lnk2(*lnk);
        lnk2.SetEvent(&event);
        lnk2.SetHtmlCell(this);

        // note : this cast is legal because parent is *always* CHtmlWindow
        wxStaticCast(parent, CHtmlWindow)->OnLinkClicked(lnk2);
    }
}


wxCursor CHtmlCell::GetCursor() const
{
    if ( GetLink() )
    {
        if ( !gs_cursorLink )
            gs_cursorLink = new wxCursor(wxCURSOR_HAND);
        return *gs_cursorLink;
    }
    else
        return *wxSTANDARD_CURSOR;
}


bool CHtmlCell::AdjustPagebreak(int *pagebreak, int* WXUNUSED(known_pagebreaks), int WXUNUSED(number_of_pages)) const
{
    if ((!m_CanLiveOnPagebreak) &&
                m_PosY < *pagebreak && m_PosY + m_Height > *pagebreak)
    {
        *pagebreak = m_PosY;
        return true;
    }

    return false;
}



void CHtmlCell::SetLink(const CHtmlLinkInfo& link)
{
    if (m_Link) delete m_Link;
    m_Link = NULL;
    if (link.GetHref() != wxEmptyString)
        m_Link = new CHtmlLinkInfo(link);
}


void CHtmlCell::Layout(int WXUNUSED(w), int WXUNUSED(vw))
{
    SetPos(0, 0);
}



const CHtmlCell* CHtmlCell::Find(int WXUNUSED(condition), const void* WXUNUSED(param)) const
{
    return NULL;
}


CHtmlCell *CHtmlCell::FindCellByPos(wxCoord x, wxCoord y,
                                      unsigned flags) const
{
    if ( x >= 0 && x < m_Width && y >= 0 && y < m_Height )
    {
        return wxConstCast(this, CHtmlCell);
    }
    else
    {
        if ((flags & wxHTML_FIND_NEAREST_AFTER) &&
                (y < 0 || (y < 0+m_Height && x < 0+m_Width)))
            return wxConstCast(this, CHtmlCell);
        else if ((flags & wxHTML_FIND_NEAREST_BEFORE) &&
                (y >= 0+m_Height || (y >= 0 && x >= 0)))
            return wxConstCast(this, CHtmlCell);
        else
            return NULL;
    }
}


wxPoint CHtmlCell::GetAbsPos() const
{
    wxPoint p(m_PosX, m_PosY);
    for (CHtmlCell *parent = m_Parent; parent; parent = parent->m_Parent)
    {
        p.x += parent->m_PosX;
        p.y += parent->m_PosY;
    }
    return p;
}

unsigned CHtmlCell::GetDepth() const
{
    unsigned d = 0;
    for (CHtmlCell *p = m_Parent; p; p = p->m_Parent)
        d++;
    return d;
}

bool CHtmlCell::IsBefore(CHtmlCell *cell) const
{
    const CHtmlCell *c1 = this;
    const CHtmlCell *c2 = cell;
    unsigned d1 = GetDepth();
    unsigned d2 = cell->GetDepth();

    if ( d1 > d2 )
        for (; d1 != d2; d1-- )
            c1 = c1->m_Parent;
    else if ( d1 < d2 )
        for (; d1 != d2; d2-- )
            c2 = c2->m_Parent;

    if ( cell == this )
        return true;

    while ( c1 && c2 )
    {
        if ( c1->m_Parent == c2->m_Parent )
        {
            while ( c1 )
            {
                if ( c1 == c2 )
                    return true;
                c1 = c1->GetNext();
            }
            return false;
        }
        else
        {
            c1 = c1->m_Parent;
            c2 = c2->m_Parent;
        }
    }

    wxFAIL_MSG(_T("Cells are in different trees"));
    return false;
}


//-----------------------------------------------------------------------------
// CHtmlWordCell
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlWordCell, CHtmlCell)

CHtmlWordCell::CHtmlWordCell(const wxString& word, wxDC& dc) : CHtmlCell()
{
    m_Word = word;
    dc.GetTextExtent(m_Word, &m_Width, &m_Height, &m_Descent);
    SetCanLiveOnPagebreak(false);
    m_allowLinebreak = true;
}

void CHtmlWordCell::SetPreviousWord(CHtmlWordCell *cell)
{
    if ( cell && m_Parent == cell->m_Parent &&
         !wxIsspace(cell->m_Word.Last()) && !wxIsspace(m_Word[0u]) )
    {
        m_allowLinebreak = false;
    }
}

// Splits m_Word into up to three parts according to selection, returns
// substring before, in and after selection and the points (in relative coords)
// where s2 and s3 start:
void CHtmlWordCell::Split(wxDC& dc,
                           const wxPoint& selFrom, const wxPoint& selTo,
                           unsigned& pos1, unsigned& pos2) const
{
    wxPoint pt1 = (selFrom == wxDefaultPosition) ?
                   wxDefaultPosition : selFrom - GetAbsPos();
    wxPoint pt2 = (selTo == wxDefaultPosition) ?
                   wxPoint(m_Width, wxDefaultCoord) : selTo - GetAbsPos();

    unsigned len = m_Word.length();
    unsigned i = 0;
    pos1 = 0;

    // adjust for cases when the start/end position is completely
    // outside the cell:
    if ( pt1.y < 0 )
        pt1.x = 0;
    if ( pt2.y >= m_Height )
        pt2.x = m_Width;

    // before selection:
#ifdef __WXMAC__
    // implementation using PartialExtents to support fractional widths
    wxArrayInt widths ;
    dc.GetPartialTextExtents(m_Word,widths) ;
    while( i < len && pt1.x >= widths[i] )
        i++ ;
#else // __WXMAC__
    wxCoord charW, charH;
    while ( pt1.x > 0 && i < len )
    {
        dc.GetTextExtent(m_Word[i], &charW, &charH);
        pt1.x -= charW;
        if ( pt1.x >= 0 )
        {
            pos1 += charW;
            i++;
        }
    }
#endif // __WXMAC__/!__WXMAC__

    // in selection:
    unsigned j = i;
#ifdef __WXMAC__
    while( j < len && pt2.x >= widths[j] )
        j++ ;
#else // __WXMAC__
    pos2 = pos1;
    pt2.x -= pos2;
    while ( pt2.x > 0 && j < len )
    {
        dc.GetTextExtent(m_Word[j], &charW, &charH);
        pt2.x -= charW;
        if ( pt2.x >= 0 )
        {
            pos2 += charW;
            j++;
        }
    }
#endif // __WXMAC__/!__WXMAC__

    pos1 = i;
    pos2 = j;
}

void CHtmlWordCell::SetSelectionPrivPos(wxDC& dc, CHtmlSelection *s) const
{
    unsigned p1, p2;

    Split(dc,
          this == s->GetFromCell() ? s->GetFromPos() : wxDefaultPosition,
          this == s->GetToCell() ? s->GetToPos() : wxDefaultPosition,
          p1, p2);

    wxPoint p(0, m_Word.length());

    if ( this == s->GetFromCell() )
        p.x = p1; // selection starts here
    if ( this == s->GetToCell() )
        p.y = p2; // selection ends here

    if ( this == s->GetFromCell() )
        s->SetFromPrivPos(p);
    if ( this == s->GetToCell() )
        s->SetToPrivPos(p);
}


static void SwitchSelState(wxDC& dc, CHtmlRenderingInfo& info,
                           bool toSelection)
{
    wxColour fg = info.GetState().GetFgColour();
    wxColour bg = info.GetState().GetBgColour();

    if ( toSelection )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextForeground(info.GetStyle().GetSelectedTextColour(fg));
        dc.SetTextBackground(info.GetStyle().GetSelectedTextBgColour(bg));
        dc.SetBackground(wxBrush(info.GetStyle().GetSelectedTextBgColour(bg),
                                 wxSOLID));
    }
    else
    {
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.SetTextForeground(fg);
        dc.SetTextBackground(bg);
        dc.SetBackground(wxBrush(bg, wxSOLID));
    }
}


void CHtmlWordCell::Draw(wxDC& dc, int x, int y,
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          CHtmlRenderingInfo& info)
{
#if 0 // useful for debugging
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(x+m_PosX,y+m_PosY,m_Width /* VZ: +1? */ ,m_Height);
#endif

    bool drawSelectionAfterCell = false;

    if ( info.GetState().GetSelectionState() == wxHTML_SEL_CHANGING )
    {
        // Selection changing, we must draw the word piecewise:
        CHtmlSelection *s = info.GetSelection();
        wxString txt;
        int w, h;
        int ofs = 0;

        wxPoint priv = (this == s->GetFromCell()) ?
                           s->GetFromPrivPos() : s->GetToPrivPos();

        // NB: this is quite a hack: in order to compute selection boundaries
        //     (in word's characters) we must know current font, which is only
        //     possible inside rendering code. Therefore we update the
        //     information here and store it in CHtmlSelection so that
        //     ConvertToText can use it later:
        if ( priv == wxDefaultPosition )
        {
            SetSelectionPrivPos(dc, s);
            priv = (this == s->GetFromCell()) ?
                    s->GetFromPrivPos() : s->GetToPrivPos();
        }

        int part1 = priv.x;
        int part2 = priv.y;

        if ( part1 > 0 )
        {
            txt = m_Word.Mid(0, part1);
            dc.DrawText(txt, x + m_PosX, y + m_PosY);
            dc.GetTextExtent(txt, &w, &h);
            ofs += w;
        }

        SwitchSelState(dc, info, true);

        txt = m_Word.Mid(part1, part2-part1);
        dc.DrawText(txt, ofs + x + m_PosX, y + m_PosY);

        if ( (size_t)part2 < m_Word.length() )
        {
            dc.GetTextExtent(txt, &w, &h);
            ofs += w;
            SwitchSelState(dc, info, false);
            txt = m_Word.Mid(part2);
            dc.DrawText(txt, ofs + x + m_PosX, y + m_PosY);
        }
        else
            drawSelectionAfterCell = true;
    }
    else
    {
        CHtmlSelectionState selstate = info.GetState().GetSelectionState();
        // Not changing selection state, draw the word in single mode:
        if ( selstate != wxHTML_SEL_OUT &&
             dc.GetBackgroundMode() != wxSOLID )
        {
            SwitchSelState(dc, info, true);
        }
        else if ( selstate == wxHTML_SEL_OUT &&
                  dc.GetBackgroundMode() == wxSOLID )
        {
            SwitchSelState(dc, info, false);
        }
        dc.DrawText(m_Word, x + m_PosX, y + m_PosY);
        drawSelectionAfterCell = (selstate != wxHTML_SEL_OUT);
    }

    // NB: If the text is justified then there is usually some free space
    //     between adjacent cells and drawing the selection only onto cells
    //     would result in ugly unselected spaces. The code below detects
    //     this special case and renders the selection *outside* the sell,
    //     too.
    if ( m_Parent->GetAlignHor() == wxHTML_ALIGN_JUSTIFY &&
         drawSelectionAfterCell )
    {
        CHtmlCell *nextCell = m_Next;
        while ( nextCell && nextCell->IsFormattingCell() )
            nextCell = nextCell->GetNext();
        if ( nextCell )
        {
            int nextX = nextCell->GetPosX();
            if ( m_PosX + m_Width < nextX )
            {
                dc.SetBrush(dc.GetBackground());
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(x + m_PosX + m_Width, y + m_PosY,
                                 nextX - m_PosX - m_Width, m_Height);
            }
        }
    }
}


wxString CHtmlWordCell::ConvertToText(CHtmlSelection *s) const
{
    if ( s && (this == s->GetFromCell() || this == s->GetToCell()) )
    {
        wxPoint priv = this == s->GetFromCell() ? s->GetFromPrivPos()
                                                : s->GetToPrivPos();

        // VZ: we may be called before we had a chance to re-render ourselves
        //     and in this case GetFrom/ToPrivPos() is not set yet -- assume
        //     that this only happens in case of a double/triple click (which
        //     seems to be the case now) and so it makes sense to select the
        //     entire contents of the cell in this case
        //
        // TODO: but this really needs to be fixed in some better way later...
        if ( priv != wxDefaultPosition )
        {
            int part1 = priv.x;
            int part2 = priv.y;
            return m_Word.Mid(part1, part2-part1);
        }
        //else: return the whole word below
    }

    return m_Word;
}

wxCursor CHtmlWordCell::GetCursor() const
{
    if ( !GetLink() )
    {
        if ( !gs_cursorText )
            gs_cursorText = new wxCursor(wxCURSOR_IBEAM);
        return *gs_cursorText;
    }
    else
        return CHtmlCell::GetCursor();
}


//-----------------------------------------------------------------------------
// CHtmlContainerCell
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlContainerCell, CHtmlCell)

CHtmlContainerCell::CHtmlContainerCell(CHtmlContainerCell *parent) : CHtmlCell()
{
    m_Cells = m_LastCell = NULL;
    m_Parent = parent;
    m_MaxTotalWidth = 0;
    if (m_Parent) m_Parent->InsertCell(this);
    m_AlignHor = wxHTML_ALIGN_LEFT;
    m_AlignVer = wxHTML_ALIGN_BOTTOM;
    m_IndentLeft = m_IndentRight = m_IndentTop = m_IndentBottom = 0;
    m_WidthFloat = 100; m_WidthFloatUnits = wxHTML_UNITS_PERCENT;
    m_UseBkColour = false;
    m_UseBorder = false;
    m_MinHeight = 0;
    m_MinHeightAlign = wxHTML_ALIGN_TOP;
    m_LastLayout = -1;
}

CHtmlContainerCell::~CHtmlContainerCell()
{
    CHtmlCell *cell = m_Cells;
    while ( cell )
    {
        CHtmlCell *cellNext = cell->GetNext();
        delete cell;
        cell = cellNext;
    }
}



void CHtmlContainerCell::SetIndent(int i, int what, int units)
{
    int val = (units == wxHTML_UNITS_PIXELS) ? i : -i;
    if (what & wxHTML_INDENT_LEFT) m_IndentLeft = val;
    if (what & wxHTML_INDENT_RIGHT) m_IndentRight = val;
    if (what & wxHTML_INDENT_TOP) m_IndentTop = val;
    if (what & wxHTML_INDENT_BOTTOM) m_IndentBottom = val;
    m_LastLayout = -1;
}



int CHtmlContainerCell::GetIndent(int ind) const
{
    if (ind & wxHTML_INDENT_LEFT) return m_IndentLeft;
    else if (ind & wxHTML_INDENT_RIGHT) return m_IndentRight;
    else if (ind & wxHTML_INDENT_TOP) return m_IndentTop;
    else if (ind & wxHTML_INDENT_BOTTOM) return m_IndentBottom;
    else return -1; /* BUG! Should not be called... */
}




int CHtmlContainerCell::GetIndentUnits(int ind) const
{
    bool p = false;
    if (ind & wxHTML_INDENT_LEFT) p = m_IndentLeft < 0;
    else if (ind & wxHTML_INDENT_RIGHT) p = m_IndentRight < 0;
    else if (ind & wxHTML_INDENT_TOP) p = m_IndentTop < 0;
    else if (ind & wxHTML_INDENT_BOTTOM) p = m_IndentBottom < 0;
    if (p) return wxHTML_UNITS_PERCENT;
    else return wxHTML_UNITS_PIXELS;
}



bool CHtmlContainerCell::AdjustPagebreak(int *pagebreak, int* known_pagebreaks, int number_of_pages) const
{
    if (!m_CanLiveOnPagebreak)
        return CHtmlCell::AdjustPagebreak(pagebreak, known_pagebreaks, number_of_pages);

    else
    {
        CHtmlCell *c = GetFirstChild();
        bool rt = false;
        int pbrk = *pagebreak - m_PosY;

        while (c)
        {
            if (c->AdjustPagebreak(&pbrk, known_pagebreaks, number_of_pages))
                rt = true;
            c = c->GetNext();
        }
        if (rt)
            *pagebreak = pbrk + m_PosY;
        return rt;
    }
}



void CHtmlContainerCell::Layout(int w, int vw)
{
    CHtmlCell::Layout(w, vw);

    if (m_LastLayout == w) return;

    // VS: Any attempt to layout with negative or zero width leads to hell,
    // but we can't ignore such attempts completely, since it sometimes
    // happen (e.g. when trying how small a table can be). The best thing we
    // can do is to set the width of child cells to zero
    if (w < 1)
    {
       m_Width = 0;
       for (CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            cell->Layout(0, 0);
            // this does two things: it recursively calls this code on all
            // child contrainers and resets children's position to (0,0)
       return;
    }

    CHtmlCell *cell = m_Cells,
               *line = m_Cells;
    CHtmlCell *nextCell;
    long xpos = 0, ypos = m_IndentTop;
    int xdelta = 0, ybasicpos = 0, ydiff;
    int s_width, nextWordWidth, s_indent;
    int ysizeup = 0, ysizedown = 0;
    int MaxLineWidth = 0;
    int curLineWidth = 0;
    m_MaxTotalWidth = 0;


    /*

    WIDTH ADJUSTING :

    */

    if (m_WidthFloatUnits == wxHTML_UNITS_PERCENT)
    {
        if (m_WidthFloat < 0) m_Width = (100 + m_WidthFloat) * w / 100;
        else m_Width = m_WidthFloat * w / 100;
    }
    else
    {
        if (m_WidthFloat < 0) m_Width = w + m_WidthFloat;
        else m_Width = m_WidthFloat;
    }

    if (m_Cells)
    {
        int l = (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
        int r = (m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight;
        for (CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
            cell->Layout(m_Width - (l + r), vw - (l + r));
    }

    /*

    LAYOUTING :

    */

    // adjust indentation:
    s_indent = (m_IndentLeft < 0) ? (-m_IndentLeft * m_Width / 100) : m_IndentLeft;
    s_width = m_Width - s_indent - ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);

    // my own layouting:
    while (cell != NULL)
    {
        switch (m_AlignVer)
        {
            case wxHTML_ALIGN_TOP :      ybasicpos = 0; break;
            case wxHTML_ALIGN_BOTTOM :   ybasicpos = - cell->GetHeight(); break;
            case wxHTML_ALIGN_CENTER :   ybasicpos = - cell->GetHeight() / 2; break;
        }
        ydiff = cell->GetHeight() + ybasicpos;

        if (cell->GetDescent() + ydiff > ysizedown) ysizedown = cell->GetDescent() + ydiff;
        if (ybasicpos + cell->GetDescent() < -ysizeup) ysizeup = - (ybasicpos + cell->GetDescent());

        // layout nonbreakable run of cells:
        cell->SetPos(xpos, ybasicpos + cell->GetDescent());
        xpos += cell->GetWidth();
        if (!cell->IsTerminalCell())
        {
            // Container cell indicates new line
            if (curLineWidth > m_MaxTotalWidth)
                m_MaxTotalWidth = curLineWidth;

            if (wxMax(cell->GetWidth(), cell->GetMaxTotalWidth()) > m_MaxTotalWidth)
                m_MaxTotalWidth = cell->GetMaxTotalWidth();
            curLineWidth = 0;
        }
        else
            // Normal cell, add maximum cell width to line width
            curLineWidth += cell->GetMaxTotalWidth();

        cell = cell->GetNext();

        // compute length of the next word that would be added:
        nextWordWidth = 0;
        if (cell)
        {
            nextCell = cell;
            do
            {
                nextWordWidth += nextCell->GetWidth();
                nextCell = nextCell->GetNext();
            } while (nextCell && !nextCell->IsLinebreakAllowed());
        }

        // force new line if occurred:
        if ((cell == NULL) ||
            (xpos + nextWordWidth > s_width && cell->IsLinebreakAllowed()))
        {
            if (xpos > MaxLineWidth) MaxLineWidth = xpos;
            if (ysizeup < 0) ysizeup = 0;
            if (ysizedown < 0) ysizedown = 0;
            switch (m_AlignHor) {
                case wxHTML_ALIGN_LEFT :
                case wxHTML_ALIGN_JUSTIFY :
                         xdelta = 0;
                         break;
                case wxHTML_ALIGN_RIGHT :
                         xdelta = 0 + (s_width - xpos);
                         break;
                case wxHTML_ALIGN_CENTER :
                         xdelta = 0 + (s_width - xpos) / 2;
                         break;
            }
            if (xdelta < 0) xdelta = 0;
            xdelta += s_indent;

            ypos += ysizeup;

            if (m_AlignHor != wxHTML_ALIGN_JUSTIFY || cell == NULL)
            {
                while (line != cell)
                {
                    line->SetPos(line->GetPosX() + xdelta,
                                   ypos + line->GetPosY());
                    line = line->GetNext();
                }
            }
            else // align == justify
            {
                // we have to distribute the extra horz space between the cells
                // on this line

                // an added complication is that some cells have fixed size and
                // shouldn't get any increment (it so happens that these cells
                // also don't allow line break on them which provides with an
                // easy way to test for this) -- and neither should the cells
                // adjacent to them as this could result in a visible space
                // between two cells separated by, e.g. font change, cell which
                // is wrong

                int step = s_width - xpos;
                if ( step > 0 )
                {
                    // first count the cells which will get extra space
                    int total = 0;

                    const CHtmlCell *c;
                    if ( line != cell )
                    {
                        for ( c = line->GetNext(); c != cell; c = c->GetNext() )
                        {
                            if ( c->IsLinebreakAllowed() )
                            {
                                total++;
                            }
                        }
                    }

                    // and now extra space to those cells which merit it
                    if ( total )
                    {
                        // first cell on line is not moved:
                        line->SetPos(line->GetPosX() + s_indent,
                                     line->GetPosY() + ypos);

                        line = line->GetNext();
                        for ( int n = 0; line != cell; line = line->GetNext() )
                        {
                            if ( line->IsLinebreakAllowed() )
                            {
                                // offset the next cell relative to this one
                                // thus increasing our size
                                n++;
                            }

                            line->SetPos(line->GetPosX() + s_indent +
                                           ((n * step) / total),
                                           line->GetPosY() + ypos);
                        }
                    }
                    else
                    {
                        // this will cause the code to enter "else branch" below:
                        step = 0;
                    }
                }
                // else branch:
                if ( step <= 0 ) // no extra space to distribute
                {
                    // just set the indent properly
                    while (line != cell)
                    {
                        line->SetPos(line->GetPosX() + s_indent,
                                     line->GetPosY() + ypos);
                        line = line->GetNext();
                    }
                }
            }

            ypos += ysizedown;
            xpos = 0;
            ysizeup = ysizedown = 0;
            line = cell;
        }
    }

    // setup height & width, depending on container layout:
    m_Height = ypos + (ysizedown + ysizeup) + m_IndentBottom;

    if (m_Height < m_MinHeight)
    {
        if (m_MinHeightAlign != wxHTML_ALIGN_TOP)
        {
            int diff = m_MinHeight - m_Height;
            if (m_MinHeightAlign == wxHTML_ALIGN_CENTER) diff /= 2;
            cell = m_Cells;
            while (cell)
            {
                cell->SetPos(cell->GetPosX(), cell->GetPosY() + diff);
                cell = cell->GetNext();
            }
        }
        m_Height = m_MinHeight;
    }

    if (curLineWidth > m_MaxTotalWidth)
        m_MaxTotalWidth = curLineWidth;

    m_MaxTotalWidth += s_indent + ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);
    MaxLineWidth += s_indent + ((m_IndentRight < 0) ? (-m_IndentRight * m_Width / 100) : m_IndentRight);
    if (m_Width < MaxLineWidth) m_Width = MaxLineWidth;

    m_LastLayout = w;
}

void CHtmlContainerCell::UpdateRenderingStatePre(CHtmlRenderingInfo& info,
                                                  CHtmlCell *cell) const
{
    CHtmlSelection *s = info.GetSelection();
    if (!s) return;
    if (s->GetFromCell() == cell || s->GetToCell() == cell)
    {
        info.GetState().SetSelectionState(wxHTML_SEL_CHANGING);
    }
}

void CHtmlContainerCell::UpdateRenderingStatePost(CHtmlRenderingInfo& info,
                                                   CHtmlCell *cell) const
{
    CHtmlSelection *s = info.GetSelection();
    if (!s) return;
    if (s->GetToCell() == cell)
        info.GetState().SetSelectionState(wxHTML_SEL_OUT);
    else if (s->GetFromCell() == cell)
        info.GetState().SetSelectionState(wxHTML_SEL_IN);
}

#define mMin(a, b) (((a) < (b)) ? (a) : (b))
#define mMax(a, b) (((a) < (b)) ? (b) : (a))

void CHtmlContainerCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                               CHtmlRenderingInfo& info)
{
#if 0 // useful for debugging
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(x+m_PosX,y+m_PosY,m_Width,m_Height);
#endif

    int xlocal = x + m_PosX;
    int ylocal = y + m_PosY;

    if (m_UseBkColour)
    {
        wxBrush myb = wxBrush(m_BkColour, wxSOLID);

        int real_y1 = mMax(ylocal, view_y1);
        int real_y2 = mMin(ylocal + m_Height - 1, view_y2);

        dc.SetBrush(myb);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(xlocal, real_y1, m_Width, real_y2 - real_y1 + 1);
    }

    if (m_UseBorder)
    {
        wxPen mypen1(m_BorderColour1, 1, wxSOLID);
        wxPen mypen2(m_BorderColour2, 1, wxSOLID);

        dc.SetPen(mypen1);
        dc.DrawLine(xlocal, ylocal, xlocal, ylocal + m_Height - 1);
        dc.DrawLine(xlocal, ylocal, xlocal + m_Width, ylocal);
        dc.SetPen(mypen2);
        dc.DrawLine(xlocal + m_Width - 1, ylocal, xlocal +  m_Width - 1, ylocal + m_Height - 1);
        dc.DrawLine(xlocal, ylocal + m_Height - 1, xlocal + m_Width, ylocal + m_Height - 1);
    }

    if (m_Cells)
    {
        // draw container's contents:
        for (CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {

            // optimize drawing: don't render off-screen content:
            if ((ylocal + cell->GetPosY() <= view_y2) &&
                (ylocal + cell->GetPosY() + cell->GetHeight() > view_y1))
            {
                // the cell is visible, draw it:
                UpdateRenderingStatePre(info, cell);
                cell->Draw(dc,
                           xlocal, ylocal, view_y1, view_y2,
                           info);
                UpdateRenderingStatePost(info, cell);
            }
            else
            {
                // the cell is off-screen, proceed with font+color+etc.
                // changes only:
                cell->DrawInvisible(dc, xlocal, ylocal, info);
            }
        }
    }
}



void CHtmlContainerCell::DrawInvisible(wxDC& dc, int x, int y,
                                        CHtmlRenderingInfo& info)
{
    if (m_Cells)
    {
        for (CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {
            UpdateRenderingStatePre(info, cell);
            cell->DrawInvisible(dc, x + m_PosX, y + m_PosY, info);
            UpdateRenderingStatePost(info, cell);
        }
    }
}


wxColour CHtmlContainerCell::GetBackgroundColour()
{
    if (m_UseBkColour)
        return m_BkColour;
    else
        return wxNullColour;
}



CHtmlLinkInfo *CHtmlContainerCell::GetLink(int x, int y) const
{
    CHtmlCell *cell = FindCellByPos(x, y);

    // VZ: I don't know if we should pass absolute or relative coords to
    //     CHtmlCell::GetLink()? As the base class version just ignores them
    //     anyhow, it hardly matters right now but should still be clarified
    return cell ? cell->GetLink(x, y) : NULL;
}



void CHtmlContainerCell::InsertCell(CHtmlCell *f)
{
    if (!m_Cells) m_Cells = m_LastCell = f;
    else
    {
        m_LastCell->SetNext(f);
        m_LastCell = f;
        if (m_LastCell) while (m_LastCell->GetNext()) m_LastCell = m_LastCell->GetNext();
    }
    f->SetParent(this);
    m_LastLayout = -1;
}



void CHtmlContainerCell::SetAlign(const CHtmlTag& tag)
{
    if (tag.HasParam(wxT("ALIGN")))
    {
        wxString alg = tag.GetParam(wxT("ALIGN"));
        alg.MakeUpper();
        if (alg == wxT("CENTER"))
            SetAlignHor(wxHTML_ALIGN_CENTER);
        else if (alg == wxT("LEFT"))
            SetAlignHor(wxHTML_ALIGN_LEFT);
        else if (alg == wxT("JUSTIFY"))
            SetAlignHor(wxHTML_ALIGN_JUSTIFY);
        else if (alg == wxT("RIGHT"))
            SetAlignHor(wxHTML_ALIGN_RIGHT);
        m_LastLayout = -1;
    }
}



void CHtmlContainerCell::SetWidthFloat(const CHtmlTag& tag, double pixel_scale)
{
    if (tag.HasParam(wxT("WIDTH")))
    {
        int wdi;
        wxString wd = tag.GetParam(wxT("WIDTH"));

        if (wd[wd.Length()-1] == wxT('%'))
        {
            wxSscanf(wd.c_str(), wxT("%i%%"), &wdi);
            SetWidthFloat(wdi, wxHTML_UNITS_PERCENT);
        }
        else
        {
            wxSscanf(wd.c_str(), wxT("%i"), &wdi);
            SetWidthFloat((int)(pixel_scale * (double)wdi), wxHTML_UNITS_PIXELS);
        }
        m_LastLayout = -1;
    }
}



const CHtmlCell* CHtmlContainerCell::Find(int condition, const void* param) const
{
    if (m_Cells)
    {
        for (CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext())
        {
            const CHtmlCell *r = cell->Find(condition, param);
            if (r) return r;
        }
    }
    return NULL;
}


CHtmlCell *CHtmlContainerCell::FindCellByPos(wxCoord x, wxCoord y,
                                               unsigned flags) const
{
    if ( flags & wxHTML_FIND_EXACT )
    {
        for ( const CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            int cx = cell->GetPosX(),
                cy = cell->GetPosY();

            if ( (cx <= x) && (cx + cell->GetWidth() > x) &&
                 (cy <= y) && (cy + cell->GetHeight() > y) )
            {
                return cell->FindCellByPos(x - cx, y - cy, flags);
            }
        }
    }
    else if ( flags & wxHTML_FIND_NEAREST_AFTER )
    {
        CHtmlCell *c;
        for ( const CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            if ( cell->IsFormattingCell() )
                continue;
            int cellY = cell->GetPosY();
            if (!( y < cellY || (y < cellY + cell->GetHeight() &&
                                 x < cell->GetPosX() + cell->GetWidth()) ))
                continue;

            c = cell->FindCellByPos(x - cell->GetPosX(), y - cellY, flags);
            if (c) return c;
        }
    }
    else if ( flags & wxHTML_FIND_NEAREST_BEFORE )
    {
        CHtmlCell *c2, *c = NULL;
        for ( const CHtmlCell *cell = m_Cells; cell; cell = cell->GetNext() )
        {
            if ( cell->IsFormattingCell() )
                continue;
            int cellY = cell->GetPosY();
            if (!( cellY + cell->GetHeight() <= y ||
                   (y >= cellY && x >= cell->GetPosX()) ))
                break;
            c2 = cell->FindCellByPos(x - cell->GetPosX(), y - cellY, flags);
            if (c2)
                c = c2;
        }
        if (c) return c;
    }

    return NULL;
}


void CHtmlContainerCell::OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event)
{
    CHtmlCell *cell = FindCellByPos(x, y);
    if ( cell )
        cell->OnMouseClick(parent, x, y, event);
}



CHtmlCell *CHtmlContainerCell::GetFirstTerminal() const
{
    if ( m_Cells )
    {
        CHtmlCell *c2;
        for (CHtmlCell *c = m_Cells; c; c = c->GetNext())
        {
            c2 = c->GetFirstTerminal();
            if ( c2 )
                return c2;
        }
    }
    return NULL;
}

CHtmlCell *CHtmlContainerCell::GetLastTerminal() const
{
    if ( m_Cells )
    {
        // most common case first:
        CHtmlCell *c = m_LastCell->GetLastTerminal();
        if ( c )
            return c;

        CHtmlCell *ctmp;
        CHtmlCell *c2 = NULL;
        for (c = m_Cells; c; c = c->GetNext())
        {
            ctmp = c->GetLastTerminal();
            if ( ctmp )
                c2 = ctmp;
        }
        return c2;
    }
    else
        return NULL;
}


static bool IsEmptyContainer(CHtmlContainerCell *cell)
{
    for ( CHtmlCell *c = cell->GetFirstChild(); c; c = c->GetNext() )
    {
        if ( !c->IsTerminalCell() || !c->IsFormattingCell() )
            return false;
    }
    return true;
}

void CHtmlContainerCell::RemoveExtraSpacing(bool top, bool bottom)
{
    if ( top )
        SetIndent(0, wxHTML_INDENT_TOP);
    if ( bottom )
        SetIndent(0, wxHTML_INDENT_BOTTOM);

    if ( m_Cells )
    {
        CHtmlCell *c;
        CHtmlContainerCell *cont;
        if ( top )
        {
            for ( c = m_Cells; c; c = c->GetNext() )
            {
                if ( c->IsTerminalCell() )
                {
                    if ( !c->IsFormattingCell() )
                        break;
                }
                else
                {
                    cont = (CHtmlContainerCell*)c;
                    if ( IsEmptyContainer(cont) )
                    {
                        cont->SetIndent(0, wxHTML_INDENT_VERTICAL);
                    }
                    else
                    {
                        cont->RemoveExtraSpacing(true, false);
                        break;
                    }
                }
            }
        }

        if ( bottom )
        {
            wxArrayPtrVoid arr;
            for ( c = m_Cells; c; c = c->GetNext() )
                arr.Add((void*)c);

            for ( int i = arr.GetCount() - 1; i >= 0; i--)
            {
                c = (CHtmlCell*)arr[i];
                if ( c->IsTerminalCell() )
                {
                    if ( !c->IsFormattingCell() )
                        break;
                }
                else
                {
                    cont = (CHtmlContainerCell*)c;
                    if ( IsEmptyContainer(cont) )
                    {
                        cont->SetIndent(0, wxHTML_INDENT_VERTICAL);
                    }
                    else
                    {
                        cont->RemoveExtraSpacing(false, true);
                        break;
                    }
                }
            }
        }
    }
}




// --------------------------------------------------------------------------
// CHtmlColourCell
// --------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlColourCell, CHtmlCell)

void CHtmlColourCell::Draw(wxDC& dc,
                            int x, int y,
                            int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                            CHtmlRenderingInfo& info)
{
    DrawInvisible(dc, x, y, info);
}

void CHtmlColourCell::DrawInvisible(wxDC& dc,
                                     int WXUNUSED(x), int WXUNUSED(y),
                                     CHtmlRenderingInfo& info)
{
    CHtmlRenderingState& state = info.GetState();
    if (m_Flags & wxHTML_CLR_FOREGROUND)
    {
        state.SetFgColour(m_Colour);
        if (state.GetSelectionState() != wxHTML_SEL_IN)
            dc.SetTextForeground(m_Colour);
        else
            dc.SetTextForeground(
                    info.GetStyle().GetSelectedTextColour(m_Colour));
    }
    if (m_Flags & wxHTML_CLR_BACKGROUND)
    {
        state.SetBgColour(m_Colour);
        if (state.GetSelectionState() != wxHTML_SEL_IN)
        {
            dc.SetTextBackground(m_Colour);
            dc.SetBackground(wxBrush(m_Colour, wxSOLID));
        }
        else
        {
            wxColour c = info.GetStyle().GetSelectedTextBgColour(m_Colour);
            dc.SetTextBackground(c);
            dc.SetBackground(wxBrush(c, wxSOLID));
        }
    }
}




// ---------------------------------------------------------------------------
// CHtmlFontCell
// ---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlFontCell, CHtmlCell)

void CHtmlFontCell::Draw(wxDC& dc,
                          int WXUNUSED(x), int WXUNUSED(y),
                          int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                          CHtmlRenderingInfo& WXUNUSED(info))
{
    dc.SetFont(m_Font);
}

void CHtmlFontCell::DrawInvisible(wxDC& dc, int WXUNUSED(x), int WXUNUSED(y),
                                   CHtmlRenderingInfo& WXUNUSED(info))
{
    dc.SetFont(m_Font);
}








// ---------------------------------------------------------------------------
// CHtmlWidgetCell
// ---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(CHtmlWidgetCell, CHtmlCell)

CHtmlWidgetCell::CHtmlWidgetCell(wxWindow *wnd, int w)
{
    int sx, sy;
    m_Wnd = wnd;
    m_Wnd->GetSize(&sx, &sy);
    m_Width = sx, m_Height = sy;
    m_WidthFloat = w;
}


void CHtmlWidgetCell::Draw(wxDC& WXUNUSED(dc),
                            int WXUNUSED(x), int WXUNUSED(y),
                            int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                            CHtmlRenderingInfo& WXUNUSED(info))
{
    int absx = 0, absy = 0, stx, sty;
    CHtmlCell *c = this;

    while (c)
    {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
    }

    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->SetSize(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty, m_Width, m_Height);
}



void CHtmlWidgetCell::DrawInvisible(wxDC& WXUNUSED(dc),
                                     int WXUNUSED(x), int WXUNUSED(y),
                                     CHtmlRenderingInfo& WXUNUSED(info))
{
    int absx = 0, absy = 0, stx, sty;
    CHtmlCell *c = this;

    while (c)
    {
        absx += c->GetPosX();
        absy += c->GetPosY();
        c = c->GetParent();
    }

    ((wxScrolledWindow*)(m_Wnd->GetParent()))->GetViewStart(&stx, &sty);
    m_Wnd->SetSize(absx - wxHTML_SCROLL_STEP * stx, absy  - wxHTML_SCROLL_STEP * sty, m_Width, m_Height);
}



void CHtmlWidgetCell::Layout(int w, int vw)
{
    if (m_WidthFloat != 0)
    {
        m_Width = (w * m_WidthFloat) / 100;
        m_Wnd->SetSize(m_Width, m_Height);
    }

    CHtmlCell::Layout(w, vw);
}



// ----------------------------------------------------------------------------
// CHtmlTerminalCellsInterator
// ----------------------------------------------------------------------------

const CHtmlCell* CHtmlTerminalCellsInterator::operator++()
{
    if ( !m_pos )
        return NULL;

    do
    {
        if ( m_pos == m_to )
        {
            m_pos = NULL;
            return NULL;
        }

        if ( m_pos->GetNext() )
            m_pos = m_pos->GetNext();
        else
        {
            // we must go up the hierarchy until we reach container where this
            // is not the last child, and then go down to first terminal cell:
            while ( m_pos->GetNext() == NULL )
            {
                m_pos = m_pos->GetParent();
                if ( !m_pos )
                    return NULL;
            }
            m_pos = m_pos->GetNext();
        }
        while ( m_pos->GetFirstChild() != NULL )
            m_pos = m_pos->GetFirstChild();
    } while ( !m_pos->IsTerminalCell() );

    return m_pos;
}







//-----------------------------------------------------------------------------
// Cleanup
//-----------------------------------------------------------------------------

class CHtmlCellModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(CHtmlCellModule)
public:
    CHtmlCellModule() : wxModule() {}
    bool OnInit() { return true; }
    void OnExit()
    {
        wxDELETE(gs_cursorLink);
        wxDELETE(gs_cursorText);
    }
};

IMPLEMENT_DYNAMIC_CLASS(CHtmlCellModule, wxModule)

#endif
