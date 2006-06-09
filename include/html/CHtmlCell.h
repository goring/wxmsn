/////////////////////////////////////////////////////////////////////////////
// Name:        CHtmlCell.h
// Purpose:     CHtmlCell class is used by CHtmlWindow/CHtmlWinParser
//              as a basic visual element of HTML page
// Author:      Vaclav Slavik
// RCS-ID:      $Id: CHtmlCell.h,v 1.58 2005/02/24 14:33:32 VZ Exp $
// Copyright:   (c) 1999-2003 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLCELL_H_
#define _WX_HTMLCELL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CHtmlCell.h"
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "CHtmlTag.h"
#include "htmldefs.h"
#include "wx/window.h"


class WXDLLIMPEXP_HTML CHtmlLinkInfo;
class WXDLLIMPEXP_HTML CHtmlCell;
class WXDLLIMPEXP_HTML CHtmlContainerCell;


// CHtmlSelection is data holder with information about text selection.
// Selection is defined by two positions (beginning and end of the selection)
// and two leaf(!) cells at these positions.
class WXDLLIMPEXP_HTML CHtmlSelection
{
public:
    CHtmlSelection()
        : m_fromPos(wxDefaultPosition), m_toPos(wxDefaultPosition),
          m_fromPrivPos(wxDefaultPosition), m_toPrivPos(wxDefaultPosition),
          m_fromCell(NULL), m_toCell(NULL) {}

    void Set(const wxPoint& fromPos, const CHtmlCell *fromCell,
             const wxPoint& toPos, const CHtmlCell *toCell);
    void Set(const CHtmlCell *fromCell, const CHtmlCell *toCell);

    const CHtmlCell *GetFromCell() const { return m_fromCell; }
    const CHtmlCell *GetToCell() const { return m_toCell; }

    // these values are in absolute coordinates:
    const wxPoint& GetFromPos() const { return m_fromPos; }
    const wxPoint& GetToPos() const { return m_toPos; }

    // these are From/ToCell's private data
    const wxPoint& GetFromPrivPos() const { return m_fromPrivPos; }
    const wxPoint& GetToPrivPos() const { return m_toPrivPos; }
    void SetFromPrivPos(const wxPoint& pos) { m_fromPrivPos = pos; }
    void SetToPrivPos(const wxPoint& pos) { m_toPrivPos = pos; }
    void ClearPrivPos() { m_toPrivPos = m_fromPrivPos = wxDefaultPosition; }

    bool IsEmpty() const
        { return m_fromPos == wxDefaultPosition &&
                 m_toPos == wxDefaultPosition; }

private:
    wxPoint m_fromPos, m_toPos;
    wxPoint m_fromPrivPos, m_toPrivPos;
    const CHtmlCell *m_fromCell, *m_toCell;
};



enum CHtmlSelectionState
{
    wxHTML_SEL_OUT,     // currently rendered cell is outside the selection
    wxHTML_SEL_IN,      // ... is inside selection
    wxHTML_SEL_CHANGING // ... is the cell on which selection state changes
};

// Selection state is passed to CHtmlCell::Draw so that it can render itself
// differently e.g. when inside text selection or outside it.
class WXDLLIMPEXP_HTML CHtmlRenderingState
{
public:
    CHtmlRenderingState() : m_selState(wxHTML_SEL_OUT) {}

    void SetSelectionState(CHtmlSelectionState s) { m_selState = s; }
    CHtmlSelectionState GetSelectionState() const { return m_selState; }

    void SetFgColour(const wxColour& c) { m_fgColour = c; }
    const wxColour& GetFgColour() const { return m_fgColour; }
    void SetBgColour(const wxColour& c) { m_bgColour = c; }
    const wxColour& GetBgColour() const { return m_bgColour; }

private:
    CHtmlSelectionState  m_selState;
    wxColour              m_fgColour, m_bgColour;
};


// HTML rendering customization. This class is used when rendering CHtmlCells
// as a callback:
class WXDLLIMPEXP_HTML CHtmlRenderingStyle
{
public:
    virtual ~CHtmlRenderingStyle() {}
    virtual wxColour GetSelectedTextColour(const wxColour& clr) = 0;
    virtual wxColour GetSelectedTextBgColour(const wxColour& clr) = 0;
};

// Standard style:
class WXDLLIMPEXP_HTML CDefaultHtmlRenderingStyle : public CHtmlRenderingStyle
{
public:
    virtual wxColour GetSelectedTextColour(const wxColour& clr);
    virtual wxColour GetSelectedTextBgColour(const wxColour& clr);
};


// Information given to cells when drawing them. Contains rendering state,
// selection information and rendering style object that can be used to
// customize the output.
class WXDLLIMPEXP_HTML CHtmlRenderingInfo
{
public:
    CHtmlRenderingInfo() : m_selection(NULL), m_style(NULL) {}

    void SetSelection(CHtmlSelection *s) { m_selection = s; }
    CHtmlSelection *GetSelection() const { return m_selection; }

    void SetStyle(CHtmlRenderingStyle *style) { m_style = style; }
    CHtmlRenderingStyle& GetStyle() { return *m_style; }

    CHtmlRenderingState& GetState() { return m_state; }

protected:
    CHtmlSelection      *m_selection;
    CHtmlRenderingStyle *m_style;
    CHtmlRenderingState m_state;
};


// Flags for CHtmlCell::FindCellByPos
enum
{
    wxHTML_FIND_EXACT             = 1,
    wxHTML_FIND_NEAREST_BEFORE    = 2,
    wxHTML_FIND_NEAREST_AFTER     = 4
};




// ---------------------------------------------------------------------------
// CHtmlCell
//                  Internal data structure. It represents fragments of parsed
//                  HTML page - a word, picture, table, horizontal line and so
//                  on.  It is used by CHtmlWindow to represent HTML page in
//                  memory.
// ---------------------------------------------------------------------------


class WXDLLIMPEXP_HTML CHtmlCell : public wxObject
{
public:
    CHtmlCell();
    virtual ~CHtmlCell();

    void SetParent(CHtmlContainerCell *p) {m_Parent = p;}
    CHtmlContainerCell *GetParent() const {return m_Parent;}

    int GetPosX() const {return m_PosX;}
    int GetPosY() const {return m_PosY;}
    int GetWidth() const {return m_Width;}

    // Returns the maximum possible length of the cell.
    // Call Layout at least once before using GetMaxTotalWidth()
    virtual int GetMaxTotalWidth() const { return m_Width; }

    int GetHeight() const {return m_Height;}
    int GetDescent() const {return m_Descent;}

    // Formatting cells are not visible on the screen, they only alter
    // renderer's state.
    bool IsFormattingCell() const { return m_Width == 0 && m_Height == 0; }

    const wxString& GetId() const { return m_id; }
	const int GetDataInt() const { return m_dataint; }
	const void* GetDataPtr() const { return m_dataptr; }
	const wxString& GetDataStr() const { return m_datastr; }

    void SetId(const wxString& id) { m_id = id; }
	void SetDataInt(const int data) { m_dataint = data; }
	void SetDataStr(const wxString& data) { m_datastr = data; }
	void SetDataPtr(void* data) { m_dataptr = data; }


    // returns the link associated with this cell. The position is position
    // within the cell so it varies from 0 to m_Width, from 0 to m_Height
    virtual CHtmlLinkInfo* GetLink(int WXUNUSED(x) = 0,
                                    int WXUNUSED(y) = 0) const
        { return m_Link; }

    // Returns cursor to be used when mouse is over the cell:
    virtual wxCursor GetCursor() const;

    // return next cell among parent's cells
    CHtmlCell *GetNext() const {return m_Next;}
    // returns first child cell (if there are any, i.e. if this is container):
    virtual CHtmlCell* GetFirstChild() const { return NULL; }

    // members writing methods
    virtual void SetPos(int x, int y) {m_PosX = x, m_PosY = y;}
    void SetLink(const CHtmlLinkInfo& link);
    void SetNext(CHtmlCell *cell) {m_Next = cell;}

    // 1. adjust cell's width according to the fact that maximal possible width
    //    is w.  (this has sense when working with horizontal lines, tables
    //    etc.)
    // 2. prepare layout (=fill-in m_PosX, m_PosY (and sometime m_Height)
    //    members) = place items to fit window, according to the width w
    virtual void Layout(int w, int vw);

    // renders the cell
    virtual void Draw(wxDC& WXUNUSED(dc),
                      int WXUNUSED(x), int WXUNUSED(y),
                      int WXUNUSED(view_y1), int WXUNUSED(view_y2),
                      CHtmlRenderingInfo& WXUNUSED(info)) {}

    // proceed drawing actions in case the cell is not visible (scrolled out of
    // screen).  This is needed to change fonts, colors and so on.
    virtual void DrawInvisible(wxDC& WXUNUSED(dc),
                               int WXUNUSED(x), int WXUNUSED(y),
                               CHtmlRenderingInfo& WXUNUSED(info)) {}

    // This method returns pointer to the FIRST cell for that
    // the condition
    // is true. It first checks if the condition is true for this
    // cell and then calls m_Next->Find(). (Note: it checks
    // all subcells if the cell is container)
    // Condition is unique condition identifier (see htmldefs.h)
    // (user-defined condition IDs should start from 10000)
    // and param is optional parameter
    // Example : m_Cell->Find(wxHTML_COND_ISANCHOR, "news");
    //   returns pointer to anchor news
    virtual const CHtmlCell* Find(int condition, const void* param) const;

    // This function is called when mouse button is clicked over the cell.
    //
    // Parent is pointer to CHtmlWindow that generated the event
    // HINT: if this handling is not enough for you you should use
    //       CHtmlWidgetCell
    virtual void OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event);

    // This method used to adjust pagebreak position. The parameter is variable
    // that contains y-coordinate of page break (= horizontal line that should
    // not be crossed by words, images etc.). If this cell cannot be divided
    // into two pieces (each one on another page) then it moves the pagebreak
    // few pixels up.
    //
    // Returned value : true if pagebreak was modified, false otherwise
    // Usage : while (container->AdjustPagebreak(&p)) {}
    virtual bool AdjustPagebreak(int *pagebreak,
                                 int *known_pagebreaks = NULL,
                                 int number_of_pages = 0) const;

    // Sets cell's behaviour on pagebreaks (see AdjustPagebreak). Default
    // is true - the cell can be split on two pages
    void SetCanLiveOnPagebreak(bool can) { m_CanLiveOnPagebreak = can; }

    // Can the line be broken before this cell?
    virtual bool IsLinebreakAllowed() const
        { return !IsFormattingCell(); }

    // Returns true for simple == terminal cells, i.e. not composite ones.
    // This if for internal usage only and may disappear in future versions!
    virtual bool IsTerminalCell() const { return true; }

    // Find a cell inside this cell positioned at the given coordinates
    // (relative to this's positions). Returns NULL if no such cell exists.
    // The flag can be used to specify whether to look for terminal or
    // nonterminal cells or both. In either case, returned cell is deepest
    // cell in cells tree that contains [x,y].
    virtual CHtmlCell *FindCellByPos(wxCoord x, wxCoord y,
                                  unsigned flags = wxHTML_FIND_EXACT) const;

    // Returns absolute position of the cell on HTML canvas
    wxPoint GetAbsPos() const;

    // Returns first (last) terminal cell inside this cell. It may return NULL,
    // but it is rare -- only if there are no terminals in the tree.
    virtual CHtmlCell *GetFirstTerminal() const
        { return wxConstCast(this, CHtmlCell); }
    virtual CHtmlCell *GetLastTerminal() const
        { return wxConstCast(this, CHtmlCell); }

    // Returns cell's depth, i.e. how far under the root cell it is
    // (if it is the root, depth is 0)
    unsigned GetDepth() const;

    // Returns true if the cell appears before 'cell' in natural order of
    // cells (= as they are read). If cell A is (grand)parent of cell B,
    // then both A.IsBefore(B) and B.IsBefore(A) always return true.
    bool IsBefore(CHtmlCell *cell) const;

    // Converts the cell into text representation. If sel != NULL then
    // only part of the cell inside the selection is converted.
    virtual wxString ConvertToText(CHtmlSelection *WXUNUSED(sel)) const
        { return wxEmptyString; }

protected:
    CHtmlCell *m_Next;
            // pointer to the next cell
    CHtmlContainerCell *m_Parent;
            // pointer to parent cell
    long m_Width, m_VirtualWidth, m_Height, m_Descent;
            // dimensions of fragment
            // m_Descent is used to position text&images..
    long m_PosX, m_PosY;
            // position where the fragment is drawn
    CHtmlLinkInfo *m_Link;
            // destination address if this fragment is hypertext link, NULL otherwise
    bool m_CanLiveOnPagebreak;
            // true if this cell can be placed on pagebreak, false otherwise
    wxString m_id;
            // unique identifier of the cell, generated from "id" property of tags

	// Some members which can hold some useful user's infos
	int m_dataint;
	void* m_dataptr;
	wxString m_datastr;

    DECLARE_ABSTRACT_CLASS(CHtmlCell)
    DECLARE_NO_COPY_CLASS(CHtmlCell)
};




// ----------------------------------------------------------------------------
// Inherited cells:
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// CHtmlWordCell
//                  Single word in input stream.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlWordCell : public CHtmlCell
{
public:
    CHtmlWordCell(const wxString& word, wxDC& dc);
    void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
              CHtmlRenderingInfo& info);
    wxCursor GetCursor() const;
    wxString ConvertToText(CHtmlSelection *sel) const;
    bool IsLinebreakAllowed() const { return m_allowLinebreak; }

    void SetPreviousWord(CHtmlWordCell *cell);

protected:
    void SetSelectionPrivPos(wxDC& dc, CHtmlSelection *s) const;
    void Split(wxDC& dc,
               const wxPoint& selFrom, const wxPoint& selTo,
               unsigned& pos1, unsigned& pos2) const;

    wxString m_Word;
    bool     m_allowLinebreak;

    DECLARE_ABSTRACT_CLASS(CHtmlWordCell)
    DECLARE_NO_COPY_CLASS(CHtmlWordCell)
};





// Container contains other cells, thus forming tree structure of rendering
// elements. Basic code of layout algorithm is contained in this class.
class WXDLLIMPEXP_HTML CHtmlContainerCell : public CHtmlCell
{
public:
    CHtmlContainerCell(CHtmlContainerCell *parent);
    ~CHtmlContainerCell();

    virtual void Layout(int w, int vw);
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      CHtmlRenderingInfo& info);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               CHtmlRenderingInfo& info);
    virtual bool AdjustPagebreak(int *pagebreak, int *known_pagebreaks = NULL, int number_of_pages = 0) const;

    // insert cell at the end of m_Cells list
    void InsertCell(CHtmlCell *cell);

    // sets horizontal/vertical alignment
    void SetAlignHor(int al) {m_AlignHor = al; m_LastLayout = -1;}
    int GetAlignHor() const {return m_AlignHor;}
    void SetAlignVer(int al) {m_AlignVer = al; m_LastLayout = -1;}
    int GetAlignVer() const {return m_AlignVer;}

    // sets left-border indentation. units is one of wxHTML_UNITS_* constants
    // what is combination of wxHTML_INDENT_*
    void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS);
    // returns the indentation. ind is one of wxHTML_INDENT_* constants
    int GetIndent(int ind) const;
    // returns type of value returned by GetIndent(ind)
    int GetIndentUnits(int ind) const;

    // sets alignment info based on given tag's params
    void SetAlign(const CHtmlTag& tag);
    // sets floating width adjustment
    // (examples : 32 percent of parent container,
    // -15 pixels percent (this means 100 % - 15 pixels)
    void SetWidthFloat(int w, int units) {m_WidthFloat = w; m_WidthFloatUnits = units; m_LastLayout = -1;}
    void SetWidthFloat(const CHtmlTag& tag, double pixel_scale = 1.0);
    // sets minimal height of this container.
    void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP) {m_MinHeight = h; m_MinHeightAlign = align; m_LastLayout = -1;}

    void SetBackgroundColour(const wxColour& clr) {m_UseBkColour = true; m_BkColour = clr;}
    // returns background colour (of wxNullColour if none set), so that widgets can
    // adapt to it:
    wxColour GetBackgroundColour();
    void SetBorder(const wxColour& clr1, const wxColour& clr2) {m_UseBorder = true; m_BorderColour1 = clr1, m_BorderColour2 = clr2;}
    virtual CHtmlLinkInfo* GetLink(int x = 0, int y = 0) const;
    virtual const CHtmlCell* Find(int condition, const void* param) const;
    virtual void OnMouseClick(wxWindow *parent, int x, int y, const wxMouseEvent& event);

    virtual CHtmlCell* GetFirstChild() const { return m_Cells; }
#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( CHtmlCell* GetFirstCell() const );
#endif

    // see comment in CHtmlCell about this method
    virtual bool IsTerminalCell() const { return false; }

    virtual CHtmlCell *FindCellByPos(wxCoord x, wxCoord y,
                                  unsigned flags = wxHTML_FIND_EXACT) const;

    virtual CHtmlCell *GetFirstTerminal() const;
    virtual CHtmlCell *GetLastTerminal() const;


    // Removes indentation on top or bottom of the container (i.e. above or
    // below first/last terminal cell). For internal use only.
    void RemoveExtraSpacing(bool top, bool bottom);

    // Returns the maximum possible length of the container.
    // Call Layout at least once before using GetMaxTotalWidth()
    virtual int GetMaxTotalWidth() const { return m_MaxTotalWidth; }

protected:
    void UpdateRenderingStatePre(CHtmlRenderingInfo& info,
                                 CHtmlCell *cell) const;
    void UpdateRenderingStatePost(CHtmlRenderingInfo& info,
                                  CHtmlCell *cell) const;

protected:
    int m_IndentLeft, m_IndentRight, m_IndentTop, m_IndentBottom;
            // indentation of subcells. There is always m_Indent pixels
            // big space between given border of the container and the subcells
            // it m_Indent < 0 it is in PERCENTS, otherwise it is in pixels
    int m_MinHeight, m_MinHeightAlign;
        // minimal height.
    CHtmlCell *m_Cells, *m_LastCell;
            // internal cells, m_Cells points to the first of them, m_LastCell to the last one.
            // (LastCell is needed only to speed-up InsertCell)
    int m_AlignHor, m_AlignVer;
            // alignment horizontal and vertical (left, center, right)
    int m_WidthFloat, m_WidthFloatUnits;
            // width float is used in adjustWidth
    bool m_UseBkColour;
    wxColour m_BkColour;
            // background color of this container
    bool m_UseBorder;
    wxColour m_BorderColour1, m_BorderColour2;
            // borders color of this container
    int m_LastLayout;
            // if != -1 then call to Layout may be no-op
            // if previous call to Layout has same argument
    int m_MaxTotalWidth;
            // Maximum possible length if ignoring line wrap


    DECLARE_ABSTRACT_CLASS(CHtmlContainerCell)
    DECLARE_NO_COPY_CLASS(CHtmlContainerCell)
};

#if WXWIN_COMPATIBILITY_2_4
inline CHtmlCell* CHtmlContainerCell::GetFirstCell() const
    { return GetFirstChild(); }
#endif




// ---------------------------------------------------------------------------
// CHtmlColourCell
//                  Color changer.
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlColourCell : public CHtmlCell
{
public:
    CHtmlColourCell(const wxColour& clr, int flags = wxHTML_CLR_FOREGROUND) : CHtmlCell() {m_Colour = clr; m_Flags = flags;}
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      CHtmlRenderingInfo& info);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               CHtmlRenderingInfo& info);

protected:
    wxColour m_Colour;
    unsigned m_Flags;

    DECLARE_ABSTRACT_CLASS(CHtmlColourCell)
    DECLARE_NO_COPY_CLASS(CHtmlColourCell)
};




//--------------------------------------------------------------------------------
// CHtmlFontCell
//                  Sets actual font used for text rendering
//--------------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlFontCell : public CHtmlCell
{
public:
    CHtmlFontCell(wxFont *font) : CHtmlCell() { m_Font = (*font); }
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      CHtmlRenderingInfo& info);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               CHtmlRenderingInfo& info);

protected:
    wxFont m_Font;

    DECLARE_ABSTRACT_CLASS(CHtmlFontCell)
    DECLARE_NO_COPY_CLASS(CHtmlFontCell)
};






//--------------------------------------------------------------------------------
// CHtmlWidgetCell
//                  This cell is connected with wxWindow object
//                  You can use it to insert windows into HTML page
//                  (buttons, input boxes etc.)
//--------------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlWidgetCell : public CHtmlCell
{
public:
    // !!! wnd must have correct parent!
    // if w != 0 then the m_Wnd has 'floating' width - it adjust
    // it's width according to parent container's width
    // (w is percent of parent's width)
    CHtmlWidgetCell(wxWindow *wnd, int w = 0);
    ~CHtmlWidgetCell() { m_Wnd->Destroy(); }
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2,
                      CHtmlRenderingInfo& info);
    virtual void DrawInvisible(wxDC& dc, int x, int y,
                               CHtmlRenderingInfo& info);
    virtual void Layout(int w, int vw);

protected:
    wxWindow* m_Wnd;
    int m_WidthFloat;
            // width float is used in adjustWidth (it is in percents)

    DECLARE_ABSTRACT_CLASS(CHtmlWidgetCell)
    DECLARE_NO_COPY_CLASS(CHtmlWidgetCell)
};



//--------------------------------------------------------------------------------
// CHtmlLinkInfo
//                  Internal data structure. It represents hypertext link
//--------------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlLinkInfo : public wxObject
{
public:
    CHtmlLinkInfo() : wxObject()
          { m_Href = m_Target = wxEmptyString; m_Event = NULL, m_Cell = NULL; }
    CHtmlLinkInfo(const wxString& href, const wxString& target = wxEmptyString) : wxObject()
          { m_Href = href; m_Target = target; m_Event = NULL, m_Cell = NULL; }
    CHtmlLinkInfo(const CHtmlLinkInfo& l) : wxObject()
          { m_Href = l.m_Href, m_Target = l.m_Target, m_Event = l.m_Event;
            m_Cell = l.m_Cell; }
    CHtmlLinkInfo& operator=(const CHtmlLinkInfo& l)
          { m_Href = l.m_Href, m_Target = l.m_Target, m_Event = l.m_Event;
            m_Cell = l.m_Cell; return *this; }

    void SetEvent(const wxMouseEvent *e) { m_Event = e; }
    void SetHtmlCell(const CHtmlCell *e) { m_Cell = e; }

    wxString GetHref() const { return m_Href; }
    wxString GetTarget() const { return m_Target; }
    const wxMouseEvent* GetEvent() const { return m_Event; }
    const CHtmlCell* GetHtmlCell() const { return m_Cell; }

private:
    wxString m_Href, m_Target;
    const wxMouseEvent *m_Event;
    const CHtmlCell *m_Cell;
};



// ----------------------------------------------------------------------------
// CHtmlTerminalCellsInterator
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlTerminalCellsInterator
{
public:
    CHtmlTerminalCellsInterator(const CHtmlCell *from, const CHtmlCell *to)
        : m_to(to), m_pos(from) {}

    operator bool() const { return m_pos != NULL; }
    const CHtmlCell* operator++();
    const CHtmlCell* operator->() const { return m_pos; }
    const CHtmlCell* operator*() const { return m_pos; }

private:
    const CHtmlCell *m_to, *m_pos;
};



#endif // wxUSE_HTML

#endif // _WX_HTMLCELL_H_
