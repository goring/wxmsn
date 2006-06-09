/////////////////////////////////////////////////////////////////////////////
// Name:        htmlwin.cpp
// Purpose:     CHtmlWindow class for parsing & displaying HTML (implementation)
// Author:      Vaclav Slavik
// RCS-ID:      $Id: htmlwin.cpp,v 1.109 2005/05/23 10:54:39 ABX Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CHtmlWin.h"
#pragma implementation "htmlproc.h"
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
    #include "wx/dcclient.h"
    #include "wx/frame.h"
#endif

#include <iostream>

#include "CHtmlWin.h"
#include "CHtmlProc.h"
#include "wx/list.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/timer.h"
#include "wx/dcmemory.h"
#include "wx/settings.h"

#include "wx/arrimpl.cpp"
#include "wx/listimpl.cpp"


#if wxUSE_CLIPBOARD
// ----------------------------------------------------------------------------
// CHtmlWinAutoScrollTimer: the timer used to generate a stream of scroll
// events when a captured mouse is held outside the window
// ----------------------------------------------------------------------------

class CHtmlWinAutoScrollTimer : public wxTimer
{
public:
    CHtmlWinAutoScrollTimer(wxScrolledWindow *win,
                      wxEventType eventTypeToSend,
                      int pos, int orient)
    {
        m_win = win;
        m_eventType = eventTypeToSend;
        m_pos = pos;
        m_orient = orient;
    }

    virtual void Notify();

private:
    wxScrolledWindow *m_win;
    wxEventType m_eventType;
    int m_pos,
        m_orient;

    DECLARE_NO_COPY_CLASS(CHtmlWinAutoScrollTimer)
};

void CHtmlWinAutoScrollTimer::Notify()
{
    // only do all this as long as the window is capturing the mouse
    if ( wxWindow::GetCapture() != m_win )
    {
        Stop();
    }
    else // we still capture the mouse, continue generating events
    {
        // first scroll the window if we are allowed to do it
        wxScrollWinEvent event1(m_eventType, m_pos, m_orient);
        event1.SetEventObject(m_win);
        if ( m_win->GetEventHandler()->ProcessEvent(event1) )
        {
            // and then send a pseudo mouse-move event to refresh the selection
            wxMouseEvent event2(wxEVT_MOTION);
            wxGetMousePosition(&event2.m_x, &event2.m_y);

            // the mouse event coordinates should be client, not screen as
            // returned by wxGetMousePosition
            wxWindow *parentTop = m_win;
            while ( parentTop->GetParent() )
                parentTop = parentTop->GetParent();
            wxPoint ptOrig = parentTop->GetPosition();
            event2.m_x -= ptOrig.x;
            event2.m_y -= ptOrig.y;

            event2.SetEventObject(m_win);

            // FIXME: we don't fill in the other members - ok?
            m_win->GetEventHandler()->ProcessEvent(event2);
        }
        else // can't scroll further, stop
        {
            Stop();
        }
    }
}

#endif // wxUSE_CLIPBOARD


//-----------------------------------------------------------------------------
// our private arrays:
//-----------------------------------------------------------------------------

WX_DECLARE_LIST(CHtmlProcessor, CHtmlProcessorList);
WX_DEFINE_LIST(CHtmlProcessorList);


//-----------------------------------------------------------------------------
// CHtmlWindow
//-----------------------------------------------------------------------------


void CHtmlWindow::Init()
{
    m_tmpMouseMoved = false;
    m_tmpLastLink = NULL;
    m_tmpLastCell = NULL;
    m_tmpCanDrawLocks = 0;
    m_FS = new wxFileSystem();
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Cell = NULL;
    m_Parser = new CHtmlWinParser(this);
    m_Parser->SetFS(m_FS);
    m_Processors = NULL;
    m_Style = 0;
    SetBorders(10);
    m_selection = NULL;
    m_makingSelection = false;
#if wxUSE_CLIPBOARD
    m_timerAutoScroll = NULL;
    m_lastDoubleClick = 0;
#endif // wxUSE_CLIPBOARD
    m_backBuffer = NULL;
	m_backBgBuffer = NULL;
    m_tmpSelFromCell = NULL;
}

bool CHtmlWindow::Create(wxWindow *parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    if (!wxScrolledWindow::Create(parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL,
                                  name))
        return false;

    m_Style = style;
    SetPage(wxT("<html><body></body></html>"));
	EnableScrolling(false, false);
    return true;
}


CHtmlWindow::~CHtmlWindow()
{
#if wxUSE_CLIPBOARD
    StopAutoScrolling();
#endif // wxUSE_CLIPBOARD

    delete m_selection;

    delete m_Cell;

    if ( m_Processors )
    {
        WX_CLEAR_LIST(CHtmlProcessorList, *m_Processors);
    }

    delete m_Parser;
    delete m_FS;
    delete m_Processors;
    delete m_backBuffer;
	delete m_backBgBuffer;
}


void CHtmlWindow::SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
{
    wxString op = m_OpenedPage;

    m_Parser->SetFonts(normal_face, fixed_face, sizes);
    // fonts changed => contents invalid, so reload the page:
    SetPage(wxT("<html><body></body></html>"));
    if (!op.empty())
        LoadPage(op);
}

void CHtmlWindow::SetStandardFonts(int size,
                                    const wxString& normal_face,
                                    const wxString& fixed_face)
{
    wxString op = m_OpenedPage;

    m_Parser->SetStandardFonts(size, normal_face, fixed_face);
    // fonts changed => contents invalid, so reload the page:
    SetPage(wxT("<html><body></body></html>"));
    if (!op.empty())
        LoadPage(op);
}


bool CHtmlWindow::SetPage(const wxString& source)
{
    wxString newsrc(source);

    wxDELETE(m_selection);

    // we will soon delete all the cells, so clear pointers to them:
    m_tmpSelFromCell = NULL;

    // pass HTML through registered processors:
    if (m_Processors || m_GlobalProcessors)
    {
        CHtmlProcessorList::compatibility_iterator nodeL, nodeG;
        int prL, prG;

        nodeL = (m_Processors) ? m_Processors->GetFirst() : CHtmlProcessorList::compatibility_iterator();
        nodeG = (m_GlobalProcessors) ? m_GlobalProcessors->GetFirst() : CHtmlProcessorList::compatibility_iterator();

        // VS: there are two lists, global and local, both of them sorted by
        //     priority. Since we have to go through _both_ lists with
        //     decreasing priority, we "merge-sort" the lists on-line by
        //     processing that one of the two heads that has higher priority
        //     in every iteration
        while (nodeL || nodeG)
        {
            prL = (nodeL) ? nodeL->GetData()->GetPriority() : -1;
            prG = (nodeG) ? nodeG->GetData()->GetPriority() : -1;
            if (prL > prG)
            {
                if (nodeL->GetData()->IsEnabled())
                    newsrc = nodeL->GetData()->Process(newsrc);
                nodeL = nodeL->GetNext();
            }
            else // prL <= prG
            {
                if (nodeG->GetData()->IsEnabled())
                    newsrc = nodeG->GetData()->Process(newsrc);
                nodeG = nodeG->GetNext();
            }
        }
    }

    // ...and run the parser on it:
    SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    SetBackgroundImage(wxNullBitmap);
    wxClientDC dc(this);
    dc.BeginDrawing();
    dc.SetMapMode(wxMM_TEXT);
    m_OpenedPage = m_OpenedAnchor = m_OpenedPageTitle = wxEmptyString;
    m_Parser->SetDC(&dc);
    if(m_Cell)
    {
        delete m_Cell;
        m_Cell = NULL;
    }
    m_Cell = (CHtmlContainerCell*)m_Parser->Parse(newsrc);
    m_Cell->SetIndent(m_Borders, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cell->SetAlignHor(wxHTML_ALIGN_CENTER);
    dc.EndDrawing();
    CreateLayout();
    if(m_tmpCanDrawLocks == 0)
    {
		Refresh();
	}

    return true;
}

bool CHtmlWindow::AppendToPage(const wxString& source)
{
    return SetPage(*(GetParser()->GetSource()) + source);
}

bool CHtmlWindow::LoadPage(const wxString& location)
{
    wxBusyCursor busyCursor;

    wxFSFile *f;
    bool rt_val;
    bool needs_refresh = false;

    m_tmpCanDrawLocks++;

    if (location[0] == wxT('#'))
    {
        // local anchor:
        wxString anch = location.Mid(1) /*1 to end*/;
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND && location.BeforeFirst(wxT('#')) == m_OpenedPage)
    {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }
    else if (location.Find(wxT('#')) != wxNOT_FOUND &&
             (m_FS->GetPath() + location.BeforeFirst(wxT('#'))) == m_OpenedPage)
    {
        wxString anch = location.AfterFirst(wxT('#'));
        m_tmpCanDrawLocks--;
        rt_val = ScrollToAnchor(anch);
        m_tmpCanDrawLocks++;
    }

    else
    {
        needs_refresh = true;

        f = m_Parser->OpenURL(wxHTML_URL_PAGE, location);

        // try to interpret 'location' as filename instead of URL:
        if (f == NULL)
        {
            wxFileName fn(location);
            wxString location2 = wxFileSystem::FileNameToURL(fn);
            f = m_Parser->OpenURL(wxHTML_URL_PAGE, location2);
        }

        if (f == NULL)
        {
            wxLogError(_("Unable to open requested HTML document: %s"), location.c_str());
            m_tmpCanDrawLocks--;
            return false;
        }

        else
        {
            wxList::compatibility_iterator node;
            wxString src = wxEmptyString;

            node = m_Filters.GetFirst();
            while (node)
            {
                CHtmlFilter *h = (CHtmlFilter*) node->GetData();
                if (h->CanRead(*f))
                {
                    src = h->ReadFile(*f);
                    break;
                }
                node = node->GetNext();
            }
            if (src == wxEmptyString)
            {
                if (m_DefaultFilter == NULL) m_DefaultFilter = GetDefaultFilter();
                src = m_DefaultFilter->ReadFile(*f);
            }

            m_FS->ChangePathTo(f->GetLocation());
            rt_val = SetPage(src);
            m_OpenedPage = f->GetLocation();
            if (f->GetAnchor() != wxEmptyString)
            {
                ScrollToAnchor(f->GetAnchor());
            }

            delete f;
        }
    }

    if (needs_refresh)
    {
        m_tmpCanDrawLocks--;
        Refresh();
    }
    else
        m_tmpCanDrawLocks--;

    return rt_val;
}


bool CHtmlWindow::LoadFile(const wxFileName& filename)
{
    wxString url = wxFileSystem::FileNameToURL(filename);
    return LoadPage(url);
}


bool CHtmlWindow::ScrollToAnchor(const wxString& anchor)
{
    const CHtmlCell *c = m_Cell->Find(wxHTML_COND_ISANCHOR, &anchor);
    if (!c)
    {
        wxLogWarning(_("HTML anchor %s does not exist."), anchor.c_str());
        return false;
    }
    else
    {
        int y;

        for (y = 0; c != NULL; c = c->GetParent()) y += c->GetPosY();
        Scroll(-1, y / wxHTML_SCROLL_STEP);
        m_OpenedAnchor = anchor;
        return true;
    }
}


void CHtmlWindow::CreateLayout()
{
	if(m_Cell != NULL)
	{
		int ScrollX, ScrollY;
		GetViewStart(&ScrollX, &ScrollY);

		int ClientWidth, ClientHeight, VirtualClientWidth, VirtualClientHeight;
		GetClientSize(&ClientWidth, &ClientHeight);
		GetVirtualSize(&VirtualClientWidth, &VirtualClientHeight);
		m_Cell->Layout(ClientWidth, VirtualClientWidth);

		if(m_Style & wxHW_SCROLLBAR_NEVER)
		{
			SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0, ScrollX, ScrollY);
		}
		else
		{
			if(ClientHeight < m_Cell->GetHeight() + GetCharHeight())
			{
				SetScrollbars(wxHTML_SCROLL_STEP, wxHTML_SCROLL_STEP, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, (m_Cell->GetHeight() + GetCharHeight()) / wxHTML_SCROLL_STEP, ScrollX, ScrollY);
			}
			else
			{
				SetScrollbars(wxHTML_SCROLL_STEP, 1, m_Cell->GetWidth() / wxHTML_SCROLL_STEP, 0, ScrollX, ScrollY);
			}
		}
	}
}


void CHtmlWindow::AddProcessor(CHtmlProcessor *processor)
{
    if (!m_Processors)
    {
        m_Processors = new CHtmlProcessorList;
    }
    CHtmlProcessorList::compatibility_iterator node;

    for (node = m_Processors->GetFirst(); node; node = node->GetNext())
    {
        if (processor->GetPriority() > node->GetData()->GetPriority())
        {
            m_Processors->Insert(node, processor);
            return;
        }
    }
    m_Processors->Append(processor);
}

/*static */ void CHtmlWindow::AddGlobalProcessor(CHtmlProcessor *processor)
{
    if (!m_GlobalProcessors)
    {
        m_GlobalProcessors = new CHtmlProcessorList;
    }
    CHtmlProcessorList::compatibility_iterator node;

    for (node = m_GlobalProcessors->GetFirst(); node; node = node->GetNext())
    {
        if (processor->GetPriority() > node->GetData()->GetPriority())
        {
            m_GlobalProcessors->Insert(node, processor);
            return;
        }
    }
    m_GlobalProcessors->Append(processor);
}



wxList CHtmlWindow::m_Filters;
CHtmlFilter *CHtmlWindow::m_DefaultFilter = NULL;
CHtmlProcessorList *CHtmlWindow::m_GlobalProcessors = NULL;

void CHtmlWindow::CleanUpStatics()
{
    wxDELETE(m_DefaultFilter);
    WX_CLEAR_LIST(wxList, m_Filters);
    if (m_GlobalProcessors)
        WX_CLEAR_LIST(CHtmlProcessorList, *m_GlobalProcessors);
    wxDELETE(m_GlobalProcessors);
}



void CHtmlWindow::AddFilter(CHtmlFilter *filter)
{
    m_Filters.Append(filter);
}


bool CHtmlWindow::IsSelectionEnabled() const
{
#if wxUSE_CLIPBOARD
    return !(m_Style & wxHW_NO_SELECTION);
#else
    return false;
#endif
}


#if wxUSE_CLIPBOARD
wxString CHtmlWindow::DoSelectionToText(CHtmlSelection *sel)
{
    if ( !sel )
        return wxEmptyString;

    const CHtmlCell *end = sel->GetToCell();
    wxString text;
    CHtmlTerminalCellsInterator i(sel->GetFromCell(), end);
    if ( i )
    {
        text << i->ConvertToText(sel);
        ++i;
    }
    const CHtmlCell *prev = *i;
    while ( i )
    {
        if ( prev->GetParent() != i->GetParent() )
            text << _T('\n');
        text << i->ConvertToText(*i == end ? sel : NULL);
        prev = *i;
        ++i;
    }
    return text;
}

wxString CHtmlWindow::ToText()
{
    if (m_Cell)
    {
        CHtmlSelection sel;
        sel.Set(m_Cell->GetFirstTerminal(), m_Cell->GetLastTerminal());
        return DoSelectionToText(&sel);
    }
    else
        return wxEmptyString;
}

#endif // wxUSE_CLIPBOARD

bool CHtmlWindow::CopySelection(ClipboardType t)
{
#if wxUSE_CLIPBOARD
    if ( m_selection )
    {
#if defined(__UNIX__) && !defined(__WXMAC__)
        wxTheClipboard->UsePrimarySelection(t == Primary);
#else // !__UNIX__
        // Primary selection exists only under X11, so don't do anything under
        // the other platforms when we try to access it
        //
        // TODO: this should be abstracted at wxClipboard level!
        if ( t == Primary )
            return false;
#endif // __UNIX__/!__UNIX__

        if ( wxTheClipboard->Open() )
        {
            const wxString txt(SelectionToText());
            wxTheClipboard->SetData(new wxTextDataObject(txt));
            wxTheClipboard->Close();
            wxLogTrace(_T("wxhtmlselection"),
                       _("Copied to clipboard:\"%s\""), txt.c_str());

            return true;
        }
    }
#else
    wxUnusedVar(t);
#endif // wxUSE_CLIPBOARD

    return false;
}


void CHtmlWindow::OnLinkClicked(const CHtmlLinkInfo& link)
{
    const wxMouseEvent *e = link.GetEvent();
    if (e == NULL || e->LeftUp())
        LoadPage(link.GetHref());
}

void CHtmlWindow::OnCellClicked(CHtmlCell *cell,
                                 wxCoord x, wxCoord y,
                                 const wxMouseEvent& event)
{
    wxCHECK_RET( cell, _T("can't be called with NULL cell") );

    cell->OnMouseClick(this, x, y, event);
}

void CHtmlWindow::OnCellMouseHover(CHtmlCell * WXUNUSED(cell),
                                    wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    // do nothing here
}

void CHtmlWindow::OnEraseBackground(wxEraseEvent& event)
{

}

void CHtmlWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	if(m_tmpCanDrawLocks <= 0 && m_Cell != NULL)
	{
		wxRect rect = GetUpdateRegion().GetBox();
		wxSize sz = GetSize();

		if(!m_backBuffer)
			m_backBuffer = new wxBitmap(sz.x, sz.y);

		wxBufferedPaintDC PaintDC(this, *m_backBuffer);

		PaintDC.BeginDrawing();

		if(m_bmpBg.Ok())
		{
			if(!m_backBgBuffer)
			{
				wxMemoryDC bgdcm;
				m_backBgBuffer = new wxBitmap(sz.x, sz.y);
				bgdcm.SelectObject(*m_backBgBuffer);

				for(wxCoord x=0; x<sz.x; x+=m_bmpBg.GetWidth())
				{
					for(wxCoord y=0; y<sz.y; y+=m_bmpBg.GetHeight())
					{
						bgdcm.DrawBitmap(m_bmpBg, x, y, true);
					}
				}
			}

			if(m_bmpBg.GetMask())
			{
				PaintDC.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
				PaintDC.Clear();
			}

			PaintDC.DrawBitmap(*m_backBgBuffer, 0, 0, true);
		}
		else
		{
			PaintDC.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
			PaintDC.Clear();
		}

		DoPrepareDC(PaintDC);
		PaintDC.SetMapMode(wxMM_TEXT);
		PaintDC.SetBackgroundMode(wxTRANSPARENT);

		CHtmlRenderingInfo rinfo;
		CDefaultHtmlRenderingStyle rstyle;
		rinfo.SetSelection(m_selection);
		rinfo.SetStyle(&rstyle);
		int x, y;
		GetViewStart(&x, &y);

		m_Cell->Draw(PaintDC, 0, 0,
					 y * wxHTML_SCROLL_STEP + rect.GetTop(),
					 y * wxHTML_SCROLL_STEP + rect.GetBottom(),
					 rinfo);

		PaintDC.EndDrawing();
	}
	else
	{
		wxPaintDC PaintDC(this);
	}
}


void CHtmlWindow::OnSize(wxSizeEvent& event)
{
    wxDELETE(m_backBuffer);
    wxDELETE(m_backBgBuffer);

    wxScrolledWindow::OnSize(event);
    CreateLayout();

    // Recompute selection if necessary:
    if ( m_selection )
    {
        m_selection->Set(m_selection->GetFromCell(),
                         m_selection->GetToCell());
        m_selection->ClearPrivPos();
    }

    Refresh();
}


void CHtmlWindow::OnMouseMove(wxMouseEvent& WXUNUSED(event))
{
    m_tmpMouseMoved = true;
}

void CHtmlWindow::OnMouseDown(wxMouseEvent& event)
{
#if wxUSE_CLIPBOARD
    if ( event.LeftDown() && IsSelectionEnabled() )
    {
        const long TRIPLECLICK_LEN = 200; // 0.2 sec after doubleclick
        if ( wxGetLocalTimeMillis() - m_lastDoubleClick <= TRIPLECLICK_LEN )
        {
            SelectLine(CalcUnscrolledPosition(event.GetPosition()));

            (void) CopySelection();
        }
        else
        {
            m_makingSelection = true;

            if ( m_selection )
            {
                wxDELETE(m_selection);
                Refresh();
            }
            m_tmpSelFromPos = CalcUnscrolledPosition(event.GetPosition());
            m_tmpSelFromCell = NULL;

            CaptureMouse();
        }
    }
#else
    wxUnusedVar(event);
#endif // wxUSE_CLIPBOARD
}

void CHtmlWindow::OnMouseUp(wxMouseEvent& event)
{
#if wxUSE_CLIPBOARD
    if ( m_makingSelection )
    {
        ReleaseMouse();
        m_makingSelection = false;

        // did the user move the mouse far enough from starting point?
        if ( CopySelection(Primary) )
        {
            // we don't want mouse up event that ended selecting to be
            // handled as mouse click and e.g. follow hyperlink:
            return;
        }
    }
#endif // wxUSE_CLIPBOARD

    SetFocus();
    if ( m_Cell )
    {
        wxPoint pos = CalcUnscrolledPosition(event.GetPosition());
        CHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);

        // check is needed because FindCellByPos returns terminal cell and
        // containers may have empty borders -- in this case NULL will be
        // returned
        if ( cell )
            OnCellClicked(cell, pos.x, pos.y, event);
    }
}


void CHtmlWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_tmpMouseMoved && (m_Cell != NULL))
    {
#ifdef DEBUG_HTML_SELECTION
        Refresh();
#endif
        int xc, yc, x, y;
        wxGetMousePosition(&xc, &yc);
        ScreenToClient(&xc, &yc);
        CalcUnscrolledPosition(xc, yc, &x, &y);

        CHtmlCell *cell = m_Cell->FindCellByPos(x, y);

        // handle selection update:
        if ( m_makingSelection )
        {
            if ( !m_tmpSelFromCell )
                m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y);

            // NB: a trick - we adjust selFromPos to be upper left or bottom
            //     right corner of the first cell of the selection depending
            //     on whether the mouse is moving to the right or to the left.
            //     This gives us more "natural" behaviour when selecting
            //     a line (specifically, first cell of the next line is not
            //     included if you drag selection from left to right over
            //     entire line):
            wxPoint dirFromPos;
            if ( !m_tmpSelFromCell )
            {
                dirFromPos = m_tmpSelFromPos;
            }
            else
            {
                dirFromPos = m_tmpSelFromCell->GetAbsPos();
                if ( x < m_tmpSelFromPos.x )
                {
                    dirFromPos.x += m_tmpSelFromCell->GetWidth();
                    dirFromPos.y += m_tmpSelFromCell->GetHeight();
                }
            }
            bool goingDown = dirFromPos.y < y ||
                             (dirFromPos.y == y && dirFromPos.x < x);

            // determine selection span:
            if ( /*still*/ !m_tmpSelFromCell )
            {
                if (goingDown)
                {
                    m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y,
                                         wxHTML_FIND_NEAREST_AFTER);
                    if (!m_tmpSelFromCell)
                        m_tmpSelFromCell = m_Cell->GetFirstTerminal();
                }
                else
                {
                    m_tmpSelFromCell = m_Cell->FindCellByPos(
                                         m_tmpSelFromPos.x,m_tmpSelFromPos.y,
                                         wxHTML_FIND_NEAREST_BEFORE);
                    if (!m_tmpSelFromCell)
                        m_tmpSelFromCell = m_Cell->GetLastTerminal();
                }
            }

            CHtmlCell *selcell = cell;
            if (!selcell)
            {
                if (goingDown)
                {
                    selcell = m_Cell->FindCellByPos(x, y,
                                                 wxHTML_FIND_NEAREST_BEFORE);
                    if (!selcell)
                        selcell = m_Cell->GetLastTerminal();
                }
                else
                {
                    selcell = m_Cell->FindCellByPos(x, y,
                                                 wxHTML_FIND_NEAREST_AFTER);
                    if (!selcell)
                        selcell = m_Cell->GetFirstTerminal();
                }
            }

            // NB: it may *rarely* happen that the code above didn't find one
            //     of the cells, e.g. if CHtmlWindow doesn't contain any
            //     visible cells.
            if ( selcell && m_tmpSelFromCell )
            {
                if ( !m_selection )
                {
                    // start selecting only if mouse movement was big enough
                    // (otherwise it was meant as mouse click, not selection):
                    const int PRECISION = 2;
                    wxPoint diff = m_tmpSelFromPos - wxPoint(x,y);
                    if (abs(diff.x) > PRECISION || abs(diff.y) > PRECISION)
                    {
                        m_selection = new CHtmlSelection();
                    }
                }
                if ( m_selection )
                {
                    if ( m_tmpSelFromCell->IsBefore(selcell) )
                    {
                        m_selection->Set(m_tmpSelFromPos, m_tmpSelFromCell,
                                         wxPoint(x,y), selcell);                                    }
                    else
                    {
                        m_selection->Set(wxPoint(x,y), selcell,
                                         m_tmpSelFromPos, m_tmpSelFromCell);
                    }
                    m_selection->ClearPrivPos();
                    Refresh();
                }
            }
        }

        // handle cursor and status bar text changes:
        if ( cell != m_tmpLastCell )
        {
            CHtmlLinkInfo *lnk = cell ? cell->GetLink(x, y) : NULL;
            wxCursor cur;
            if (cell)
                cur = cell->GetCursor();
            else
                cur = *wxSTANDARD_CURSOR;
            SetCursor(cur);

            if (lnk != m_tmpLastLink)
            {
                m_tmpLastLink = lnk;
            }

            m_tmpLastCell = cell;
        }
        else // mouse moved but stayed in the same cell
        {
            if ( cell )
                OnCellMouseHover(cell, x, y);
        }

        m_tmpMouseMoved = false;
    }
}

#if wxUSE_CLIPBOARD
void CHtmlWindow::StopAutoScrolling()
{
    if ( m_timerAutoScroll )
    {
        wxDELETE(m_timerAutoScroll);
    }
}

void CHtmlWindow::OnMouseEnter(wxMouseEvent& event)
{
    StopAutoScrolling();
    event.Skip();
}

void CHtmlWindow::OnMouseLeave(wxMouseEvent& event)
{
    // don't prevent the usual processing of the event from taking place
    event.Skip();

    // when a captured mouse leave a scrolled window we start generate
    // scrolling events to allow, for example, extending selection beyond the
    // visible area in some controls
    if ( wxWindow::GetCapture() == this )
    {
        // where is the mouse leaving?
        int pos, orient;
        wxPoint pt = event.GetPosition();
        if ( pt.x < 0 )
        {
            orient = wxHORIZONTAL;
            pos = 0;
        }
        else if ( pt.y < 0 )
        {
            orient = wxVERTICAL;
            pos = 0;
        }
        else // we're lower or to the right of the window
        {
            wxSize size = GetClientSize();
            if ( pt.x > size.x )
            {
                orient = wxHORIZONTAL;
                pos = GetVirtualSize().x / wxHTML_SCROLL_STEP;
            }
            else if ( pt.y > size.y )
            {
                orient = wxVERTICAL;
                pos = GetVirtualSize().y / wxHTML_SCROLL_STEP;
            }
            else // this should be impossible
            {
                // but seems to happen sometimes under wxMSW - maybe it's a bug
                // there but for now just ignore it

                //wxFAIL_MSG( _T("can't understand where has mouse gone") );

                return;
            }
        }

        // only start the auto scroll timer if the window can be scrolled in
        // this direction
        if ( !HasScrollbar(orient) )
            return;

        delete m_timerAutoScroll;
        m_timerAutoScroll = new CHtmlWinAutoScrollTimer
                                (
                                    this,
                                    pos == 0 ? wxEVT_SCROLLWIN_LINEUP
                                             : wxEVT_SCROLLWIN_LINEDOWN,
                                    pos,
                                    orient
                                );
        m_timerAutoScroll->Start(50); // FIXME: make configurable
    }
}

void CHtmlWindow::OnKeyUp(wxKeyEvent& event)
{
    if ( IsSelectionEnabled() &&
         event.GetKeyCode() == 'C' && event.ControlDown() )
    {
        (void) CopySelection();
    }
}

void CHtmlWindow::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    (void) CopySelection();
}

void CHtmlWindow::OnDoubleClick(wxMouseEvent& event)
{
    // select word under cursor:
    if ( IsSelectionEnabled() )
    {
        SelectWord(CalcUnscrolledPosition(event.GetPosition()));

        (void) CopySelection(Primary);

        m_lastDoubleClick = wxGetLocalTimeMillis();
    }
    else
        event.Skip();
}

void CHtmlWindow::SelectWord(const wxPoint& pos)
{
    if ( m_Cell )
    {
        CHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);
        if ( cell )
        {
            delete m_selection;
            m_selection = new CHtmlSelection();
            m_selection->Set(cell, cell);
            RefreshRect(wxRect(CalcScrolledPosition(cell->GetAbsPos()),
                               wxSize(cell->GetWidth(), cell->GetHeight())));
        }
    }
}

void CHtmlWindow::SelectLine(const wxPoint& pos)
{
    if ( m_Cell )
    {
        CHtmlCell *cell = m_Cell->FindCellByPos(pos.x, pos.y);
        if ( cell )
        {
            // We use following heuristic to find a "line": let the line be all
            // cells in same container as the cell under mouse cursor that are
            // neither completely above nor completely bellow the clicked cell
            // (i.e. are likely to be words positioned on same line of text).

            int y1 = cell->GetAbsPos().y;
            int y2 = y1 + cell->GetHeight();
            int y;
            const CHtmlCell *c;
            const CHtmlCell *before = NULL;
            const CHtmlCell *after = NULL;

            // find last cell of line:
            for ( c = cell->GetNext(); c; c = c->GetNext())
            {
                y = c->GetAbsPos().y;
                if ( y + c->GetHeight() > y1 && y < y2 )
                    after = c;
                else
                    break;
            }
            if ( !after )
                after = cell;

            // find first cell of line:
            for ( c = cell->GetParent()->GetFirstChild();
                    c && c != cell; c = c->GetNext())
            {
                y = c->GetAbsPos().y;
                if ( y + c->GetHeight() > y1 && y < y2 )
                {
                    if ( ! before )
                        before = c;
                }
                else
                    before = NULL;
            }
            if ( !before )
                before = cell;

            delete m_selection;
            m_selection = new CHtmlSelection();
            m_selection->Set(before, after);

            Refresh();
        }
    }
}

void CHtmlWindow::SelectAll()
{
    if ( m_Cell )
    {
        delete m_selection;
        m_selection = new CHtmlSelection();
        m_selection->Set(m_Cell->GetFirstTerminal(), m_Cell->GetLastTerminal());
        Refresh();
    }
}

#endif // wxUSE_CLIPBOARD



IMPLEMENT_ABSTRACT_CLASS(CHtmlProcessor,wxObject)

#if wxUSE_EXTENDED_RTTI
IMPLEMENT_DYNAMIC_CLASS_XTI(CHtmlWindow, wxScrolledWindow,"CHtmlWin.h")

wxBEGIN_PROPERTIES_TABLE(CHtmlWindow)
/*
    TODO PROPERTIES
        style , wxHW_SCROLLBAR_AUTO
        borders , (dimension)
        url , string
        htmlcode , string
*/
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(CHtmlWindow)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( CHtmlWindow , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(CHtmlWindow,wxScrolledWindow)
#endif

BEGIN_EVENT_TABLE(CHtmlWindow, wxScrolledWindow)
    EVT_SIZE(CHtmlWindow::OnSize)
    EVT_LEFT_DOWN(CHtmlWindow::OnMouseDown)
    EVT_LEFT_UP(CHtmlWindow::OnMouseUp)
    EVT_RIGHT_UP(CHtmlWindow::OnMouseUp)
    EVT_MOTION(CHtmlWindow::OnMouseMove)
    EVT_ERASE_BACKGROUND(CHtmlWindow::OnEraseBackground)
    EVT_PAINT(CHtmlWindow::OnPaint)
#if wxUSE_CLIPBOARD
    EVT_LEFT_DCLICK(CHtmlWindow::OnDoubleClick)
    EVT_ENTER_WINDOW(CHtmlWindow::OnMouseEnter)
    EVT_LEAVE_WINDOW(CHtmlWindow::OnMouseLeave)
    EVT_KEY_UP(CHtmlWindow::OnKeyUp)
    EVT_MENU(wxID_COPY, CHtmlWindow::OnCopy)
#endif // wxUSE_CLIPBOARD
END_EVENT_TABLE()





// A module to allow initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class CHtmlWinModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(CHtmlWinModule)
public:
    CHtmlWinModule() : wxModule() {}
    bool OnInit() { return true; }
    void OnExit() { CHtmlWindow::CleanUpStatics(); }
};

IMPLEMENT_DYNAMIC_CLASS(CHtmlWinModule, wxModule)


// This hack forces the linker to always link in m_* files
// (wxHTML doesn't work without handlers from these files)
#include "forcelnk.h"
FORCE_WXHTML_MODULES()

#endif // wxUSE_HTML
