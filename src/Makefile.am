SUBDIRS = html

WXMSNHTML_LIB = $(top_builddir)/src/html/libwxmsnhtml.la

INCLUDES = \
	-I$(top_builddir)/include\
	-I$(top_builddir)/include/html

AM_CXXFLAGS =\
	$(WX_CXXFLAGS)
	
AM_CPPFLAGS = \
	-Wall

AM_LDFLAGS = \
	$(WX_LDFLAGS)

bin_PROGRAMS = wxmsn

wxmsn_SOURCES = \
	CContact.cc\
	CContactList.cc\
	CGroup.cc\
	CPopup.cc\
	CData.cc\
	chl.c\
	md5.c\
	sha1.c\
	CConsole.cxx\
	CGui.cxx\
	CConnec.cxx\
	CSsl.cxx\
	CParseSocket.cxx\
	CMsnAuth.cxx\
	Utils.cxx\
	CChatFrame.cxx\
	CChatTab.cxx \
	CChatArea.cc \
	CEntryText.cc \
	CContactFrame.cxx \
	CCoreHandler.cxx \
	CConnectPanel.cxx \
	Events.cxx \
	CListEvent.cxx \
	base64.c \
	SCPrefs.cxx


wxmsn_DEPENDENCIES = $(WXMSNHTML_LIB)

wxmsn_LDADD =  $(WXMSNHTML_LIB) $(WX_LIBS)
