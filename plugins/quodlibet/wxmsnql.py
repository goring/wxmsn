# Copyright 2006 Neil "Superna" ARMSTRONG
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.

from os import system, popen, path
from string import join
from qltk import Frame
import gtk, config

WXFILE = '~/.wxmsn/songplayed'

class wxMsnCurrentSong(object):
    PLUGIN_NAME = 'wxMsn Current Song'
    PLUGIN_DESC = 'Change wxMsn current song according to what you are listening now.'
    PLUGIN_VERSION = '0.1'

    def __init__(self):
        gtk.quit_add(0, self.quit)

    def quit(self):
        self.change_status('', '')

    def change_status(self, title, artist):
        file = open(path.expanduser(WXFILE), 'w')
        file.write(title + '\n' + artist);

    def plugin_on_song_started(self, song):
        if song == None:
           self.change_status('', '')
        else:
            try:
                self.change_status(song.__getitem__('title'), song.__getitem__('artist'));
            except KeyError:
                self.change_status('','');

    def plugin_on_song_ended(self, song, stopped):
        self.change_status('', '')
