/*  Copyright (c) 2005 Julien "Trapamoosch" ENCHE
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
*/

/*
    $Id$
*/

#ifndef _ARRAYDECLARATION_H_
#define _ARRAYDECLARATION_H_

#include "wx/wx.h"
#include "wx/dynarray.h"

class CGroup;
class CContact;
WX_DEFINE_ARRAY(CContact*, ContactArray);
WX_DEFINE_ARRAY(CGroup*, GroupArray);

#endif
