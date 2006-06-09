/* Created by Anjuta version 1.2.4 */
/*	This file will not be overwritten */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include "CHtmlWin.h"

class MyFrame : public wxFrame
{
	public:
		MyFrame();
//		~MyFrame();
};

MyFrame::MyFrame() : wxFrame(NULL, -1, wxT("Test"))
{
	CHtmlWindow* Html = new CHtmlWindow(this, -1);
	Html->SetPage(wxT("<html><body background='/home/trapamoosch/Projects/wxmsn/trunk/src/resources/avatar.png'><font face='Verdana' color='#FF0000'>test</font><br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br>test<br></body></html>"));
}

// -----------------------------------

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();
	MyFrame *frame = new MyFrame();
    SetTopWindow(frame);
	frame->Show(true);
	return true;
}
