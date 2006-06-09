#!/bin/bash

#   Copyright (c) 2005 Neil "Superna" ARMSTRONG
#   This file is part of wxMSN.
#
#   wxMSN is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   wxMSN is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with wxMSN; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#   Amarok Plugin for wxMSN

state=$(dcop amarok player status)

wxpath=$HOME"/.wxmsn/"
wxfile=$wxpath"songplayed"

if [ "$state" = 1 ]; then
	state="paused"
elif [ "$state" = 2 ]; then
	state="playing"
else
	state="empty"
fi

input=trackChange

while [ 1 ]
do
	if [ ${input:0:17} = engineStateChange ]; then
		state=${input:18}
		input=trackChange
	fi
	
	if [ "$input" = trackChange ] || [ ${#state} > 0 ]; then

		artist=$(dcop amarok player artist)
		title=$(dcop amarok player title)

		if ! [ -b $wxfile ]; then
			if ! [ -d $wxpath ]; then
				mkdir -p $wxpath
			fi
			touch $wxfile
		fi

		echo -e "$title\n$artist" > $wxfile
		
	fi
	
	read input
	
done
