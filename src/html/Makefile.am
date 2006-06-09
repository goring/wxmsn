noinst_LTLIBRARIES = libwxmsnhtml.la
libwxmsnhtml_la_SOURCES = CHtmlCell.cpp CHtmlPars.cpp CHtmlTag.cpp\
                          CWinPars.cpp m_fonts.cpp  m_image.cpp m_links.cpp\
                          m_pre.cpp m_tables.cpp CHtmlFilt.cpp CHtmlPrint.cpp\
                          CHtmlWin.cpp m_dflist.cpp m_hline.cpp m_layout.cpp\
                          m_list.cpp m_style.cpp

AM_LDFLAGS = $(WX_LDFLAGS)

AM_CXXFLAGS = $(WX_CXXFLAGS)

AM_CPPFLAGS = -Wall

INCLUDES = -I$(top_builddir)/include/html
