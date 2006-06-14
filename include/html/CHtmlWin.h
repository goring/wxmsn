/////////////////////////////////////////////////////////////////////////////
// Name:        CHtmlWin.h
// Purpose:     CHtmlWindow class for parsing & displaying HTML
// Author:      Vaclav Slavik
// RCS-ID:      $Id: CHtmlWin.h,v 1.66 2005/05/31 09:18:24 JS Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLWIN_H_
#define _WX_HTMLWIN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CHtmlWin.h"
#endif

#include "wx/defs.h"
#if wxUSE_HTML

#include "wx/window.h"
#include "wx/scrolwin.h"
#include "wx/config.h"
#include "CWinPars.h"
#include "CHtmlCell.h"
#include "wx/filesys.h"
#include "CHtmlFilt.h"
#include "wx/filename.h"
#include "wx/bitmap.h"
#include "wx/dcbuffer.h"

class CHtmlProcessor;
class CHtmlWinModule;
class CHtmlProcessorList;
class WXDLLIMPEXP_HTML CHtmlWinAutoScrollTimer;


// CHtmlWindow flags:
#define wxHW_SCROLLBAR_NEVER    0x0002
#define wxHW_SCROLLBAR_AUTO     0x0004
#define wxHW_NO_SELECTION       0x0008

#define wxHW_DEFAULT_STYLE      wxHW_SCROLLBAR_AUTO


// enums for CHtmlWindow::OnOpeningURL
enum CHtmlOpeningStatus
{
    wxHTML_OPEN,
    wxHTML_BLOCK,
    wxHTML_REDIRECT
};

// ----------------------------------------------------------------------------
// CHtmlWindow
//                  (This is probably the only class you will directly use.)
//                  Purpose of this class is to display HTML page (either local
//                  file or downloaded via HTTP protocol) in a window. Width of
//                  window is constant - given in constructor - virtual height
//                  is changed dynamicly depending on page size.  Once the
//                  window is created you can set it's content by calling
//                  SetPage(text) or LoadPage(filename).
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_HTML CHtmlWindow : public wxScrolledWindow
{
    DECLARE_DYNAMIC_CLASS(CHtmlWindow)
    friend class CHtmlWinModule;

public:
    CHtmlWindow() { Init(); }
    CHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHW_DEFAULT_STYLE,
                 const wxString& name = wxT("htmlWindow"))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }
    ~CHtmlWindow();

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHW_SCROLLBAR_AUTO,
                const wxString& name = wxT("htmlWindow"));

    // Set HTML page and display it. !! source is HTML document itself,
    // it is NOT address/filename of HTML document. If you want to
    // specify document location, use LoadPage() istead
    // Return value : false if an error occurred, true otherwise
    bool SetPage(const wxString& source);

    // Append to current page
    bool AppendToPage(const wxString& source);

    // Load HTML page from given location. Location can be either
    // a) /usr/wxGTK2/docs/html/wx.htm
    // b) http://www.somewhere.uk/document.htm
    // c) ftp://ftp.somesite.cz/pub/something.htm
    // In case there is no prefix (http:,ftp:), the method
    // will try to find it itself (1. local file, then http or ftp)
    // After the page is loaded, the method calls SetPage() to display it.
    // Note : you can also use path relative to previously loaded page
    // Return value : same as SetPage
    virtual bool LoadPage(const wxString& location);

    // Loads HTML page from file
    bool LoadFile(const wxFileName& filename);

    // Returns full location of opened page
    wxString GetOpenedPage() const {return m_OpenedPage;}
    // Returns anchor within opened page
    wxString GetOpenedAnchor() const {return m_OpenedAnchor;}
    // Returns <TITLE> of opened page or empty string otherwise
    wxString GetOpenedPageTitle() const {return m_OpenedPageTitle;}

    // Sets fonts to be used when displaying HTML page.
    void SetFonts(wxString normal_face, wxString fixed_face,
                  const int *sizes = NULL);

    // Sets font sizes to be relative to the given size or the system
    // default size; use either specified or default font
    void SetStandardFonts(int size = -1,
                          const wxString& normal_face = wxEmptyString,
                          const wxString& fixed_face = wxEmptyString);

    // Sets space between text and window borders.
    void SetBorders(int b) {m_Borders = b;}

    // Sets the bitmap to use for background (currnetly it will be tiled,
    // when/if we have CSS support we could add other possibilities...)
    void SetBackgroundImage(const wxBitmap& bmpBg) { m_bmpBg = bmpBg; }

    // Returns pointer to conteiners/cells structure.
    // It should be used ONLY when printing
    CHtmlContainerCell* GetInternalRepresentation() const {return m_Cell;}

    // Adds input filter
    static void AddFilter(CHtmlFilter *filter);

    // Returns a pointer to the parser.
    CHtmlWinParser *GetParser() const { return m_Parser; }

    // Adds HTML processor to this instance of CHtmlWindow:
    void AddProcessor(CHtmlProcessor *processor);
    // Adds HTML processor to CHtmlWindow class as whole:
    static void AddGlobalProcessor(CHtmlProcessor *processor);


    // -- Callbacks --

    // Called when the mouse hovers over a cell: (x, y) are logical coords
    // Default behaviour is to do nothing at all
    virtual void OnCellMouseHover(CHtmlCell *cell, wxCoord x, wxCoord y);

    // Called when user clicks on a cell. Default behavior is to call
    // OnLinkClicked() if this cell corresponds to a hypertext link
    virtual void OnCellClicked(CHtmlCell *cell,
                               wxCoord x, wxCoord y,
                               const wxMouseEvent& event);

    // Called when user clicked on hypertext link. Default behavior is to
    // call LoadPage(loc)
    virtual void OnLinkClicked(const CHtmlLinkInfo& link);

    // Called when CHtmlWindow wants to fetch data from an URL (e.g. when
    // loading a page or loading an image). The data are downloaded if and only if
    // OnOpeningURL returns true. If OnOpeningURL returns wxHTML_REDIRECT,
    // it must set *redirect to the new URL
    virtual CHtmlOpeningStatus OnOpeningURL(CHtmlURLType WXUNUSED(type),
                                             const wxString& WXUNUSED(url),
                                             wxString *WXUNUSED(redirect)) const
        { return wxHTML_OPEN; }

#if wxUSE_CLIPBOARD
    // Helper functions to select parts of page:
    void SelectWord(const wxPoint& pos);
    void SelectLine(const wxPoint& pos);
    void SelectAll();

    // Convert selection to text:
    wxString SelectionToText() { return DoSelectionToText(m_selection); }

    // Converts current page to text:
    wxString ToText();
#endif // wxUSE_CLIPBOARD

protected:
    void Init();

    // Scrolls to anchor of this name. (Anchor is #news
    // or #features etc. it is part of address sometimes:
    // http://www.ms.mff.cuni.cz/~vsla8348/wxhtml/index.html#news)
    // Return value : true if anchor exists, false otherwise
    bool ScrollToAnchor(const wxString& anchor);

    // Prepares layout (= fill m_PosX, m_PosY for fragments) based on
    // actual size of window. This method also setup scrollbars
    void CreateLayout();

    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);

#if wxUSE_CLIPBOARD
    void OnKeyUp(wxKeyEvent& event);
    void OnDoubleClick(wxMouseEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
#endif // wxUSE_CLIPBOARD

    virtual void OnInternalIdle();

    // Returns new filter (will be stored into m_DefaultFilter variable)
    virtual CHtmlFilter *GetDefaultFilter() {return new CHtmlFilterPlainText;}

    // cleans static variables
    static void CleanUpStatics();

    // Returns true if text selection is enabled (wxClipboard must be available
    // and wxHW_NO_SELECTION not used)
    bool IsSelectionEnabled() const;

    enum ClipboardType
    {
        Primary,
        Secondary
    };

    // Copies selection to clipboard if the clipboard support is available
    //
    // returns true if anything was copied to clipboard, false otherwise
    bool CopySelection(ClipboardType t = Secondary);

#if wxUSE_CLIPBOARD
    // Automatic scrolling during selection:
    void StopAutoScrolling();
#endif // wxUSE_CLIPBOARD

	void AssignIntDataToCells(const int data, const CHtmlCell* StartCell, const CHtmlCell* EndCell);
	void AssignPtrDataToCells(const void* data, const CHtmlCell* StartCell, const CHtmlCell* EndCell);
protected:
    wxString DoSelectionToText(CHtmlSelection *sel);

    // This is pointer to the first cell in parsed data.  (Note: the first cell
    // is usually top one = all other cells are sub-cells of this one)
    CHtmlContainerCell *m_Cell;
    // parser which is used to parse HTML input.
    // Each CHtmlWindow has it's own parser because sharing one global
    // parser would be problematic (because of reentrancy)
    CHtmlWinParser *m_Parser;
    // contains name of actualy opened page or empty string if no page opened
    wxString m_OpenedPage;
    // contains name of current anchor within m_OpenedPage
    wxString m_OpenedAnchor;
    // contains title of actualy opened page or empty string if no <TITLE> tag
    wxString m_OpenedPageTitle;
    // class for opening files (file system)
    wxFileSystem* m_FS;

    // borders (free space between text and window borders)
    // defaults to 10 pixels.
    int m_Borders;

    int m_Style;

    // current text selection or NULL
    CHtmlSelection *m_selection;

    // true if the user is dragging mouse to select text
    bool m_makingSelection;

#if wxUSE_CLIPBOARD
    // time of the last doubleclick event, used to detect tripleclicks
    // (tripleclicks are used to select whole line):
    wxLongLong m_lastDoubleClick;

    // helper class to automatically scroll the window if the user is selecting
    // text and the mouse leaves CHtmlWindow:
    CHtmlWinAutoScrollTimer *m_timerAutoScroll;
#endif // wxUSE_CLIPBOARD

private:
    // window content for double buffered rendering:
    wxBitmap *m_backBgBuffer;
	wxBitmap *m_backBuffer;

	// background image, may be invalid
    wxBitmap m_bmpBg;

    // variables used when user is selecting text
    wxPoint     m_tmpSelFromPos;
    CHtmlCell *m_tmpSelFromCell;

    // contains last link name
    CHtmlLinkInfo *m_tmpLastLink;
    // contains the last (terminal) cell which contained the mouse
    CHtmlCell *m_tmpLastCell;
    // if >0 contents of the window is not redrawn
    // (in order to avoid ugly blinking)
    int m_tmpCanDrawLocks;

    // list of HTML filters
    static wxList m_Filters;
    // this filter is used when no filter is able to read some file
    static CHtmlFilter *m_DefaultFilter;

    // html processors array:
    CHtmlProcessorList *m_Processors;
    static CHtmlProcessorList *m_GlobalProcessors;

    // a flag indicated if mouse moved
    // (if true we will try to change cursor in last call to OnIdle)
    bool m_tmpMouseMoved;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(CHtmlWindow)
};


#endif // wxUSE_HTML

#endif // _WX_HTMLWIN_H_
