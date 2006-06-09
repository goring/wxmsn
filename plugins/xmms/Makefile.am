bin_PROGRAMS = @PLUGINS@
EXTRA_PROGRAMS = wxmsnxmms.so wxmsnaudacious.so wxmsnbmp.so
AM_CFLAGS =\
	 -Wall\
	 -Wl,--export-dynamic\
	 -O2\
	 -shared\
	 -fPIC

wxmsnxmms_so_SOURCES = wxmsnxmms.c
wxmsnaudacious_so_SOURCES = wxmsnxmms.c
wxmsnbmp_so_SOURCES = wxmsnxmms.c

wxmsnxmms_so_LDFLAGS = @XMMS_LIBS@
wxmsnaudacious_so_LDFLAGS = @AUDACIOUS_LIBS@
wxmsnbmp_so_LDFLAGS = @BMP_LIBS@

wxmsnxmms_so_CFLAGS = @XMMS_CFLAGS@  -fPIC
wxmsnaudacious_so_CFLAGS = @AUDACIOUS_CFLAGS@ -fPIC -DBMP
wxmsnbmp_so_CFLAGS = @BMP_CFLAGS@  -fPIC -DBMP
	
