/*  Copyright (c) 2005 Neil "Superna" ARMSTRONG
    This file is part of wxMSN.

    wxMSN is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    wxMSN is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wxMSN; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
    * *********************** *
    * based on hello world
    * a sample xmms plugin
    *
    * iso:crash 01/2002 <iso@kapsobor.de>
    * 
    * ******************** 
*/

/* include required header files */

#include <pthread.h>
#include <gtk/gtk.h>
#ifndef BMP
	#include <xmmsctrl.h>
	#include <plugin.h>
#else
	#include <bmp/beepctrl.h>
	#include <bmp/plugin.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define SLEEPTIME 10

/* for the sleep() funtion */
#include <unistd.h>

#define VERSION "0.1.0"

/* the configuration file (xmms handles it very comfortably for us) */
#define WXFILE "/.wxmsn/songplayed"

/* the funtions */
void *wxmsnxmms_thread(void *args);
void wxmsnxmms_init(void);
void wxmsnxmms_writedata();
void wxmsnxmms_cleanup(void);

static pthread_t tid;

/* the message to be displayed */
gboolean wasplaying = 0;
char path[1024];

/* the structures for InputPlugins etc can be found in 'plugin.h' */
GeneralPlugin wxmsnxmms =
{
	NULL,
	NULL,
	-1,
#ifndef BMP
	"wxMsn XMMS Plugin "VERSION,
#else
	"wxMsn BMP Plugin "VERSION,
#endif
	wxmsnxmms_init,
	NULL,
	NULL,
	wxmsnxmms_cleanup,
};

/* simply stop the running thread if running plugin gets disabled */
void wxmsnxmms_cleanup(void)
{
	pthread_cancel (tid);
}

/* required by xmms to display info line */
GeneralPlugin *get_gplugin_info(void)
{
	return (&wxmsnxmms);
}

/* initialization of the plugin */
void wxmsnxmms_init(void)
{
	strcpy(path, getenv("HOME"));
	strcat(path, WXFILE);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	pthread_create(&tid, &attr, wxmsnxmms_thread, NULL);
	return;
}

/* the main part */
void *wxmsnxmms_thread(void *args)
{
	while(1)
	{
        int fd;
        gchar * title;
        if(xmms_remote_is_playing(wxmsnxmms.xmms_session)==1)
        {
            int pos = xmms_remote_get_playlist_pos(wxmsnxmms.xmms_session);
            fd = open(path, O_WRONLY | O_TRUNC);
            if(fd >= 0)
	    {
	    	title = xmms_remote_get_playlist_title(wxmsnxmms.xmms_session, pos);
            	pos = write(fd, title, strlen(title));
            	write(fd, "\n", 1);
	    }
	    else
		    printf("** wxMSN : unable to open %s ***\n", path);
	    close(fd);
            wasplaying = 1;
        }
        else if(wasplaying)
        {
            fd = open(path, O_WRONLY | O_TRUNC);
	    if(fd < 0)
		    printf("** wxMSN : unable to open %s ***\n", path);
            close(fd);
            wasplaying = 0;
		}
		sleep(SLEEPTIME);
	}
}
