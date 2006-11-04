# Copyright 2006 Neil "Superna" ARMSTRONG
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.

import rhythmdb, rb
import gobject, gtk

class SamplePython(rb.Plugin):

	def __init__(self):
		rb.Plugin.__init__(self)
			
	def activate(self, shell):
		print "activating sample python plugin"

		db = shell.get_property("db")
		model = db.query_model_new_empty()
		self.source = gobject.new (PythonSource, shell=shell, name=_("Python Source"), query_model=model)
		shell.append_source(self.source, None)
	
	def deactivate(self, shell):
		print "deactivating sample python plugin"
		self.source.delete_thyself()
		self.source = None


class PythonSource(rb.Source):
	def __init__(self):
		rb.Source.__init__(self)
		
gobject.type_register(PythonSource)
