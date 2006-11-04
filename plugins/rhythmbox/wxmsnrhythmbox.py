# Copyright 2006 Neil "Superna" ARMSTRONG
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.

from os import system, popen, path
import rhythmdb, rb
import gobject, gtk

WXFILE = '~/.wxmsn/songplayed'

class wxMsnRhythmbox(rb.Plugin):

	def __init__(self):
		rb.Plugin.__init__(self)
			
	def activate(self, shell):
		self.shell = shell
		sp = shell.get_player ()
		self.pec_id = sp.connect ('playing-song-changed', self.playing_entry_changed)
		self.pc_id = sp.connect ('playing-changed', self.playing_changed)
		self.set_entry(sp.get_playing_entry ())
	
	def deactivate(self, shell):
		self.do_write('', '')
		self.shell = None
		sp = shell.get_player ()
		sp.disconnect (self.pec_id)
		sp.disconnect (self.pc_id)

	def playing_changed (self, sp, playing):
		self.set_entry(sp.get_playing_entry ())

	def playing_entry_changed (self, sp, entry):
		self.set_entry(entry)

	def set_entry(self, entry):
		if entry is not None:
			db = self.shell.get_property ("db")
			st_artist = db.entry_get (entry, rhythmdb.PROP_ARTIST) or _("")
			st_title = db.entry_get (entry, rhythmdb.PROP_TITLE) or _("")
			self.do_write(st_title, st_artist)	
		else:
			self.do_write('', '')

	def do_write(self, title, artist):
		file = open(path.expanduser(WXFILE), 'w')
		if artist is not '':
        		file.write(title + '\n' + artist);
		else:
			file.write(title);

